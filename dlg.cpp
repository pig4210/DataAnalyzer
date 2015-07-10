#include "dlg.h"

#include <algorithm>
#include <fstream>

#include <xlib.h>

#include <lua.hpp>
#pragma comment(lib, "lua")

using namespace std;

using namespace DataAnalyzer;


static const wchar_t* const gk_app_name       = L"数据解析器";
static const wchar_t* const gk_app_ver        = L"ver 2.0";
static const wchar_t* const gk_author         = L"by triones";
static const wchar_t* const gk_copyright      = L"All rights reserved (c)";

static const char* const gk_cfg_file_name     = "cfg";
static const char* const gk_port_name         = "da_port";
static const char* const gk_left_name         = "da_left";
static const char* const gk_top_name          = "da_top";
static const char* const gk_width_name        = "da_width";
static const char* const gk_height_name       = "da_height";
static const char* const gk_data_width_name   = "da_data_width";
static const char* const gk_data_height_name  = "da_data_height";
static const char* const gk_num_width_name    = "da_num_width";
static const char* const gk_ins_widht_name    = "da_ins_width";

static const char* const gk_analyzer_file_name = "analyzer";

static bool g_listen                          = false;        //!< 监听标志
static unsigned short g_port                  = 42108;        //!< 监听端口
static HANDLE g_thd                           = nullptr;      //!< 监听线程

//! 信息输出缓冲，信息数据输出在窗口下方。dlg_show_info函数写入，tm_Tick函数读出
static xmsg g_info;
struct Data_ST                                //!< 数据解析结构
  {
  string introduction;                        //!< 简要说明
  string datas;                               //!< 详细数据
  };
//! 数据解析缓冲，输出在DataGridView。监听线程写入，tm_Tick函数读出
static vector<Data_ST> g_analy_data;
static SysCritical g_cri;                     //!< 多线程操作CLR同步，同时用于两种缓冲数据的互斥访问

//! 唯一Lua状态，在FormLoad时生成，FormClosing时销毁
static lua_State* xlua                        = nullptr;
//! 用于Lua状态的互斥访问，多数情况下由监听线程访问，但拖放脚本时，由窗口线程访问
static SysCritical xlua_cri;

#undef xlog_static_lvl
#define xlog_static_lvl xlog::lvl_debug

//! 获取当前模块全路径，包含"\"
static string get_this_path()
  {
  HMODULE hMod;
  auto rets = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                (LPCTSTR)&get_this_path,
                                &hMod);
  if(!rets)
    {
    throw runtime_error(
      xmsg() << "获取当前模块位置失败:" << (intptr_t)GetLastError()
      );
    }
  xtrace << "当前模块位置:" << hMod;

  char path[MAX_PATH];

  if(0 == GetModuleFileNameA(hMod, path, _countof(path)))
    {
    throw runtime_error(
      xmsg() << "获取当前模块文件名失败:" << (intptr_t)GetLastError()
      );
    }

  xtrace << "当前模块文件名:" << path;

  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  auto en = _splitpath_s(path, drive, dir, fname, ext);
  if(0 != en)
    {
    throw runtime_error(
      xmsg() << "分解当前模块路径失败:" << strerror(en)
      );
    }

  return string(drive) + string(dir);
  }

//! 向Lua状态的package.path、package.cpath加入当前模块路径，有相同判定，不重复添加
static void lua_append_this_path(lua_State* ls)
  {
  const string path(get_this_path());
  xtrace << "path:" << path;

  xmsg lua_path;
  lua_path << path << "?.lua";
  string lp(lua_path);
  transform(lp.begin(), lp.end(), lp.begin(), tolower);
  xtrace << "lua_path:" << lua_path << " -- " << lp;

  xmsg dll_path;
  dll_path << path << "?.dll";
  string dp(dll_path);
  transform(dp.begin(), dp.end(), dp.begin(), tolower);
  xtrace << "dll_path:" << dll_path << " -- " << dp;

  lua_getglobal(ls, LUA_LOADLIBNAME);

  lua_pushstring(ls, "path");
  lua_gettable(ls, -2);
  string pkp(lua_tostring(ls, -1));
  lua_pop(ls, 1);
  string pp(pkp);
  transform(pp.begin(), pp.end(), pp.begin(), tolower);
  xtrace << "package.path:" << pkp;
  xtrace << "package.path:" << pp;

  lua_pushstring(ls, "cpath");
  lua_gettable(ls, -2);
  string pkc(lua_tostring(ls, -1));
  lua_pop(ls, 1);
  string pc(pkc);
  transform(pc.begin(), pc.end(), pc.begin(), tolower);
  xtrace << "package.cpath:" << pkc;
  xtrace << "package.cpath:" << pc;

  if(string::npos == pp.find(lp))
    {
    xmsg ss;
    ss << pkp << ';' << lua_path;
    lua_pushstring(ls, "path");
    lua_pushstring(ls, ss.c_str());
    lua_settable(ls, -3);
    xtrace << "Append package.path";
    }

  if(string::npos == pc.find(dp))
    {
    xmsg ss;
    ss << pkc << ';' << dll_path;
    lua_pushstring(ls, "cpath");
    lua_pushstring(ls, ss.c_str());
    lua_settable(ls, -3);
    xtrace << "Append package.cpath";
    }
  lua_pop(ls, lua_gettop(ls));
  }

//! udp对象，这样的设计好处在于，允许在建立UDP对象前，预读取配置，修改指定的监听端口
xUDP& da_udp()
  {
  static xUDP g_da_udp(AddrInfo(0x00000000, g_port));
  return g_da_udp;
  }

//! 用于向窗口输出消息，数据写入缓冲，等待tim_Tick读取
static void dlg_show_info(const char* msg)
  {
  if(msg == nullptr)  return;
  g_cri.enter();
  try
    {
    if(!g_info.empty())
      {
      g_info << "\r\n";
      }
    g_info << msg;
    }
  catch(...)
    {

    }
  g_cri.leave();
  }

static void dlg_show_info(const string& msg)
  {
  return dlg_show_info(msg.c_str());
  }

//! 控制面板隐藏，阻止cancel事件
System::Void dlg::Btn_Hide_Click(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  if(PL_Data->Visible)
    {
    PL_Data->Visible = false;
    }
  }

//! 保存配置
System::Void dlg::dlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  xmsg msg;
  msg << gk_port_name << " = " << (int)g_port << ";\r\n";
  msg << gk_left_name << " = " << (int)Left << ";\r\n";
  msg << gk_top_name << " = " << (int)Top << ";\r\n";
  msg << gk_width_name << " = " << (int)Width << ";\r\n";
  msg << gk_height_name << " = " << (int)Height << ";\r\n";
  msg << gk_data_width_name << " = " << (int)PL_Data->Width << ";\r\n";
  msg << gk_data_height_name << " = " << (int)PL_Data->Height << ";\r\n";
  msg << gk_num_width_name << " = " << (int)Col_Num->Width << ";\r\n";
  msg << gk_ins_widht_name << " = " << (int)Col_Introduction->Width << ";\r\n";

  xmsg filename;
  filename << get_this_path() << gk_cfg_file_name << ".lua";
  ofstream file(filename.c_str());
  file << msg.c_str();
  file.close();

  if(g_thd)
    {
    TerminateThread(g_thd, 0);
    CloseHandle(g_thd);
    }
  if(xlua)  lua_close(xlua);
  }

//! 窗口关闭
System::Void dlg::Btn_Close_Click(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  Close();
  }

//! 窗口最小化
System::Void dlg::Btn_Min_Click(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  this->WindowState = FormWindowState::Minimized;
  }

//! 窗口移动、最大化/还原控制
System::Void dlg::LB_Pos_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  static unsigned int old_tick = 0;
  const unsigned int now_tick = GetTickCount();
  if((now_tick - old_tick) <= GetDoubleClickTime())
    {
    if(WindowState == FormWindowState::Maximized)
      {
      WindowState = FormWindowState::Normal;
      }
    else
      {
      WindowState = FormWindowState::Maximized;
      }
    return;
    }
  old_tick = now_tick;
  ReleaseCapture();
  SendMessage((HWND)this->Handle.ToPointer(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
  }

//! 解析脚本，使用了Lua状态，注意外部互斥
static bool analyzer(const netline& buf, Data_ST& da)
  {
  try
    {
    bool done = false;
    lua_getglobal(xlua, "analyzer");
    if(lua_type(xlua, -1) != LUA_TFUNCTION)
      {
      lua_pop(xlua, lua_gettop(xlua));
      return false;
      }
    lua_pushlstring(xlua, (const char*)buf.c_str(), buf.size());

    if(LUA_OK == lua_pcall(xlua, 1, 2, 0))
      {
      if(lua_type(xlua, -1) != LUA_TNIL)
        {
        da.datas = lua_tostring(xlua, -1);
        da.introduction = lua_tostring(xlua, -2);
        done = true;
        }
      }
    else
      {
      dlg_show_info(lua_tostring(xlua, -1));
      }

    lua_pop(xlua, lua_gettop(xlua));
    return done;
    }
  catch(const runtime_error& err)
    {
    dlg_show_info(xmsg() << "数据解析出错：" << err.what());
    }
  catch(...)
    {
    dlg_show_info("数据解析出现异常");
    }
  return false;
  }

//! 监听线程，调用可能的解析脚本，如果不能解析，默认解析
static DWORD WINAPI Thd(PVOID lParam)
  {
  UNREFERENCED_PARAMETER(lParam);
  try
    {
    Data_ST da;
    netline& buf = da_udp().recvbuf;
    while(da_udp().recv() != 0)
      {
      if(buf.empty()) continue;
      if(g_listen)
        {
        xlua_cri.enter();

        const bool done = analyzer(buf, da);

        xlua_cri.leave();

        if(!done)
          {
          const unsigned char* lp = (const unsigned char*)buf.c_str();
          xmsg msg;
          msg << lp[0] << ' ' << lp[1] << ' ' << lp[2] << ' ' << lp[3];
          da.introduction = msg;
          da.datas = hex2show(buf.c_str(), buf.size());
          }

        if(!da.introduction.empty())
          {
          g_cri.enter();
          try
            {
            g_analy_data.push_back(da);
            }
          catch(...)
            {

            }
          g_cri.leave();
          }
        }
      buf.clear();
      }

    dlg_show_info("工作线程结束");
    }
  catch(const runtime_error& err)
    {
    dlg_show_info(xmsg() << "监听线程出错：" << err.what());
    }
  catch(...)
    {
    dlg_show_info("监听线程出现异常");
    }
  return 0;
  }

//! 初始化Lua状态，读取配置
System::Void dlg::dlg_Load(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  //做标题
  wstring title;
  title += gk_app_name;
  title += L"  ";
  title += gk_app_ver;
  title += L"  ";
  title += gk_author;
  title += L"  ";
  title += gk_copyright;
  this->Text = gcnew System::String(title.c_str());

  xlua = luaL_newstate();

  luaL_openlibs(xlua);

  try
    {
    lua_append_this_path(xlua);

    //读取配置
    lua_getglobal(xlua, "require");
    if(lua_type(xlua, -1) != LUA_TFUNCTION)
      {
      dlg_show_info(xmsg() << "提取require函数出错:" << lua_type(xlua, -1));
      lua_pop(xlua, lua_gettop(xlua));
      }
    else
      {
      lua_pushstring(xlua, gk_cfg_file_name);
      if(LUA_OK != lua_pcall(xlua, 1, LUA_MULTRET, 0))
        {
        dlg_show_info(xmsg() << "加载配置出错:" << lua_tostring(xlua, -1));
        lua_pop(xlua, lua_gettop(xlua));
        }
      else
        {
        lua_pop(xlua, lua_gettop(xlua));

        lua_getglobal(xlua, gk_port_name);
        const unsigned short port = luaL_optinteger(xlua, 1, 0);
        lua_pop(xlua, lua_gettop(xlua));
        if(port != 0 && port != g_port)
          {
          g_port = port;
          }

        const int err_def = 0x80000000;

        lua_getglobal(xlua, gk_left_name);
        const int left = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(left != err_def && left < System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width)
          {
          Left = left;
          }

        lua_getglobal(xlua, gk_top_name);
        const int top = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(top != err_def && top < System::Windows::Forms::Screen::PrimaryScreen->Bounds.Height)
          {
          Top = top;
          }

        lua_getglobal(xlua, gk_width_name);
        const int width = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(width > Width && width < System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width)
          {
          Width = width;
          }

        lua_getglobal(xlua, gk_height_name);
        const int height = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(height > Height && height < System::Windows::Forms::Screen::PrimaryScreen->Bounds.Height)
          {
          Height = height;
          }

        lua_getglobal(xlua, gk_data_width_name);
        const int data_width = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(data_width > 0 && data_width < Width)
          {
          PL_Data->Width = data_width;
          }

        lua_getglobal(xlua, gk_data_height_name);
        const int data_height = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(data_height > 0 && data_height < Height)
          {
          PL_Data->Height = data_height;
          }

        dlg_SizeChanged(sender, e);

        lua_getglobal(xlua, gk_num_width_name);
        const int num_width = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(num_width > 0)
          {
          Col_Num->Width = num_width;
          }

        lua_getglobal(xlua, gk_ins_widht_name);
        const int ins_width = luaL_optinteger(xlua, 1, err_def);
        lua_pop(xlua, lua_gettop(xlua));
        if(ins_width > 0)
          {
          Col_Introduction->Width = ins_width;
          }

        lua_pop(xlua, lua_gettop(xlua));
        }
      }

    lua_getglobal(xlua, "require");
    if(lua_type(xlua, -1) != LUA_TFUNCTION)
      {
      dlg_show_info(xmsg() << "提取require函数出错:" << lua_type(xlua, -1));
      }
    else
      {
      lua_pushstring(xlua, gk_analyzer_file_name);
      if(LUA_OK != lua_pcall(xlua, 1, LUA_MULTRET, 0))
        {
        dlg_show_info(xmsg() << "加载解析脚本出错:" << lua_tostring(xlua, -1));
        }
      }
    lua_pop(xlua, lua_gettop(xlua));
    }
  catch(const runtime_error& err)
    {
    dlg_show_info(err.what());
    }
  catch(...)
    {
    dlg_show_info("初始化配置失败");
    }

  da_udp().recvbuf.reserve(0x2000);
  //开启监听
  Btn_Listen_Click(sender, e);

  //开启监听线程
  g_thd = CreateThread(nullptr, 0, Thd, nullptr, 0, nullptr);
  }

//! 监听/暂停
System::Void dlg::Btn_Listen_Click(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  g_listen = !g_listen;

  if(g_listen)
    {
    tm->Enabled = true;
    xmsg msg;
    msg << "正在UDP端口: " << (int)g_port << " 监听...";
    dlg_show_info(msg);
    }
  else
    {
    dlg_show_info("监听暂停中...");
    tm->Enabled = false;
    tm_Tick(sender, e);
    }
  }

//! 用于信息的清除
System::Void dlg::TB_Info_DoubleClick(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  TB_Info->Clear();
  tp->SetToolTip(TB_Info, TB_Info->Text);
  }

//! 根据窗口大小变化，调整控制面板、数据面板的位置居中
System::Void dlg::dlg_SizeChanged(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  if(this->WindowState == FormWindowState::Minimized)
    {
    return;
    }

  if(Width < Btn_Min->Right)
    {
    Width = Btn_Min->Right;
    }

  const int co_h = PL_Control->Height + DGV_Data->ColumnHeadersHeight + TB_Info->Height +
    this->Padding.Top + this->Padding.Bottom;
  if(Height < co_h)
    {
    Height = co_h;
    }

  if(PL_Data->Width > DGV_Data->Width)
    {
    PL_Data->Width = DGV_Data->Width;
    }
  if(PL_Data->Height > DGV_Data->Height - DGV_Data->ColumnHeadersHeight)
    {
    PL_Data->Height = DGV_Data->Height - DGV_Data->ColumnHeadersHeight;
    }

  const int top = (Height - PL_Data->Height) / 2;
  if(top != PL_Data->Top) PL_Data->Top = top;
  const int left = (Width - PL_Data->Width) / 2;
  if(left != PL_Data->Left) PL_Data->Left = left;

  }

#pragma unmanaged
unsigned __int64 rdtsc()
  {
  return __rdtsc();
  }
#pragma managed

//! 时钟用于刷新信息输出缓冲、数据输出缓冲
System::Void dlg::tm_Tick(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  if(g_info.empty() && g_analy_data.empty()) return;
  g_cri.enter();
  try
    {
    if(!g_info.empty())
      {
      if(!System::String::IsNullOrEmpty(TB_Info->Text))
        {
        TB_Info->AppendText("\r\n");
        }
      TB_Info->AppendText(gcnew System::String(g_info.c_str()));
      tp->SetToolTip(TB_Info, TB_Info->Text);
      g_info.clear();
      }
    if(!g_analy_data.empty())
      {
      System::Windows::Forms::DataGridViewRowCollection^ Rows = DGV_Data->Rows;
      for(auto da : g_analy_data)
        {
        const int index = Rows->Add();
        xmsg msg;
        msg << rdtsc();
        Rows[index]->Cells[0]->Value = gcnew System::String(msg.c_str());
        Rows[index]->Cells[1]->Value = gcnew System::String(da.introduction.c_str());
        Rows[index]->Cells[2]->Value = gcnew System::String(da.datas.c_str());
        }
      //自动滚动调整
      if(DGV_Data->SelectedRows->Count > 0)
        {
        if(DGV_Data->SelectedRows->Count == 1)
          {
          if(DGV_Data->SelectedRows[0] == DGV_Data->Rows[0])
            {
            DGV_Data->FirstDisplayedScrollingRowIndex = DGV_Data->Rows->Count - 1;
            }
          else if(DGV_Data->SelectedRows[0] == DGV_Data->Rows[DGV_Data->Rows->Count - 1 - g_analy_data.size()])
            {
            DGV_Data->CurrentCell = DGV_Data->Rows[DGV_Data->Rows->Count - 1]->Cells[0];
            }
          }
        }
      else
        {
        DGV_Data->FirstDisplayedScrollingRowIndex = DGV_Data->Rows->Count - 1;
        }
      g_analy_data.clear();
      }
    }
  catch(...)
    {

    }
  g_cri.leave();
  }

//! 指定数据，显示详细
System::Void dlg::DGV_Data_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  if(e->KeyData == Keys::Enter)
    {
    e->Handled = true;
    DGV_Data_DoubleClick(sender, e);
    }
  }

//! 指定数据，显示详细
System::Void dlg::DGV_Data_DoubleClick(System::Object^  sender, System::EventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  if(DGV_Data->SelectedRows->Count <= 0)  return;

  TC_Data->TabPages->Clear();

  for(int i = DGV_Data->SelectedRows->Count; i > 0; --i)
    {
    TabPage^ Page = gcnew TabPage();
    Page->Text = "[";
    Page->Text += DGV_Data->SelectedRows[i - 1]->Cells[0]->Value->ToString();
    Page->Text += "].";
    Page->Text += DGV_Data->SelectedRows[i - 1]->Cells[1]->Value->ToString();

    TextBox^ lb = gcnew TextBox();

    lb->Dock = DockStyle::Fill;
    lb->Multiline = true;
    lb->ReadOnly = true;
    lb->BorderStyle = BorderStyle::FixedSingle;
    lb->BackColor = Color::LightGray;
    lb->ForeColor = Color::Black;
    lb->ScrollBars = ScrollBars::Vertical;
    lb->Text = DGV_Data->SelectedRows[i - 1]->Cells[2]->Value->ToString();
    Page->Controls->Add(lb);

    Page->TabIndex = i - 1;
    TC_Data->TabPages->Add(Page);
    }

  PL_Data->Visible = true;
  TC_Data->Focus();
  }

//! 拖放事件
System::Void dlg::dlg_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);
  if(e->Data->GetDataPresent(DataFormats::FileDrop))
    {
    e->Effect = DragDropEffects::Link;
    return;
    }
  e->Effect = DragDropEffects::None;
  }
System::Void dlg::dlg_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  if(e->Data->GetDataPresent(DataFormats::FileDrop))
    {
    const char* filename = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(((Array^)e->Data->GetData(DataFormats::FileDrop))->GetValue(0)->ToString());

    xmsg msg;
    msg << "加载脚本:" << filename;
    dlg_show_info(msg.c_str());

    xlua_cri.enter();

    if(LUA_OK != luaL_loadfile(xlua, filename))
      {
      dlg_show_info(xmsg() << "读取解析脚本出错：" << lua_tostring(xlua, -1));
      }
    else
      {
      if(LUA_OK != lua_pcall(xlua, 0, 0, 0))
        {
        dlg_show_info(xmsg() << "加载解析脚本出错：" << lua_tostring(xlua, -1));
        }
      }
    lua_pop(xlua, lua_gettop(xlua));

    xlua_cri.leave();

    System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)(void*)filename);
    }
  }

System::Void dlg::PL_Data_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
  {
  UNREFERENCED_PARAMETER(sender);
  UNREFERENCED_PARAMETER(e);

  Panel^ pl = PL_Data;
  static Point g_PL_Data_Move_Start;

  const int bl = DGV_Data->Left;
  const int bt = DGV_Data->Top + DGV_Data->ColumnHeadersHeight;
  const int br = DGV_Data->Right;
  const int bb = DGV_Data->Bottom;
  const int bw = DGV_Data->Width;
  const int bh = DGV_Data->Height;

  if(e->Button != Windows::Forms::MouseButtons::None)
    {
    Point vPoint = PointToClient(MousePosition);
    if(pl->Cursor == Cursors::SizeNWSE)     //"\"标志
      {
      if(e->X < pl->Width / 2) //左上角的移动
        {
        //X坐标不能超过窗口左边界
        if(vPoint.X != pl->Left && vPoint.X >= bl)
          {
          const int w = pl->Right - vPoint.X;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            pl->Left = vPoint.X;
            }
          }
        //Y坐标不能超过窗口上边界
        if(vPoint.Y != pl->Top && vPoint.Y >= bt)
          {
          const int h = pl->Bottom - vPoint.Y;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            pl->Top = vPoint.Y;
            }
          }
        }
      else                    //右下角的移动
        {
        //X坐标不能超过窗口右边界
        if(vPoint.X != pl->Right && vPoint.X <= br)
          {
          const int w = vPoint.X - pl->Left;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            }
          }
        //Y坐标不能超过窗口下边界
        if(vPoint.Y != pl->Bottom && vPoint.Y <= bb)
          {
          const int h = vPoint.Y - pl->Top;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            }
          }
        }
      }
    else if(pl->Cursor == Cursors::SizeNESW)  //"/"标志
      {
      if(e->X < pl->Width / 2) //左下角的移动
        {
        //X坐标不能超过窗口左边界
        if(vPoint.X != pl->Left && vPoint.X >= bl)
          {
          const int w = pl->Right - vPoint.X;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            pl->Left = vPoint.X;
            }
          }
        //Y坐标不能超过窗口下边界
        if(vPoint.Y != pl->Bottom && vPoint.Y <= bb)
          {
          const int h = vPoint.Y - pl->Top;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            }
          }
        }
      else                     //右上角的移动
        {
        //X坐标不能超过窗口右边界
        if(vPoint.X != pl->Right && vPoint.X <= br)
          {
          const int w = vPoint.X - pl->Left;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            }
          }
        //Y坐标不能超过窗口上边界
        if(vPoint.Y != pl->Top && vPoint.Y >= bt)
          {
          const int h = pl->Bottom - vPoint.Y;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            pl->Top = vPoint.Y;
            }
          }
        }
      }
    else if(pl->Cursor == Cursors::SizeWE)  //"-"标志
      {
      if(e->X < pl->Width / 2) //左边的移动
        {
        //X坐标不能超过窗口左边界
        if(vPoint.X != pl->Left && vPoint.X >= bl)
          {
          const int w = pl->Right - vPoint.X;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            pl->Left = vPoint.X;
            }
          }
        }
      else                      //右边的移动
        {
        //X坐标不能超过窗口右边界
        if(vPoint.X != pl->Right && vPoint.X <= br)
          {
          const int w = vPoint.X - pl->Left;
          //修正的宽度不能过小，超过窗口右边界
          if(w <= bw && w >= 10)
            {
            pl->Width = w;
            }
          }
        }
      }
    else if(pl->Cursor == Cursors::SizeNS)  //"|"标志
      {
      if(e->Y < pl->Height / 2)     //上边的移动
        {
        //Y坐标不能超过窗口上边界
        if(vPoint.Y != pl->Top && vPoint.Y >= bt)
          {
          const int h = pl->Bottom - vPoint.Y;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            pl->Top = vPoint.Y;
            }
          }
        }
      else                          //下边的移动
        {
        //Y坐标不能超过窗口下边界
        if(vPoint.Y != pl->Bottom && vPoint.Y <= bb)
          {
          const int h = vPoint.Y - pl->Top;
          //修正高度不能过小，超过窗口下边界
          if(h <= bh && h >= 10)
            {
            pl->Height = h;
            }
          }
        }
      }
    else if(pl->Cursor == Cursors::SizeAll)
      {
      const int l = e->X - g_PL_Data_Move_Start.X + pl->Left;
      if(l >= bl && l <= (br - 10))
        {
        pl->Left = l;
        }
      const int t = e->Y - g_PL_Data_Move_Start.Y + pl->Top;
      if(t >= bt && t < (bb - 10))
        {
        pl->Top = t;
        }
      }
    return;
    }

  if(e->X <= 5)     //在左边
    {
    if(e->Y <= 5)   //在左上角
      {
      pl->Cursor = Cursors::SizeNWSE;
      return;
      }
    else if(e->Y >= pl->Height - 5)    //在左下角
      {
      pl->Cursor = Cursors::SizeNESW;
      }
    else
      {
      pl->Cursor = Cursors::SizeWE;
      }
    }
  else if(e->X >= pl->Width - 5)   //在右边
    {
    if(e->Y <= 5)                       //右上角
      {
      pl->Cursor = Cursors::SizeNESW;
      }
    else if(e->Y >= pl->Height - 5)  //右下角
      {
      pl->Cursor = Cursors::SizeNWSE;
      }
    else
      {
      pl->Cursor = Cursors::SizeWE;
      }
    }
  else if(e->Y <= 5)
    {
    pl->Cursor = Cursors::SizeNS;
    }
  else if(e->Y <= pl->Height / 2)
    {
    pl->Cursor = Cursors::SizeAll;
    g_PL_Data_Move_Start = e->Location;
    }
  else if(e->Y >= pl->Height - 5)
    {
    pl->Cursor = Cursors::SizeNS;
    }
  }

void dlg::WndProc(Message% m)
  {
  Form::WndProc(m);
  switch(m.Msg)
    {
    case WM_NCHITTEST:
      {
      if(WindowState == FormWindowState::Maximized) break;

      Point pPoint((int)m.LParam & 0xFFFF,
                   (int)m.LParam >> 16 & 0xFFFF);
      Point vPoint = PointToClient(pPoint);

      if(vPoint.X <= 5)
        {
        if(vPoint.Y <= 5)
          {
          m.Result = (IntPtr)HTTOPLEFT;
          }
        else if(vPoint.Y >= ClientSize.Height - 5)
          {
          m.Result = (IntPtr)HTBOTTOMLEFT;
          }
        else
          {
          m.Result = (IntPtr)HTLEFT;
          }
        }
      else if(vPoint.X >= ClientSize.Width - 5)
        {
        if(vPoint.Y <= 5)
          {
          m.Result = (IntPtr)HTTOPRIGHT;
          }
        else if(vPoint.Y >= ClientSize.Height - 5)
          {
          m.Result = (IntPtr)HTBOTTOMRIGHT;
          }
        else
          {
          m.Result = (IntPtr)HTRIGHT;
          }
        }
      else if(vPoint.Y <= 5)
        {
        m.Result = (IntPtr)HTTOP;
        }
      else if(vPoint.Y >= ClientSize.Height - 5)
        {
        m.Result = (IntPtr)HTBOTTOM;
        }
      }
      break;
    default:
      break;
    }
  }

[STAThreadAttribute]
int main(array<System::String^>^)
  {
  // 在创建任何控件之前启用 Windows XP 可视化效果
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  Application::Run(gcnew dlg());
  return 0;
  }
