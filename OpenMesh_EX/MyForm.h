#pragma once
#include "Texture.h"
#include "DotNetUtilities.h"
#include "Mesh/GUA_OM.h"
#include "Mesh/DP.h"
#include "Eigen/Sparse"
#include <utility>
#include "Para.h"
#define PI 3.14159f
#define A 15
#define PATH "Texture\\"
using namespace Eigen;

extern std::string mesh_path;
extern std::string para_path;
extern std::shared_ptr<Tri_Mesh> mesh;
extern std::vector<Para> para;
extern bool invalidatePanel2;
extern xform xf;
extern GLCamera camera;
extern float fov;

extern float xCursor;
extern float yCursor;
extern float width;
extern float height;
extern bool ctrl;
extern bool shift;
extern int mode_code[3];
extern GLuint m_tex;
extern std::string m_tex_name;
extern int curFace;
extern std::pair<bool, std::string> hasTexture;

static const Mouse::button physical_to_logical_map[] = {
	Mouse::NONE, Mouse::ROTATE, Mouse::MOVEXY, Mouse::MOVEZ,
	Mouse::MOVEZ, Mouse::MOVEXY, Mouse::MOVEXY, Mouse::MOVEXY,
};
extern Mouse::button Mouse_State;

void savePara(std::string filename);

namespace OpenMesh_EX {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadModelToolStripMenuItem;
	private: System::Windows::Forms::OpenFileDialog^  openModelDialog;
	private: System::Windows::Forms::OpenFileDialog^  openTextureDialog;
	private: System::Windows::Forms::SaveFileDialog^  saveModelDialog;
	private: System::Windows::Forms::OpenFileDialog^  openParaDialog;
	private: System::Windows::Forms::SaveFileDialog^  saveParaDialog;
	private: System::Windows::Forms::ToolStripMenuItem^  saveModelToolStripMenuItem;
	private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl1;
	private: HKOGLPanel::HKOGLPanelControl^  hkoglPanelControl2;

	private: System::Windows::Forms::Label^  cur_face_id;
	private: System::Windows::Forms::ToolStripMenuItem^  textureToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadParaToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveParaToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openTextureStripMenuItem;

	private: System::Windows::Forms::Label^  mode;




	protected:

	private:
		/// <summary>
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// </summary>
		
#pragma endregion
	private: System::Void hkoglPanelControl1_Load(System::Object^  sender, System::EventArgs^  e)
	{
		width = this->ClientSize.Width;
		height = this->ClientSize.Height - 23;
		glShadeModel(GL_SMOOTH);
		point center;
		Mouse_State = Mouse::WHEELUP;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		for (int i = 0; i < 50; ++i)
		{
			camera.mouse(0, 0, Mouse_State,
				xf * center,
				1.0, xf);
		}
	}

	private: System::Void hkoglPanelControl2_Load(System::Object^  sender, System::EventArgs^  e)
	{
		glEnable(GL_RGBA);
		glEnable(GL_ALPHA_TEST);
		glEnable(GL_COLOR_MATERIAL);
	}

	private: Void hkoglPanelControl1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	private: Void hkoglPanelControl2_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	private: System::Void hkoglPanelControl1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
	private: System::Void hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl1_MouseEnter(System::Object^  sender, System::EventArgs^  e);

	private: System::Void hkoglPanelControl2_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
	private: System::Void hkoglPanelControl2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl2_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl2_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void hkoglPanelControl2_MouseEnter(System::Object^  sender, System::EventArgs^  e);

	private: System::Void loadModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void openModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	private: System::Void openTextureToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void openTextureDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	private: System::Void loadParaToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void openParaDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	private: System::Void saveParaToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void saveParaDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	private: System::Void saveModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void saveModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);

	private: System::Void updatePlotMode();

void InitializeComponent(void)
{
	HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting1 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
	HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat1 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
	HKOGLPanel::HKCOGLPanelCameraSetting^  hkcoglPanelCameraSetting2 = (gcnew HKOGLPanel::HKCOGLPanelCameraSetting());
	HKOGLPanel::HKCOGLPanelPixelFormat^  hkcoglPanelPixelFormat2 = (gcnew HKOGLPanel::HKCOGLPanelPixelFormat());
	this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
	this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->loadModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->saveModelToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->loadParaToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->saveParaToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->textureToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->openTextureStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->openModelDialog = (gcnew System::Windows::Forms::OpenFileDialog());
	this->saveModelDialog = (gcnew System::Windows::Forms::SaveFileDialog());
	this->openParaDialog = (gcnew System::Windows::Forms::OpenFileDialog());
	this->saveParaDialog = (gcnew System::Windows::Forms::SaveFileDialog());
	this->hkoglPanelControl1 = (gcnew HKOGLPanel::HKOGLPanelControl());
	this->hkoglPanelControl2 = (gcnew HKOGLPanel::HKOGLPanelControl());
	this->cur_face_id = (gcnew System::Windows::Forms::Label());
	this->openTextureDialog = (gcnew System::Windows::Forms::OpenFileDialog());
	this->mode = (gcnew System::Windows::Forms::Label());
	this->menuStrip1->SuspendLayout();
	this->SuspendLayout();
	// 
	// menuStrip1
	// 
	this->menuStrip1->Dock = System::Windows::Forms::DockStyle::None;
	this->menuStrip1->ImageScalingSize = System::Drawing::Size(32, 32);
	this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
		this->fileToolStripMenuItem,
			this->textureToolStripMenuItem
	});
	this->menuStrip1->Location = System::Drawing::Point(0, 0);
	this->menuStrip1->Name = L"menuStrip1";
	this->menuStrip1->Padding = System::Windows::Forms::Padding(12, 4, 0, 4);
	this->menuStrip1->Size = System::Drawing::Size(183, 44);
	this->menuStrip1->TabIndex = 1;
	this->menuStrip1->Text = L"menuStrip1";
	this->menuStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &MyForm::menuStrip1_ItemClicked);
	// 
	// fileToolStripMenuItem
	// 
	this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
		this->loadModelToolStripMenuItem,
			this->saveModelToolStripMenuItem, this->loadParaToolStripMenuItem, this->saveParaToolStripMenuItem
	});
	this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
	this->fileToolStripMenuItem->Size = System::Drawing::Size(64, 36);
	this->fileToolStripMenuItem->Text = L"File";
	// 
	// loadModelToolStripMenuItem
	// 
	this->loadModelToolStripMenuItem->Name = L"loadModelToolStripMenuItem";
	this->loadModelToolStripMenuItem->Size = System::Drawing::Size(241, 38);
	this->loadModelToolStripMenuItem->Text = L"Load Model";
	this->loadModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadModelToolStripMenuItem_Click);
	// 
	// saveModelToolStripMenuItem
	// 
	this->saveModelToolStripMenuItem->Name = L"saveModelToolStripMenuItem";
	this->saveModelToolStripMenuItem->Size = System::Drawing::Size(241, 38);
	this->saveModelToolStripMenuItem->Text = L"Save Model";
	this->saveModelToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveModelToolStripMenuItem_Click);
	// 
	// loadParaToolStripMenuItem
	// 
	this->loadParaToolStripMenuItem->Name = L"loadParaToolStripMenuItem";
	this->loadParaToolStripMenuItem->Size = System::Drawing::Size(241, 38);
	this->loadParaToolStripMenuItem->Text = L"Load Para";
	this->loadParaToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::loadParaToolStripMenuItem_Click);
	// 
	// saveParaToolStripMenuItem
	// 
	this->saveParaToolStripMenuItem->Name = L"saveParaToolStripMenuItem";
	this->saveParaToolStripMenuItem->Size = System::Drawing::Size(241, 38);
	this->saveParaToolStripMenuItem->Text = L"Save Para";
	this->saveParaToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::saveParaToolStripMenuItem_Click);
	// 
	// textureToolStripMenuItem
	// 
	this->textureToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->openTextureStripMenuItem });
	this->textureToolStripMenuItem->Name = L"textureToolStripMenuItem";
	this->textureToolStripMenuItem->Size = System::Drawing::Size(105, 36);
	this->textureToolStripMenuItem->Text = L"Texture";
	// 
	// openTextureStripMenuItem
	// 
	this->openTextureStripMenuItem->Name = L"openTextureStripMenuItem";
	this->openTextureStripMenuItem->Size = System::Drawing::Size(173, 38);
	this->openTextureStripMenuItem->Text = L"Open";
	this->openTextureStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::openTextureToolStripMenuItem_Click);
	// 
	// openModelDialog
	// 
	this->openModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openModelDialog_FileOk);
	// 
	// saveModelDialog
	// 
	this->saveModelDialog->DefaultExt = L"obj";
	this->saveModelDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::saveModelDialog_FileOk);
	// 
	// openParaDialog
	// 
	this->openParaDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openParaDialog_FileOk);
	// 
	// saveParaDialog
	// 
	this->saveParaDialog->DefaultExt = L"txt";
	this->saveParaDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::saveParaDialog_FileOk);
	// 
	// hkoglPanelControl1
	// 
	hkcoglPanelCameraSetting1->Far = 1000;
	hkcoglPanelCameraSetting1->Fov = 45;
	hkcoglPanelCameraSetting1->Near = -1000;
	hkcoglPanelCameraSetting1->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
	this->hkoglPanelControl1->Camera_Setting = hkcoglPanelCameraSetting1;
	this->hkoglPanelControl1->Location = System::Drawing::Point(8, 33);
	this->hkoglPanelControl1->Margin = System::Windows::Forms::Padding(6, 0, 6, 6);
	this->hkoglPanelControl1->Name = L"hkoglPanelControl1";
	hkcoglPanelPixelFormat1->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	hkcoglPanelPixelFormat1->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	hkcoglPanelPixelFormat1->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	this->hkoglPanelControl1->Pixel_Format = hkcoglPanelPixelFormat1;
	this->hkoglPanelControl1->Size = System::Drawing::Size(600, 600);
	this->hkoglPanelControl1->TabIndex = 2;
	this->hkoglPanelControl1->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_Load);
	this->hkoglPanelControl1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl1_Paint);
	this->hkoglPanelControl1->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::hkoglPanelControl1_KeyDown);
	this->hkoglPanelControl1->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::hkoglPanelControl1_KeyPress);
	this->hkoglPanelControl1->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &MyForm::hkoglPanelControl1_KeyUp);
	this->hkoglPanelControl1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseDown);
	this->hkoglPanelControl1->MouseEnter += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl1_MouseEnter);
	this->hkoglPanelControl1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseMove);
	this->hkoglPanelControl1->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl1_MouseWheel);
	// 
	// hkoglPanelControl2
	// 
	hkcoglPanelCameraSetting2->Far = 1000;
	hkcoglPanelCameraSetting2->Fov = 45;
	hkcoglPanelCameraSetting2->Near = -1000;
	hkcoglPanelCameraSetting2->Type = HKOGLPanel::HKCOGLPanelCameraSetting::CAMERATYPE::ORTHOGRAPHIC;
	this->hkoglPanelControl2->Camera_Setting = hkcoglPanelCameraSetting2;
	this->hkoglPanelControl2->Location = System::Drawing::Point(614, 33);
	this->hkoglPanelControl2->Margin = System::Windows::Forms::Padding(6, 0, 6, 6);
	this->hkoglPanelControl2->Name = L"hkoglPanelControl2";
	hkcoglPanelPixelFormat2->Accumu_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	hkcoglPanelPixelFormat2->Alpha_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	hkcoglPanelPixelFormat2->Stencil_Buffer_Bits = HKOGLPanel::HKCOGLPanelPixelFormat::PIXELBITS::BITS_0;
	this->hkoglPanelControl2->Pixel_Format = hkcoglPanelPixelFormat2;
	this->hkoglPanelControl2->Size = System::Drawing::Size(600, 600);
	this->hkoglPanelControl2->TabIndex = 3;
	this->hkoglPanelControl2->Load += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl2_Load);
	this->hkoglPanelControl2->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::hkoglPanelControl2_Paint);
	this->hkoglPanelControl2->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::hkoglPanelControl2_KeyPress);
	this->hkoglPanelControl2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl2_MouseDown);
	this->hkoglPanelControl2->MouseEnter += gcnew System::EventHandler(this, &MyForm::hkoglPanelControl2_MouseEnter);
	this->hkoglPanelControl2->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl2_MouseMove);
	this->hkoglPanelControl2->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::hkoglPanelControl2_MouseWheel);
	// 
	// cur_face_id
	// 
	this->cur_face_id->AutoSize = true;
	this->cur_face_id->Location = System::Drawing::Point(618, 7);
	this->cur_face_id->Name = L"cur_face_id";
	this->cur_face_id->Size = System::Drawing::Size(161, 25);
	this->cur_face_id->TabIndex = 5;
	this->cur_face_id->Text = L"current face id: ";
	// 
	// openTextureDialog
	// 
	this->openTextureDialog->FileName = L"openTextureDialog1";
	this->openTextureDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &MyForm::openTextureDialog_FileOk);
	// 
	// mode
	// 
	this->mode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
		static_cast<System::Byte>(134)));
	this->mode->Location = System::Drawing::Point(911, 5);
	this->mode->Name = L"mode";
	this->mode->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
	this->mode->Size = System::Drawing::Size(300, 29);
	this->mode->TabIndex = 6;
	this->mode->Text = L"STATUS";
	// 
	// MyForm
	// 
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
	this->ClientSize = System::Drawing::Size(1224, 639);
	this->Controls->Add(this->mode);
	this->Controls->Add(this->cur_face_id);
	this->Controls->Add(this->hkoglPanelControl2);
	this->Controls->Add(this->hkoglPanelControl1);
	this->Controls->Add(this->menuStrip1);
	this->MainMenuStrip = this->menuStrip1;
	this->Margin = System::Windows::Forms::Padding(6);
	this->Name = L"MyForm";
	this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
	this->Text = L"Nah! It\'s A Burger Project!";
	this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
	this->menuStrip1->ResumeLayout(false);
	this->menuStrip1->PerformLayout();
	this->ResumeLayout(false);
	this->PerformLayout();

}

private: System::Void menuStrip1_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
}
private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
}

private: System::Void hkoglPanelControl1_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::ControlKey)
	{
		ctrl = 0;
	}
	else if (e->KeyCode == Keys::ShiftKey)
	{
		shift = 0;
	}
}
private: System::Void hkoglPanelControl1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::ControlKey)
	{
		ctrl = 1;
	}
	else if (e->KeyCode == Keys::ShiftKey)
	{
		shift = 1;
	}
}

};
}
