#include "MyForm.h"


//Helper Functions in face selection
double getArea(const double *points[3]);
double getDist(double x1, double x2, double x3, double y1, double y2, double y3);
OpenMesh::FaceHandle selectNearestFace(const double cursor[3]);
OpenMesh::VertexHandle selectPointFromFace(const OpenMesh::FaceHandle fh, const double cursor[3]);
void renderFace(const OpenMesh::FaceHandle fh, const double cursor[3]);

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
	while (curFace >= para.size()) {
		para.push_back(Para());
		para[para.size() - 1].setRef(mesh);
		para[para.size() - 1].setTex(m_tex_name);
		para[para.size() - 1].getTex().setDepth(2 - para.size() / 100.0);
	}
	cur_face_id->Text = "FACE " + (1+curFace).ToString() + "/" + para.size();

	//glEnable(GL_COLOR_MATERIAL);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	point center;
	center[0] = 0.0;
	center[1] = 0.0;
	center[2] = 0.0;

	point pt;
	std::set<OpenMesh::FaceHandle>::iterator it;
	if (mesh != nullptr) {
		point center;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.setupGL(xf * center, 1.0);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glMultMatrixd((double *)xf);
		mesh->Render_Solid();
		if (camera.read_depth(xCursor, (height - yCursor), pt))
		{
			const double cursor[3] = { pt[0], pt[1], pt[2] };
			OpenMesh::FaceHandle selected_face = selectNearestFace(cursor);
			OpenMesh::VertexHandle selected_pt = selectPointFromFace(selected_face, cursor);
			if (mode_code[0] == 1 || mode_code[0] == 2)
			{
				if (mode_code[0] == 1 && para[curFace].addFace(selected_face))
				{
					para[curFace].build();
					invalidatePanel2 = 1;
				}
				else if (mode_code[0] == 2 && para[curFace].deleteFace(selected_face))
				{
					para[curFace].build();
					invalidatePanel2 = 1;
				}
			}
			renderFace(selected_face, cursor); //Render currently selected face
			glPointSize(10);
			glDepthRange(0, 0);
			mesh->Render_Pointed(selected_pt); //Render selected point on face
			glColor4f(1.0, 0.5, 1.0, 1.0); //Render cursor
			glBegin(GL_POINTS);
			glVertex3f(pt[0], pt[1], pt[2]);
			glEnd();
			glDepthRange(0, 1.0);
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (int i = 0; i < para.size(); i++)
		{
			if (mode_code[2] == 0) para[i].pan();
			if (i == curFace) para[i].drawSolidWireframe(1, 1);
			else para[i].drawSolidWireframe(1, 0);
		}
		glDisable(GL_BLEND);
		glPopMatrix();
		if (invalidatePanel2 && mode_code[2] == 0) invalidatePanel2 = 0, hkoglPanelControl2->Invalidate();
	}
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e)
{
	if (e->KeyChar == 'c' && curFace > 0)
	{
		curFace--;
		mode_code[0] = 0;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}
	if (e->KeyChar == 'v')
	{
		curFace++;
		mode_code[0] = 0;
		invalidatePanel2 = 1;
		hkoglPanelControl1->Invalidate();
	}

	if (e->KeyChar == 'z') {
		if(mode_code[0] != 0) mode_code[0] = 0;
		else mode_code[0] = 1;
	}

	if (e->KeyChar == 'x') {
		if (mode_code[0] == 2) mode_code[0] = 0;
		else mode_code[0] = 2;
	}

	if (e->KeyChar == 't') {
		if (mode_code[2] == 1) mode_code[2] = 0;
		else mode_code[2] = 1;
		hkoglPanelControl1->Invalidate();
	}
	if (e->KeyChar == '.') {
		savePara(para_path);
	}
	if (mode_code[0] == 1) mode->Text = "SELECT MODE";
	else if (mode_code[0] == 2) mode->Text = "DELETE MODE";
	else mode->Text = "DRAWING";

}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if (e->Button == System::Windows::Forms::MouseButtons::Left ||
		e->Button == System::Windows::Forms::MouseButtons::Middle || ctrl)
	{
		point center;
		Mouse_State = Mouse::NONE;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
	}
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	xCursor = e->X, yCursor = e->Y;
	if (e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		point center;
		Mouse_State = Mouse::ROTATE;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
		//hkoglPanelControl1->Invalidate();
	}
	else if (e->Button == System::Windows::Forms::MouseButtons::Middle || ctrl)
	{
		point center;
		Mouse_State = Mouse::MOVEXY;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
		//hkoglPanelControl1->Invalidate();
	}
	else if (shift)
	{
		point center;
		Mouse_State = Mouse::LIGHT;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
		//hkoglPanelControl1->Invalidate();
	}

	width = this->ClientSize.Width;
	height = this->ClientSize.Height - 46;

	hkoglPanelControl1->Invalidate();
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	if (e->Delta < 0)
	{
		point center;
		Mouse_State = Mouse::WHEELDOWN;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
		hkoglPanelControl1->Invalidate();
	}
	else
	{
		point center;
		Mouse_State = Mouse::WHEELUP;
		center[0] = 0.0;
		center[1] = 0.0;
		center[2] = 0.0;
		camera.mouse(e->X, e->Y, Mouse_State,
			xf * center,
			1.0, xf);
		hkoglPanelControl1->Invalidate();
	}
}

System::Void OpenMesh_EX::MyForm::hkoglPanelControl1_MouseEnter(System::Object^  sender, System::EventArgs^  e)
{
	mode_code[0] = 0;
	mode->Text = "DRAWING";
	hkoglPanelControl1->Focus();
}


double getArea(const double *points[3])
{
	double tmp[2][3];
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tmp[i][j] = points[i + 1][j] - points[0][j];
		}
	}
	;
	return sqrt(
		pow(tmp[0][1] * tmp[1][2] - tmp[0][2] * tmp[1][1], 2) +
		pow(tmp[0][2] * tmp[1][0] - tmp[0][0] * tmp[1][2], 2) +
		pow(tmp[0][0] * tmp[1][1] - tmp[0][1] * tmp[1][0], 2)
	);
}

double getDist(double x1, double x2, double x3, double y1, double y2, double y3)
{
	return sqrt(pow(x1 - y1, 2) + pow(x2 - y2, 2) + pow(x3 - y3, 2));
}

OpenMesh::FaceHandle selectNearestFace(const double cursor[3])
{
	OpenMesh::PolyConnectivity::FIter f_it;
	OpenMesh::PolyConnectivity::FVIter	fv_it;
	OpenMesh::FaceHandle fh, ret;
	double minArea = 10000;
	const double *tmp[3] = { 0 };
	const double *in[3] = { 0 };
	for (f_it = mesh->faces_begin(); f_it != mesh->faces_end(); ++f_it)
	{
		int c = 0;
		fh = f_it.handle();
		for (fv_it = mesh->fv_iter(f_it); fv_it; ++fv_it)
		{
			double* pt = mesh->point(fv_it.handle()).data();
			tmp[c++] = pt;
		}
		if (c == 3)
		{
			double area = 0;
			in[0] = cursor;
			in[1] = tmp[0];
			in[2] = tmp[1];
			area += getArea(in);
			in[2] = tmp[2];
			area += getArea(in);
			in[1] = tmp[1];
			area += getArea(in);
			in[0] = tmp[0];
			area -= getArea(in);
			area = fabs(area);
			if (area < minArea)
			{
				ret = fh;
				minArea = area;
			}
		}
	}

	return ret;
}

OpenMesh::VertexHandle selectPointFromFace(const OpenMesh::FaceHandle fh, const double cursor[3])
{
	OpenMesh::PolyConnectivity::FVIter	fv_it;
	OpenMesh::VertexHandle ret;
	double dist = 0, minDist = 99999;
	const double *pt = 0;
	for (fv_it = mesh->fv_iter(fh); fv_it; ++fv_it)
	{
		pt = mesh->point(fv_it.handle()).data();
		dist = getDist(pt[0], pt[1], pt[2], cursor[0], cursor[1], cursor[2]);
		if (dist < minDist)
		{
			minDist = dist;
			ret = fv_it.handle();
		}
	}
	return ret;
}

void renderFace(const OpenMesh::FaceHandle fh, const double cursor[3])
{
	glDisable(GL_LIGHTING);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(-2.0, -2.0);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	OpenMesh::PolyConnectivity::FVIter	fv_it;
	const double *face_pt[3], *pt[3];
	int i = 0;
	for (fv_it = mesh->fv_iter(fh); fv_it && i < 3; ++fv_it) face_pt[i++] = mesh->point(fv_it.handle()).data();
	pt[0] = cursor;
	pt[1] = face_pt[0];
	pt[2] = face_pt[1];
	glColor4f(1.0, 0.3, 1, 0.8);
	for (int i = 0; i < 3; ++i) glVertex3d(pt[i][0], pt[i][1], pt[i][2]);
	pt[2] = face_pt[2];
	glColor4f(0.3, 0.6, 1.0, 0.8);
	for (int i = 2; i >= 0; --i) glVertex3d(pt[i][0], pt[i][1], pt[i][2]);
	pt[1] = face_pt[1];
	glColor4f(1.0, 1.0, 0.0, 0.8);
	for (int i = 0; i < 3; ++i) glVertex3d(pt[i][0], pt[i][1], pt[i][2]);
	glEnd();

	glDepthRange(-1, 0);
	glLineWidth(1.0);
	glColor4f(0.1, 0.1, 0.1, 1.0);
	glBegin(GL_LINES);
	for (int i = 0, j; i < 3; ++i)
	{
		glVertex3d(face_pt[i][0], face_pt[i][1], face_pt[i][2]);
		j = (i + 1) % 3;
		glVertex3d(face_pt[j][0], face_pt[j][1], face_pt[j][2]);
	}
	glEnd();

	glPopMatrix();
	glPopAttrib();
	glDepthRange(0, 1);
	glDisable(GL_POLYGON_OFFSET_FILL);
}