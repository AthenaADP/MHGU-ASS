#pragma once
#include "CharmDatabase.h"
#include "Armor.h"
#include "SaveData.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MHGUASS {

	public ref class ImportCharms : public System::Windows::Forms::Form
	{
		array< List_t< Charm^ >^ >^ charms;
		array< RadioButton^ >^ char_buttons;
		unsigned selected;

	public:
		unsigned language;
		bool failed;

		ImportCharms(void)
		{
			InitializeComponent();

			{
				array< RadioButton^ >^ temp = { rdoChar1, rdoChar2, rdoChar3 };
				char_buttons = temp;
			}
			failed = false;

			listView1->Columns[ 0 ]->Width = -1;
			
			DialogResult = System::Windows::Forms::DialogResult::Cancel;

			Text = StaticString( ImportCharmsFromSaveData );
			btnOK->Text = StaticString( Import );
			btnSelectBest->Text = StaticString( SelectBest );
			btnSelectNone->Text = StaticString( SelectNone );
			btnCancel->Text = StaticString( Cancel );
			btnSort->Text = StaticString( Sort );
			chkDeleteExisting->Text = StaticString( DeleteExistingCharms );
			grpCharms->Text = StaticString( Charms );
			grpCharacters->Text = StaticString( Characters );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ImportCharms()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnOK;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::ListView^  listView1;
	private: System::Windows::Forms::GroupBox^  grpCharms;
	private: System::Windows::Forms::CheckBox^  chkDeleteExisting;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::Button^  btnSelectNone;
	private: System::Windows::Forms::Button^  btnSelectBest;
	private: System::Windows::Forms::Button^  btnSort;
	private: System::Windows::Forms::GroupBox^  grpCharacters;
	private: System::Windows::Forms::RadioButton^  rdoChar3;
	private: System::Windows::Forms::RadioButton^  rdoChar2;
	private: System::Windows::Forms::RadioButton^  rdoChar1;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btnOK = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->listView1 = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->grpCharms = (gcnew System::Windows::Forms::GroupBox());
			this->chkDeleteExisting = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->btnSort = (gcnew System::Windows::Forms::Button());
			this->btnSelectBest = (gcnew System::Windows::Forms::Button());
			this->btnSelectNone = (gcnew System::Windows::Forms::Button());
			this->grpCharacters = (gcnew System::Windows::Forms::GroupBox());
			this->rdoChar3 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoChar2 = (gcnew System::Windows::Forms::RadioButton());
			this->rdoChar1 = (gcnew System::Windows::Forms::RadioButton());
			this->grpCharms->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->grpCharacters->SuspendLayout();
			this->SuspendLayout();
			// 
			// btnOK
			// 
			this->btnOK->Location = System::Drawing::Point(87, 42);
			this->btnOK->Name = L"btnOK";
			this->btnOK->Size = System::Drawing::Size(75, 23);
			this->btnOK->TabIndex = 0;
			this->btnOK->Text = L"&Import";
			this->btnOK->UseVisualStyleBackColor = true;
			this->btnOK->Click += gcnew System::EventHandler(this, &ImportCharms::btnOK_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point(87, 71);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(75, 23);
			this->btnCancel->TabIndex = 1;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &ImportCharms::btnCancel_Click);
			// 
			// listView1
			// 
			this->listView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listView1->CheckBoxes = true;
			this->listView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(1) {this->columnHeader1});
			this->listView1->Location = System::Drawing::Point(6, 19);
			this->listView1->MultiSelect = false;
			this->listView1->Name = L"listView1";
			this->listView1->Size = System::Drawing::Size(319, 328);
			this->listView1->TabIndex = 2;
			this->listView1->UseCompatibleStateImageBehavior = false;
			this->listView1->View = System::Windows::Forms::View::List;
			// 
			// grpCharms
			// 
			this->grpCharms->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->grpCharms->Controls->Add(this->listView1);
			this->grpCharms->Location = System::Drawing::Point(12, 8);
			this->grpCharms->Name = L"grpCharms";
			this->grpCharms->Size = System::Drawing::Size(331, 353);
			this->grpCharms->TabIndex = 3;
			this->grpCharms->TabStop = false;
			this->grpCharms->Text = L"Charms";
			// 
			// chkDeleteExisting
			// 
			this->chkDeleteExisting->AutoSize = true;
			this->chkDeleteExisting->Checked = true;
			this->chkDeleteExisting->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chkDeleteExisting->Location = System::Drawing::Point(6, 19);
			this->chkDeleteExisting->Name = L"chkDeleteExisting";
			this->chkDeleteExisting->Size = System::Drawing::Size(134, 17);
			this->chkDeleteExisting->TabIndex = 4;
			this->chkDeleteExisting->Text = L"Delete Existing Charms";
			this->chkDeleteExisting->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->btnSort);
			this->groupBox2->Controls->Add(this->btnSelectBest);
			this->groupBox2->Controls->Add(this->btnSelectNone);
			this->groupBox2->Controls->Add(this->chkDeleteExisting);
			this->groupBox2->Controls->Add(this->btnOK);
			this->groupBox2->Controls->Add(this->btnCancel);
			this->groupBox2->Location = System::Drawing::Point(349, 8);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(174, 129);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			// 
			// btnSort
			// 
			this->btnSort->Location = System::Drawing::Point(6, 100);
			this->btnSort->Name = L"btnSort";
			this->btnSort->Size = System::Drawing::Size(75, 23);
			this->btnSort->TabIndex = 7;
			this->btnSort->Text = L"&Sort";
			this->btnSort->UseVisualStyleBackColor = true;
			this->btnSort->Click += gcnew System::EventHandler(this, &ImportCharms::btnSort_Click);
			// 
			// btnSelectBest
			// 
			this->btnSelectBest->Location = System::Drawing::Point(6, 71);
			this->btnSelectBest->Name = L"btnSelectBest";
			this->btnSelectBest->Size = System::Drawing::Size(75, 23);
			this->btnSelectBest->TabIndex = 6;
			this->btnSelectBest->Text = L"Select &Best";
			this->btnSelectBest->UseVisualStyleBackColor = true;
			this->btnSelectBest->Click += gcnew System::EventHandler(this, &ImportCharms::btnSelectBest_Click);
			// 
			// btnSelectNone
			// 
			this->btnSelectNone->Location = System::Drawing::Point(6, 42);
			this->btnSelectNone->Name = L"btnSelectNone";
			this->btnSelectNone->Size = System::Drawing::Size(75, 23);
			this->btnSelectNone->TabIndex = 5;
			this->btnSelectNone->Text = L"Select &None";
			this->btnSelectNone->UseVisualStyleBackColor = true;
			this->btnSelectNone->Click += gcnew System::EventHandler(this, &ImportCharms::btnSelectNone_Click);
			// 
			// grpCharacters
			// 
			this->grpCharacters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->grpCharacters->Controls->Add(this->rdoChar3);
			this->grpCharacters->Controls->Add(this->rdoChar2);
			this->grpCharacters->Controls->Add(this->rdoChar1);
			this->grpCharacters->Location = System::Drawing::Point(349, 143);
			this->grpCharacters->Name = L"grpCharacters";
			this->grpCharacters->Size = System::Drawing::Size(174, 92);
			this->grpCharacters->TabIndex = 6;
			this->grpCharacters->TabStop = false;
			this->grpCharacters->Text = L"Characters";
			// 
			// rdoChar3
			// 
			this->rdoChar3->AutoSize = true;
			this->rdoChar3->Location = System::Drawing::Point(6, 65);
			this->rdoChar3->Name = L"rdoChar3";
			this->rdoChar3->Size = System::Drawing::Size(85, 17);
			this->rdoChar3->TabIndex = 2;
			this->rdoChar3->TabStop = true;
			this->rdoChar3->Text = L"radioButton3";
			this->rdoChar3->UseVisualStyleBackColor = true;
			this->rdoChar3->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar_CheckedChanged);
			// 
			// rdoChar2
			// 
			this->rdoChar2->AutoSize = true;
			this->rdoChar2->Location = System::Drawing::Point(6, 42);
			this->rdoChar2->Name = L"rdoChar2";
			this->rdoChar2->Size = System::Drawing::Size(85, 17);
			this->rdoChar2->TabIndex = 1;
			this->rdoChar2->TabStop = true;
			this->rdoChar2->Text = L"radioButton2";
			this->rdoChar2->UseVisualStyleBackColor = true;
			this->rdoChar2->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar_CheckedChanged);
			// 
			// rdoChar1
			// 
			this->rdoChar1->AutoSize = true;
			this->rdoChar1->Location = System::Drawing::Point(6, 19);
			this->rdoChar1->Name = L"rdoChar1";
			this->rdoChar1->Size = System::Drawing::Size(85, 17);
			this->rdoChar1->TabIndex = 0;
			this->rdoChar1->TabStop = true;
			this->rdoChar1->Text = L"radioButton1";
			this->rdoChar1->UseVisualStyleBackColor = true;
			this->rdoChar1->CheckedChanged += gcnew System::EventHandler(this, &ImportCharms::rdoChar_CheckedChanged);
			// 
			// ImportCharms
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(535, 373);
			this->Controls->Add(this->grpCharacters);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->grpCharms);
			this->Name = L"ImportCharms";
			this->Text = L"Import Charms";
			this->grpCharms->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->grpCharacters->ResumeLayout(false);
			this->grpCharacters->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
		void btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
		{
			Close();
		}

		void AddCharms()
		{
			List_t< Charm^ >^ charm_list = charms[ selected ];
			for( int i = 0; i < charm_list->Count; ++i )
			{
				if( listView1->Items[ i ]->Checked && !charm_list[ i ]->hacked )
				{
					charm_list[ i ]->custom = true;
					CharmDatabase::mycharms.Add( charm_list[ i ] );
				}
			}
		}

		void btnOK_Click(System::Object^  sender, System::EventArgs^  e)
		{
			DialogResult = System::Windows::Forms::DialogResult::OK;

			if( chkDeleteExisting )
				CharmDatabase::mycharms.Clear();

			listView1->Enabled = false;

			AddCharms();

			CharmDatabase::SaveCustom();

			Close();
		}

public:
		void RefreshCharmList()
		{
			listView1->BeginUpdate();
			listView1->Clear();
			for each( Charm^ charm in charms[ selected ] )
			{
				listView1->Items->Add( charm->GetName() );
			}
			listView1->EndUpdate();
			//select best ones by default
			btnSelectBest_Click( nullptr, nullptr );
		}

		void LoadCharms( String^ filename )
		{
			if( filename->EndsWith( L"mycharms.txt" ) )
			{
				charms = gcnew array< List_t< Charm^ >^ >( 1 );
				charms[0] = CharmDatabase::LoadCharms( filename );
				selected = 0;

				grpCharacters->Visible = false;
				rdoChar1->Visible = false;
				rdoChar2->Visible = false;
				rdoChar3->Visible = false;
				rdoChar1->Enabled = false;
				rdoChar2->Enabled = false;
				rdoChar3->Enabled = false;

				RefreshCharmList();
			}
			else
			{
				//load save data
				SaveData save_data( filename );
				if( save_data.failed )
				{
					MessageBox::Show( this, StaticString( SaveDataFileCorrupted ), StaticString( Error ), MessageBoxButtons::OK, MessageBoxIcon::Error );
					return;
				}

				charms = gcnew array< List_t< Charm^ >^ >( 3 );
				for( int i = 0; i < 3; ++i )
				{
					if( save_data.characters[ i ] )
					{
						char_buttons[ i ]->Text = save_data.characters[ i ]->name;
						charms[ i ] = %save_data.characters[ i ]->charms;
					}
					else
					{
						char_buttons[ i ]->Text = StaticString( NoneBrackets );
						charms[ i ] = gcnew List_t< Charm^ >();
						char_buttons[ i ]->Enabled = false;
					}
				}
				selected = save_data.last_used;
				char_buttons[ selected ]->Checked = true;
				
				//RefreshCharmList() will be called by the radio button checked event
			}
		}

		System::Void btnSelectNone_Click(System::Object^  sender, System::EventArgs^  e)
		{
			listView1->BeginUpdate();
			for each( ListViewItem^ item in listView1->Items )
				item->Checked = false;
			listView1->EndUpdate();
		}

		System::Void btnSelectBest_Click(System::Object^  sender, System::EventArgs^  e)
		{
			listView1->BeginUpdate();

			for each( Ability^ a in Ability::static_abilities )
			{
				a->relevant = true;
			}
			List_t< Charm^ >^ charm_list = charms[ selected ];
			for( int i = 0; i < charm_list->Count; ++i )
			{
				if( charm_list[ i ]->hacked )
				{
					listView1->Items[ i ]->ForeColor = Color::Red;
					continue;
				}
				else 
					listView1->Items[ i ]->Checked = true;
				for( int j = 0; j < i; j++ )
				{
					if( listView1->Items[ j ]->Checked )
					{
						if( charm_list[ i ]->StrictlyBetterThan( charm_list[ j ] ) )
							listView1->Items[ j ]->Checked = false;
						else if( charm_list[ j ]->StrictlyBetterThan( charm_list[ i ] ) ||
							charm_list[ i ] == charm_list[ j ] )
							listView1->Items[ i ]->Checked = false;
					}
				}
			}
			listView1->EndUpdate();
		}

		System::Void listView1_ItemChecked(System::Object^  sender, ItemCheckEventArgs^  e)
		{
			if( charms[ selected ][ e->Index ]->hacked )
				e->NewValue = e->CurrentValue;	
		}

		System::Void btnSort_Click(System::Object^  sender, System::EventArgs^  e)
		{
			btnSort->Enabled = false;
			charms[ selected ]->Sort( gcnew Comparison< Charm^ >( CompareCharms1 ) );
			RefreshCharmList();
		}

		System::Void rdoChar_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			RadioButton^ rdo = safe_cast< RadioButton^ >( sender );
			if( rdo->Checked )
			{
				selected = rdo->TabIndex;
				RefreshCharmList();
			}
		}
};
}
