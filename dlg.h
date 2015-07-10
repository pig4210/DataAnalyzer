#pragma once

namespace DataAnalyzer
  {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  /// <summary>
  /// dlg 摘要
  /// </summary>
  public ref class dlg : public System::Windows::Forms::Form
    {
    public:
      dlg(void)
        {
        InitializeComponent();
        //
        //TODO:  在此处添加构造函数代码
        //
        }

    protected:
      /// <summary>
      /// 清理所有正在使用的资源。
      /// </summary>
      ~dlg()
        {
        if(components)
          {
          delete components;
          }
        }
    protected:
      virtual void WndProc(Message% m) override;

    protected:
    private: System::Windows::Forms::Panel^         PL_Control;
    private: System::Windows::Forms::Button^        Btn_Close;
    private: System::Windows::Forms::Button^        Btn_Min;
    private: System::Windows::Forms::Label^         LB_Pos;
    private: System::Windows::Forms::Button^        Btn_Listen;
    private: System::Windows::Forms::DataGridView^  DGV_Data;
    private: System::Windows::Forms::Button^        Btn_Hide;
    private: System::Windows::Forms::TextBox^       TB_Info;
    private: System::Windows::Forms::ToolTip^       tp;
    private: System::Windows::Forms::Timer^         tm;
    private: System::Windows::Forms::Panel^         PL_Data;
    private: System::Windows::Forms::TabControl^    TC_Data;
    private: System::Windows::Forms::DataGridViewTextBoxColumn^  Col_Num;
    private: System::Windows::Forms::DataGridViewTextBoxColumn^  Col_Introduction;
    private: System::Windows::Forms::DataGridViewTextBoxColumn^  Col_Data;

    private: System::ComponentModel::IContainer^  components;


    private:
      /// <summary>
      /// 必需的设计器变量。
      /// </summary>

#pragma region Windows Form Designer generated code
      /// <summary>
      /// 设计器支持所需的方法 - 不要
      /// 使用代码编辑器修改此方法的内容。
      /// </summary>
      void InitializeComponent(void)
        {
        this->components = (gcnew System::ComponentModel::Container());
        System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
        System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
        System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
        System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
        System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
        System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(dlg::typeid));
        this->Btn_Close = (gcnew System::Windows::Forms::Button());
        this->Btn_Min = (gcnew System::Windows::Forms::Button());
        this->LB_Pos = (gcnew System::Windows::Forms::Label());
        this->Btn_Listen = (gcnew System::Windows::Forms::Button());
        this->PL_Control = (gcnew System::Windows::Forms::Panel());
        this->Btn_Hide = (gcnew System::Windows::Forms::Button());
        this->DGV_Data = (gcnew System::Windows::Forms::DataGridView());
        this->Col_Num = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
        this->Col_Introduction = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
        this->Col_Data = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
        this->TB_Info = (gcnew System::Windows::Forms::TextBox());
        this->tp = (gcnew System::Windows::Forms::ToolTip(this->components));
        this->tm = (gcnew System::Windows::Forms::Timer(this->components));
        this->PL_Data = (gcnew System::Windows::Forms::Panel());
        this->TC_Data = (gcnew System::Windows::Forms::TabControl());
        this->PL_Control->SuspendLayout();
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DGV_Data))->BeginInit();
        this->PL_Data->SuspendLayout();
        this->SuspendLayout();
        // 
        // Btn_Close
        // 
        this->Btn_Close->BackColor = System::Drawing::Color::Brown;
        this->Btn_Close->Dock = System::Windows::Forms::DockStyle::Left;
        this->Btn_Close->FlatAppearance->BorderSize = 0;
        this->Btn_Close->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Fuchsia;
        this->Btn_Close->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Red;
        this->Btn_Close->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
        this->Btn_Close->Location = System::Drawing::Point(0, 0);
        this->Btn_Close->Name = L"Btn_Close";
        this->Btn_Close->Size = System::Drawing::Size(32, 10);
        this->Btn_Close->TabIndex = 4;
        this->Btn_Close->Text = L"&X";
        this->Btn_Close->TextAlign = System::Drawing::ContentAlignment::TopLeft;
        this->tp->SetToolTip(this->Btn_Close, L"关闭(X)");
        this->Btn_Close->UseVisualStyleBackColor = false;
        this->Btn_Close->Click += gcnew System::EventHandler(this, &dlg::Btn_Close_Click);
        // 
        // Btn_Min
        // 
        this->Btn_Min->BackColor = System::Drawing::Color::Goldenrod;
        this->Btn_Min->Dock = System::Windows::Forms::DockStyle::Left;
        this->Btn_Min->FlatAppearance->BorderSize = 0;
        this->Btn_Min->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Lime;
        this->Btn_Min->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Gold;
        this->Btn_Min->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
        this->Btn_Min->Location = System::Drawing::Point(32, 0);
        this->Btn_Min->Name = L"Btn_Min";
        this->Btn_Min->Size = System::Drawing::Size(32, 10);
        this->Btn_Min->TabIndex = 3;
        this->Btn_Min->Text = L"&N";
        this->Btn_Min->TextAlign = System::Drawing::ContentAlignment::TopLeft;
        this->tp->SetToolTip(this->Btn_Min, L"最小化(N)");
        this->Btn_Min->UseVisualStyleBackColor = false;
        this->Btn_Min->Click += gcnew System::EventHandler(this, &dlg::Btn_Min_Click);
        // 
        // LB_Pos
        // 
        this->LB_Pos->BackColor = System::Drawing::Color::DimGray;
        this->LB_Pos->Dock = System::Windows::Forms::DockStyle::Fill;
        this->LB_Pos->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
        this->LB_Pos->Location = System::Drawing::Point(64, 0);
        this->LB_Pos->Name = L"LB_Pos";
        this->LB_Pos->Size = System::Drawing::Size(300, 10);
        this->LB_Pos->TabIndex = 2;
        this->tp->SetToolTip(this->LB_Pos, L"拖动，移动窗口，双击最大化/还原");
        this->LB_Pos->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &dlg::LB_Pos_MouseDown);
        // 
        // Btn_Listen
        // 
        this->Btn_Listen->BackColor = System::Drawing::Color::DarkSlateBlue;
        this->Btn_Listen->Dock = System::Windows::Forms::DockStyle::Right;
        this->Btn_Listen->FlatAppearance->BorderSize = 0;
        this->Btn_Listen->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
        this->Btn_Listen->Location = System::Drawing::Point(364, 0);
        this->Btn_Listen->Name = L"Btn_Listen";
        this->Btn_Listen->Size = System::Drawing::Size(166, 10);
        this->Btn_Listen->TabIndex = 1;
        this->Btn_Listen->Text = L"&D";
        this->Btn_Listen->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
        this->tp->SetToolTip(this->Btn_Listen, L"监听/暂停(D)");
        this->Btn_Listen->UseVisualStyleBackColor = false;
        this->Btn_Listen->Click += gcnew System::EventHandler(this, &dlg::Btn_Listen_Click);
        // 
        // PL_Control
        // 
        this->PL_Control->Controls->Add(this->LB_Pos);
        this->PL_Control->Controls->Add(this->Btn_Min);
        this->PL_Control->Controls->Add(this->Btn_Close);
        this->PL_Control->Controls->Add(this->Btn_Listen);
        this->PL_Control->Dock = System::Windows::Forms::DockStyle::Top;
        this->PL_Control->Location = System::Drawing::Point(2, 2);
        this->PL_Control->Name = L"PL_Control";
        this->PL_Control->Size = System::Drawing::Size(530, 10);
        this->PL_Control->TabIndex = 10;
        // 
        // Btn_Hide
        // 
        this->Btn_Hide->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(40)), static_cast<System::Int32>(static_cast<System::Byte>(40)),
                                                                     static_cast<System::Int32>(static_cast<System::Byte>(40)));
        this->Btn_Hide->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->Btn_Hide->FlatAppearance->BorderSize = 0;
        this->Btn_Hide->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Black;
        this->Btn_Hide->FlatAppearance->MouseOverBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
                                                                                              static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
        this->Btn_Hide->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
        this->Btn_Hide->Location = System::Drawing::Point(475, 11);
        this->Btn_Hide->Name = L"Btn_Hide";
        this->Btn_Hide->Size = System::Drawing::Size(58, 22);
        this->Btn_Hide->TabIndex = 4;
        this->Btn_Hide->TabStop = false;
        this->Btn_Hide->Text = L"关闭(&C)";
        this->Btn_Hide->UseVisualStyleBackColor = false;
        this->Btn_Hide->Click += gcnew System::EventHandler(this, &dlg::Btn_Hide_Click);
        // 
        // DGV_Data
        // 
        this->DGV_Data->AllowDrop = true;
        this->DGV_Data->AllowUserToAddRows = false;
        this->DGV_Data->AllowUserToResizeRows = false;
        dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
        dataGridViewCellStyle1->BackColor = System::Drawing::Color::DimGray;
        dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"宋体", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
          static_cast<System::Byte>(134)));
        dataGridViewCellStyle1->ForeColor = System::Drawing::Color::White;
        this->DGV_Data->AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
        this->DGV_Data->BackgroundColor = System::Drawing::Color::Black;
        this->DGV_Data->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->DGV_Data->ColumnHeadersBorderStyle = System::Windows::Forms::DataGridViewHeaderBorderStyle::Single;
        dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
        dataGridViewCellStyle2->BackColor = System::Drawing::Color::Black;
        dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"宋体", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
          static_cast<System::Byte>(134)));
        dataGridViewCellStyle2->ForeColor = System::Drawing::Color::Silver;
        dataGridViewCellStyle2->SelectionBackColor = System::Drawing::SystemColors::Highlight;
        dataGridViewCellStyle2->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
        dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
        this->DGV_Data->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
        this->DGV_Data->ColumnHeadersHeight = 21;
        this->DGV_Data->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::DisableResizing;
        this->DGV_Data->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3)
          {
          this->Col_Num, this->Col_Introduction,
            this->Col_Data
          });
        dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
        dataGridViewCellStyle3->BackColor = System::Drawing::Color::Black;
        dataGridViewCellStyle3->Font = (gcnew System::Drawing::Font(L"宋体", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
          static_cast<System::Byte>(134)));
        dataGridViewCellStyle3->ForeColor = System::Drawing::Color::Silver;
        dataGridViewCellStyle3->SelectionBackColor = System::Drawing::SystemColors::Highlight;
        dataGridViewCellStyle3->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
        dataGridViewCellStyle3->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
        this->DGV_Data->DefaultCellStyle = dataGridViewCellStyle3;
        this->DGV_Data->Dock = System::Windows::Forms::DockStyle::Fill;
        this->DGV_Data->EnableHeadersVisualStyles = false;
        this->DGV_Data->GridColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(50)), static_cast<System::Int32>(static_cast<System::Byte>(50)),static_cast<System::Int32>(static_cast<System::Byte>(50)));
        this->DGV_Data->Location = System::Drawing::Point(2, 12);
        this->DGV_Data->Name = L"DGV_Data";
        dataGridViewCellStyle4->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
        dataGridViewCellStyle4->BackColor = System::Drawing::Color::Black;
        dataGridViewCellStyle4->Font = (gcnew System::Drawing::Font(L"宋体", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
          static_cast<System::Byte>(134)));
        dataGridViewCellStyle4->ForeColor = System::Drawing::Color::Silver;
        dataGridViewCellStyle4->SelectionBackColor = System::Drawing::SystemColors::Highlight;
        dataGridViewCellStyle4->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
        dataGridViewCellStyle4->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
        this->DGV_Data->RowHeadersDefaultCellStyle = dataGridViewCellStyle4;
        this->DGV_Data->RowHeadersVisible = false;
        this->DGV_Data->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
        dataGridViewCellStyle5->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
        dataGridViewCellStyle5->BackColor = System::Drawing::Color::Black;
        dataGridViewCellStyle5->ForeColor = System::Drawing::Color::Silver;
        this->DGV_Data->RowsDefaultCellStyle = dataGridViewCellStyle5;
        this->DGV_Data->RowTemplate->DefaultCellStyle->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
        this->DGV_Data->RowTemplate->DefaultCellStyle->BackColor = System::Drawing::Color::Black;
        this->DGV_Data->RowTemplate->DefaultCellStyle->ForeColor = System::Drawing::Color::Silver;
        this->DGV_Data->RowTemplate->Height = 18;
        this->DGV_Data->RowTemplate->ReadOnly = true;
        this->DGV_Data->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
        this->DGV_Data->ShowEditingIcon = false;
        this->DGV_Data->Size = System::Drawing::Size(530, 428);
        this->DGV_Data->TabIndex = 5;
        this->DGV_Data->Tag = L"";
        this->DGV_Data->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &dlg::dlg_DragDrop);
        this->DGV_Data->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &dlg::dlg_DragEnter);
        this->DGV_Data->DoubleClick += gcnew System::EventHandler(this, &dlg::DGV_Data_DoubleClick);
        this->DGV_Data->Enter += gcnew System::EventHandler(this, &dlg::Btn_Hide_Click);
        this->DGV_Data->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &dlg::DGV_Data_KeyDown);
        // 
        // Col_Num
        // 
        this->Col_Num->HeaderText = L"时钟序";
        this->Col_Num->MaxInputLength = 0;
        this->Col_Num->Name = L"Col_Num";
        this->Col_Num->Width = 108;
        // 
        // Col_Introduction
        // 
        this->Col_Introduction->HeaderText = L"简要";
        this->Col_Introduction->MaxInputLength = 0;
        this->Col_Introduction->Name = L"Col_Introduction";
        this->Col_Introduction->Width = 120;
        // 
        // Col_Data
        // 
        this->Col_Data->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
        this->Col_Data->HeaderText = L"数据内容";
        this->Col_Data->Name = L"Col_Data";
        // 
        // TB_Info
        // 
        this->TB_Info->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(20)), static_cast<System::Int32>(static_cast<System::Byte>(20)),static_cast<System::Int32>(static_cast<System::Byte>(20)));
        this->TB_Info->BorderStyle = System::Windows::Forms::BorderStyle::None;
        this->TB_Info->Cursor = System::Windows::Forms::Cursors::Default;
        this->TB_Info->Dock = System::Windows::Forms::DockStyle::Bottom;
        this->TB_Info->ForeColor = System::Drawing::Color::DimGray;
        this->TB_Info->Location = System::Drawing::Point(2, 440);
        this->TB_Info->Multiline = true;
        this->TB_Info->Name = L"TB_Info";
        this->TB_Info->ReadOnly = true;
        this->TB_Info->Size = System::Drawing::Size(530, 12);
        this->TB_Info->TabIndex = 6;
        this->TB_Info->TabStop = false;
        this->TB_Info->DoubleClick += gcnew System::EventHandler(this, &dlg::TB_Info_DoubleClick);
        // 
        // tm
        // 
        this->tm->Tick += gcnew System::EventHandler(this, &dlg::tm_Tick);
        // 
        // PL_Data
        // 
        this->PL_Data->BackColor = System::Drawing::Color::Orchid;
        this->PL_Data->Controls->Add(this->TC_Data);
        this->PL_Data->Location = System::Drawing::Point(1, 120);
        this->PL_Data->Name = L"PL_Data";
        this->PL_Data->Padding = System::Windows::Forms::Padding(2);
        this->PL_Data->Size = System::Drawing::Size(532, 320);
        this->PL_Data->TabIndex = 5;
        this->PL_Data->Visible = false;
        this->PL_Data->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &dlg::PL_Data_MouseMove);
        // 
        // TC_Data
        // 
        this->TC_Data->Appearance = System::Windows::Forms::TabAppearance::FlatButtons;
        this->TC_Data->Dock = System::Windows::Forms::DockStyle::Fill;
        this->TC_Data->Location = System::Drawing::Point(2, 2);
        this->TC_Data->Name = L"TC_Data";
        this->TC_Data->SelectedIndex = 0;
        this->TC_Data->Size = System::Drawing::Size(528, 316);
        this->TC_Data->TabIndex = 1;
        // 
        // dlg
        // 
        this->AllowDrop = true;
        this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
        this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
        this->BackColor = System::Drawing::Color::Black;
        this->CancelButton = this->Btn_Hide;
        this->ClientSize = System::Drawing::Size(534, 454);
        this->Controls->Add(this->PL_Data);
        this->Controls->Add(this->DGV_Data);
        this->Controls->Add(this->TB_Info);
        this->Controls->Add(this->PL_Control);
        this->Controls->Add(this->Btn_Hide);
        this->ForeColor = System::Drawing::Color::Silver;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
        this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
        this->KeyPreview = true;
        this->Name = L"dlg";
        this->Padding = System::Windows::Forms::Padding(2);
        this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
        this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &dlg::dlg_FormClosing);
        this->Load += gcnew System::EventHandler(this, &dlg::dlg_Load);
        this->SizeChanged += gcnew System::EventHandler(this, &dlg::dlg_SizeChanged);
        this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &dlg::dlg_DragDrop);
        this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &dlg::dlg_DragEnter);
        this->PL_Control->ResumeLayout(false);
        (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->DGV_Data))->EndInit();
        this->PL_Data->ResumeLayout(false);
        this->ResumeLayout(false);
        this->PerformLayout();

        }
#pragma endregion

    private: System::Void Btn_Hide_Click(System::Object^  sender, System::EventArgs^  e);
    private: System::Void dlg_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
    private: System::Void Btn_Close_Click(System::Object^  sender, System::EventArgs^  e);
    private: System::Void Btn_Min_Click(System::Object^  sender, System::EventArgs^  e);
    private: System::Void LB_Pos_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    private: System::Void dlg_Load(System::Object^  sender, System::EventArgs^  e);
    private: System::Void Btn_Listen_Click(System::Object^  sender, System::EventArgs^  e);
    private: System::Void TB_Info_DoubleClick(System::Object^  sender, System::EventArgs^  e);
    private: System::Void dlg_SizeChanged(System::Object^  sender, System::EventArgs^  e);
    private: System::Void tm_Tick(System::Object^  sender, System::EventArgs^  e);
    private: System::Void DGV_Data_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
    private: System::Void DGV_Data_DoubleClick(System::Object^  sender, System::EventArgs^  e);
    private: System::Void dlg_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
    private: System::Void dlg_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
    private: System::Void PL_Data_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
    };
  }
