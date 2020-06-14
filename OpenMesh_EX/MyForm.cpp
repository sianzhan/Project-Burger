#include "MyForm.h"
#include <queue>
using namespace System;
using namespace System::Windows::Forms;

std::string mesh_path;
std::shared_ptr<Tri_Mesh> mesh;
std::vector<Para> para;

xform xf;
GLCamera camera;
float fov = 0.7f;
bool invalidatePanel2 = 1;
float xCursor = 0;
float yCursor = 0;
float width;
float height;
bool ctrl = 0;
bool shift = 0;
int mode_code[3] = { 0 };
GLuint m_tex;
std::string m_tex_name = "troll.png";
std::string para_path = "";
int curFace = 0;
std::pair<bool, std::string> hasTexture;

Mouse::button Mouse_State = Mouse::ROTATE;

[STAThread]
void main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	OpenMesh_EX::MyForm myForm;
	Application::Run(%myForm);
}


System::Void OpenMesh_EX::MyForm::loadModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	openModelDialog->Filter = "Model(*.obj)|*obj";
	openModelDialog->Multiselect = false;
	openModelDialog->ShowDialog();
}

System::Void OpenMesh_EX::MyForm::openModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	std::string filename;
	MarshalString(openModelDialog->SafeFileName, filename);
	mesh_path = filename;
	filename = PATH + filename;

	mesh = std::make_unique<Tri_Mesh>();
	if (ReadFile(filename, mesh.get()))
		std::cout << filename << std::endl;
	curFace = 0;
	para.clear();
	hkoglPanelControl1->Invalidate();
}

System::Void OpenMesh_EX::MyForm::saveModelToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	saveModelDialog->Filter = "Model(*.obj)|*obj";
	saveModelDialog->ShowDialog();
}

System::Void OpenMesh_EX::MyForm::saveModelDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	std::string filename;
	MarshalString(saveModelDialog->FileName, filename);
	if (SaveFile(filename, mesh.get()))
		std::cout << filename << std::endl;
}

System::Void OpenMesh_EX::MyForm::openTextureToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	openTextureDialog->Filter = "Model(*.png;*.jpg)|*png;*jpg";
	openTextureDialog->Multiselect = false;
	openTextureDialog->ShowDialog();
}

System::Void OpenMesh_EX::MyForm::openTextureDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	std::string filename;
	MarshalString(openTextureDialog->SafeFileName, filename);
	if (curFace < para.size()) para[curFace].setTex(filename);
	invalidatePanel2 = 1;
	hkoglPanelControl1->Invalidate();
}

System::Void OpenMesh_EX::MyForm::loadParaToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	openParaDialog->Filter = "Para(*.txt)|*txt";
	openParaDialog->Multiselect = false;
	openParaDialog->ShowDialog();
}

System::Void OpenMesh_EX::MyForm::openParaDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	std::string filename;
	MarshalString(openParaDialog->SafeFileName, filename);
	using namespace std;
	fstream saveFile;
	saveFile.open(PATH + filename, fstream::in);

	if (saveFile)
	{
		string tex_path;
		int N;

		getline(saveFile, mesh_path);

		mesh = std::make_unique<Tri_Mesh>();
		if (ReadFile(PATH + mesh_path, mesh.get())) {}
		else return;

		if (saveFile >> N)
		{
			int M;
			unsigned idx, repeat_mode;
			OpenMesh::FaceHandle fh;
			float translate_x, translate_y, rotate, scale_x, scale_y, alpha, depth;
			para.clear();
			curFace = 0;
			for (int i = 0; i < N; ++i)
			{
				para.push_back(Para());
				saveFile.ignore(10, '\n');

				getline(saveFile, tex_path);
				saveFile >> translate_x >> translate_y >> rotate >> scale_x >> scale_y >> alpha >> depth >> repeat_mode;
				para[i].setRef(mesh);
				para[i].setTex(tex_path);
				para[i].getTex().setTranslateX(translate_x);
				para[i].getTex().setTranslateY(translate_y);
				para[i].getTex().setRotate(rotate);
				para[i].getTex().setScaleX(scale_x);
				para[i].getTex().setScaleY(scale_y);
				para[i].getTex().setAlpha(alpha);
				para[i].getTex().setDepth(depth);
				para[i].getTex().setRepeatMode(repeat_mode);
				saveFile >> M;
				for (int j = 0; j < M; ++j)
				{
					saveFile >> idx;
					fh = mesh->face_handle(idx);
					para[i].addFace(fh);
				}
				para[i].build();
			}
			MarshalString(openParaDialog->FileName, para_path);
			invalidatePanel2 = 1;
			hkoglPanelControl1->Invalidate();
		}
	}
	else
	{
		cout << "Failed to read file :" << filename << endl;
	}
}


System::Void OpenMesh_EX::MyForm::saveParaToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
	saveParaDialog->Filter = "Para(*.txt)|*txt";
	saveParaDialog->ShowDialog();
}

System::Void OpenMesh_EX::MyForm::saveParaDialog_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
{
	std::string filename;
	MarshalString(saveParaDialog->FileName, filename);
	savePara(filename);
}

void savePara(std::string filename)
{
	using namespace std;
	if (filename.empty()) {
		std::cout << "Please specify a file location to save!" << std::endl;
		return;
	}
	fstream saveFile;
	saveFile.open(filename, fstream::out);
	if (saveFile)
	{
		saveFile << mesh_path << endl;
		saveFile << para.size() << endl;
		for (int i = 0; i < para.size(); ++i)
		{
			saveFile << para[i].getTex().getFilename() << endl;
			saveFile << para[i].getTex().getTranslateX() << ' ';
			saveFile << para[i].getTex().getTranslateY() << ' ';
			saveFile << para[i].getTex().getRotate() << ' ';
			saveFile << para[i].getTex().getScaleX() << ' ';
			saveFile << para[i].getTex().getScaleY() << ' ';
			saveFile << para[i].getTex().getAlpha() << ' ';
			saveFile << para[i].getTex().getDepth() << ' ';
			saveFile << para[i].getTex().getRepeatMode() << endl;

			saveFile << para[i].getFaceRecord().size() << endl;
			for (auto fhv_it = para[i].getFaceRecord().cbegin(); fhv_it != para[i].getFaceRecord().cend(); ++fhv_it)
			{
				saveFile << fhv_it->idx() << ' ';
			}
			saveFile << endl;
		}
		para_path = filename;
	}
	else
	{
		std::cout << "Failed to save to: " << filename << std::endl;
	}
}