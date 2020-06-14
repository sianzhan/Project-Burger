#include "MyForm.h"

System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	if (curFace < para.size())
	{
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		para[curFace].drawPlot(2);
		glPopMatrix();
	}
}


System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if (e->Button == System::Windows::Forms::MouseButtons::Left ||
		e->Button == System::Windows::Forms::MouseButtons::Middle)
	{
	}
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if (curFace >= para.size()) return;

	static int lastmousex = 400;
	static int lastmousey = 400;

	if (e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		double rot = para[curFace].getTex().getRotate();
		double diffX = (e->X - lastmousex);
		double diffY = (e->Y - lastmousey);
		double _s = cos(rot) * diffX - sin(rot) * diffY;
		double _t = sin(rot) * diffX + cos(rot) * diffY;
		para[curFace].getTex().setTranslateX(
			para[curFace].getTex().getTranslateX() + _s / 280.0 * para[curFace].getTex().getScaleX());
		para[curFace].getTex().setTranslateY(
			para[curFace].getTex().getTranslateY() - _t / 280.0 * para[curFace].getTex().getScaleY());

		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
		
	}
	lastmousex = e->X;
	lastmousey = e->Y;

	if (e->Button == System::Windows::Forms::MouseButtons::Middle)
	{
	}

}
System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if (curFace >= para.size()) return;

	if (mode_code[1] == 1) para[curFace].getTex().setRotate(para[curFace].getTex().getRotate() + e->Delta / 2000.0);
	else if (mode_code[1] == 2) para[curFace].getTex().setAlpha(para[curFace].getTex().getAlpha() + e->Delta / 5000.0);
	else
	{
		float ratio = (1 + e->Delta / 8000.0);
		if (mode_code[1] == 10) {
			float ratio = (1 + e->Delta / 8000.0);
			para[curFace].getTex().setScaleX(para[curFace].getTex().getScaleX() * ratio);
			para[curFace].getTex().setScaleY(para[curFace].getTex().getScaleY() * ratio);
		}
		else if (mode_code[1] == 11) {
			para[curFace].getTex().setScaleX(para[curFace].getTex().getScaleX() * ratio);
		}
		else if (mode_code[1] == 12) {
			para[curFace].getTex().setScaleY(para[curFace].getTex().getScaleY() * ratio);
		}
	}
	invalidatePanel2 = 1;
	hkoglPanelControl1->Invalidate();
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_MouseEnter(System::Object^  sender, System::EventArgs^  e)
{
	hkoglPanelControl2->Focus();
	updatePlotMode();
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl2_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
{
	if (curFace >= para.size()) return;

	if (e->KeyChar == 'r')
	{
		if (mode_code[1] == 1) mode_code[1] = 0;
		else mode_code[1] = 1;
	}
	else if (e->KeyChar == 'a')
	{
		if (mode_code[1] == 2) mode_code[1] = 0;
		else mode_code[1] = 2;
	}
	else if (e->KeyChar == 's')
	{
		if (mode_code[1] == 10) mode_code[1] = 11;
		else if (mode_code[1] == 11) mode_code[1] = 12;
		else if (mode_code[1] == 12) mode_code[1] = 0;
		else mode_code[1] = 10;
	}
	else if (e->KeyChar == 'c' && curFace > 0)
	{
		curFace--;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == 'v')
	{
		curFace++;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == 'f')
	{
		switch (para[curFace].getTex().getRepeatMode())
		{
		case GL_CLAMP_TO_EDGE: para[curFace].getTex().setRepeatMode(GL_CLAMP_TO_BORDER); break;
		case GL_CLAMP_TO_BORDER: para[curFace].getTex().setRepeatMode(GL_REPEAT); break;
		case GL_REPEAT: para[curFace].getTex().setRepeatMode(GL_MIRRORED_REPEAT); break;
		default: para[curFace].getTex().setRepeatMode(GL_CLAMP_TO_EDGE); break;
		}

		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == '-')
	{
		para.erase(para.begin() + curFace);
		if (curFace > 0) --curFace;
		if (para.size() == 0)
		{
			para.push_back(Para());
			para[0].setRef(mesh);
			para[0].setTex(m_tex_name);
			for (int i = 0; i < para.size(); ++i)
			{
				para[i].getTex().setDepth(2 - (i + 1) / 100.0);
			}
		}
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == '+')
	{
		++curFace;
		para.insert(para.begin() + curFace, Para());
		para[curFace].setRef(mesh);
		para[curFace].setTex(m_tex_name);
		for (int i = 0; i < para.size(); ++i)
		{
			para[i].getTex().setDepth(2 - (i + 1) / 100.0);
		}
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == 'n' && curFace + 1 < para.size())
	{
		Para tmp = std::move(para[curFace]);
		para[curFace] = std::move(para[curFace + 1]);
		para[curFace + 1] = std::move(tmp);
		for (int i = 0; i < para.size(); ++i)
		{
			para[i].getTex().setDepth(2 - (i + 1) / 100.0);
		}
		curFace++;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == 'b' && curFace > 0)
	{
		Para tmp = std::move(para[curFace - 1]);
		para[curFace - 1] = std::move(para[curFace]);
		para[curFace] = std::move(tmp);
		for (int i = 0; i < para.size(); ++i)
		{
			para[i].getTex().setDepth(2 - (i + 1) / 100.0);
		}
		curFace--;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	else if (e->KeyChar == '.') {
		savePara(para_path);
	}
	updatePlotMode();
}

System::Void OpenMesh_EX::MyForm::updatePlotMode()
{
	if (curFace >= para.size()) return;

	if (mode_code[1] == 1) mode->Text = "ROTATE MODE";
	else if (mode_code[1] == 2) mode->Text = "ALPHA MODE";
	else if (mode_code[1] == 10) mode->Text = "FULL SCALE MODE";
	else if (mode_code[1] == 11) mode->Text = "HORIZONTAL SCALE MODE";
	else if (mode_code[1] == 12) mode->Text = "VERTICAL SCALE MODE";
	else {
		mode->Text = "DRAWING ";

		switch (para[curFace].getTex().getRepeatMode())
		{
		case GL_CLAMP_TO_EDGE: mode->Text += "(CLAMP_TO_EDGE)"; break;
		case GL_CLAMP_TO_BORDER: mode->Text += "(CLAMP_TO_BORDER)"; break;
		case GL_REPEAT: mode->Text += "(REPEAT)"; break;
		case GL_MIRRORED_REPEAT: mode->Text += "(MIRRORED_REPEAT)"; break;
		default:;
		}
	}
}
