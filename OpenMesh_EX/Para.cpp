#include "Para.h"
#include <ctime>
int Para::height = 500;
int Para::width = 500;

std::map<int, std::vector<int> > Para::Tex::tex_garbage;

void Para::setRef(std::shared_ptr<Tri_Mesh> ref_) { this->ref = ref_; }
void Para::build()
{
	if (ref == nullptr) return;
	if (para == nullptr) para = std::make_unique<Tri_Mesh>();
	map_vertices_to_plot.clear();
	plot.clear();
	boundary_vertices.clear();
	inner_vertices.clear();
	hasUpdate = 1;
}

void Para::pan()
{
	if (ref == nullptr) return;
	if (hasUpdate)
	{
		buildVerticesMap();
		panVerticesToPlot();
		megaCalculation();
		hasUpdate = 0;
	}
}

void Para::drawSolidWireframe(int view, bool mark) 
{
	if (para == nullptr) return;
	int texId = tex.getId(view);
	if (texId == -1 && !tex.getFilename().empty())
	{
		std::string filename = PATH + tex.getFilename();
		texId = Texture::GenTexture(filename.c_str());
		tex.setId(view, texId);
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	//glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(tex.getDepth(), tex.getDepth());
	/*if (mark)
	{
		glPolygonOffset(8.0, 8.0);
		glBegin(GL_TRIANGLES);
		glColor4f(1.0, 1.0, 1.0, 0.5);
		for (f_it = para->faces_begin(); f_it != para->faces_end(); ++f_it)
		{
			for (fv_it = para->fv_iter(f_it); fv_it; ++fv_it)
			{
				//glNormal3dv(normal(fv_it.handle()));
				double* pt = para->point(fv_it.handle()).data();
				glVertex3d(pt[0], pt[1], pt[2]);
			}
		}
		glEnd();
	}*/
	if (texId != -1 && !plot.empty()) {
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.getRepeatMode());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.getRepeatMode());
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4f(1.0, 1.0, 1.0, tex.getAlpha());
	}
	else glColor4f(1.0, 0.5, 0.5, 1.0);
	glBegin(GL_TRIANGLES);
	OpenMesh::PolyConnectivity::FIter f_it;
	OpenMesh::PolyConnectivity::FVIter	fv_it;
	std::unordered_map<int, int>::iterator plot_it;
	for (f_it = para->faces_begin(); f_it != para->faces_end(); ++f_it)
	{
		for (fv_it = para->fv_iter(f_it); fv_it; ++fv_it)
		{
			if (texId != -1 && (plot_it = map_vertices_to_plot.find(fv_it->idx())) != map_vertices_to_plot.end()) {
				double _s = plot.at(plot_it->second)[0];
				double _t = plot.at(plot_it->second)[1];
				double _s2 = (cos(tex.getRotate()) * _s - sin(tex.getRotate()) * _t) * tex.getScaleX() + tex.getTranslateX();
				double _t2 = (sin(tex.getRotate()) * _s + cos(tex.getRotate()) * _t) * tex.getScaleY() - tex.getTranslateY();
				glTexCoord2d((_s2+1)/2, (_t2+1)/2);
			}
			OpenMesh::VertexHandle vh = ref->vertex_handle(map_vertices_para_to_ref[fv_it->idx()]);
			glNormal3dv(ref->normal(vh).data());
			glVertex3dv(ref->point(vh).data());
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_LIGHTING);
	//glPushAttrib(GL_LIGHTING_BIT);
	//glDisable(GL_LIGHTING);
	glLineWidth(10);
	glColor4f(1.0, 1.0, 0.5, 1.0);
	/*glBegin(GL_LINES);
	for (OMT::EIter e_it = para->edges_begin(); e_it != para->edges_end(); ++e_it)
	{
		OMT::HEHandle _hedge = para->halfedge_handle(e_it.handle(), 1);

		OMT::Point curVertex = para->point(para->from_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());

		curVertex = para->point(para->to_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());
	}
	glEnd();*/
	if (mark)
	{
		OMT::Point pt;
		glBegin(GL_LINES);
		for (OpenMesh::PolyConnectivity::HalfedgeIter h_it = para->halfedges_begin();
			h_it != para->halfedges_end(); ++h_it)
		{
			if (para->is_boundary(h_it))
			{
				pt = para->point(para->to_vertex_handle(h_it));
				glVertex3dv(pt.data());
				pt = para->point(para->from_vertex_handle(h_it));
				glVertex3dv(pt.data());
			}
		}
		glEnd();

	}
	//glPopAttrib();
}

void Para::drawPlot(int view)
{
	if (para == nullptr || plot.empty()) return;
	int texId = tex.getId(view);
	if (texId == -1 && !tex.getFilename().empty())
	{
		std::string filename = PATH + tex.getFilename();
		texId = Texture::GenTexture(filename.c_str());
		tex.setId(view, texId);
		tex.clearGarbage(view);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(0.45, 0.45, 0.45);
	glDisable(GL_LIGHTING);
	glPushAttrib(GL_LIGHTING_BIT);

	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	const static float mov_x[4] = { -1.0, 1.0, 1.0, -1.0 }; //Counter clockwise
	const static float mov_y[4] = { 1.0, 1.0, -1.0, -1.0 };

	//Layer 1 - Draw Texture Background
	if (texId != -1) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.getRepeatMode());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.getRepeatMode());
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glColor4f(1.0, 1.0, 1.0, 0.3 * tex.getAlpha()+0.1);
		for (int i = 0; i < 4; ++i)
		{
			double _s = mov_x[i];
			double _t = mov_y[i];
			double _s2 = (cos(tex.getRotate()) * _s - sin(tex.getRotate()) * _t) * tex.getScaleX() + tex.getTranslateX();
			double _t2 = (sin(tex.getRotate()) * _s + cos(tex.getRotate()) * _t) * tex.getScaleY() - tex.getTranslateY();
			glTexCoord2d((_s2 + 1) / 2 , (_t2 + 1) / 2 );
			glVertex2d(-mov_x[i], mov_y[i]); //Turn it clockwise and same side with the polygon
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	//Layer 2 - Draw Selecting Shade
	glBegin(GL_TRIANGLES);
	glColor4f(0.5, 0.96, 0.49, 0.5);
	OpenMesh::PolyConnectivity::FIter f_it;
	OpenMesh::PolyConnectivity::FVIter fv_it;
	int idx;
	for (f_it = para->faces_begin(); f_it != para->faces_end(); ++f_it)
	{
		for (fv_it = para->fv_iter(f_it); fv_it; ++fv_it)
		{
			idx = map_vertices_to_plot.at(fv_it->idx());
			glVertex2d(-plot.at(idx)[0], plot.at(idx)[1]);
		}
	}
	glEnd();
	
	//Layer 3 - Draw Texture Selection in Bright
	if (texId != -1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLES);
		glColor4f(1.0, 1.0, 1.0, 0.8 * tex.getAlpha());

		for (f_it = para->faces_begin(); f_it != para->faces_end(); ++f_it)
		{
			for (fv_it = para->fv_iter(f_it); fv_it; ++fv_it)
			{
				//glNormal3dv(normal(fv_it.handle()));
				idx = map_vertices_to_plot.at(fv_it->idx());
				double _s = plot.at(idx)[0];
				double _t = plot.at(idx)[1];
				double _s2 = (cos(tex.getRotate()) * _s - sin(tex.getRotate()) * _t)* tex.getScaleX() + tex.getTranslateX();
				double _t2 = (sin(tex.getRotate()) * _s + cos(tex.getRotate()) * _t)* tex.getScaleY() - tex.getTranslateY();
				glTexCoord2d((_s2 + 1) / 2 , (_t2 + 1)/ 2 );
				
				glVertex2d(-plot.at(idx)[0], plot.at(idx)[1]);
			}
		}

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	glDisable(GL_BLEND);
	glLineWidth(1.0);
	glColor4f(0.1, 0.1, 0.1, 0.5);
	OpenMesh::PolyConnectivity::EIter e_it;
	for (e_it = para->edges_begin(); e_it != para->edges_end(); e_it++)
	{
		OpenMesh::HalfedgeHandle h0 = para->halfedge_handle(*e_it, 1);
		int v0 = map_vertices_to_plot.at(para->to_vertex_handle(h0).idx());
		int v1 = map_vertices_to_plot.at(para->from_vertex_handle(h0).idx());

		glBegin(GL_LINES);
		glVertex2d(-plot.at(v0)[0], plot.at(v0)[1]);
		glVertex2d(-plot.at(v1)[0], plot.at(v1)[1]);
		glEnd();
	}
	glPopAttrib();
}

//Helper Function
//Add Face using std::set<OpenMesh::FaceHandle>::iterator over ref_faceset
bool Para::addFace(OpenMesh::FaceHandle to_add)
{
	if (ref == nullptr) return false;

	std::vector<OpenMesh::VertexHandle> vvh; //For Creating Triangle Faces
	if (face_record.find(to_add) == face_record.end())
	{
		face_record.insert(to_add);

		if (para == nullptr) para = std::make_unique<Tri_Mesh>();

		std::map<std::vector<double>, int>::iterator it;
		int idx_ref, idx_para;
		for (OpenMesh::PolyConnectivity::FVIter fv_it = ref->fv_iter(to_add); fv_it; ++fv_it)
		{
			idx_ref = fv_it->idx();
			double* pt = ref->point(fv_it).data();
			std::vector<double> cmp = { pt[0], pt[1], pt[2] };
			if ((it = vertice_record.find(cmp)) == vertice_record.end())
			{
				idx_para = vertice_record[cmp] = para->add_vertex(OpenMesh::Vec3d(pt[0], pt[1], pt[2])).idx();
				map_vertices_para_to_ref[idx_para] = idx_ref;
			}
			else idx_para = it->second;

			vvh.push_back(para->vertex_handle(idx_para));
		}
		para->add_face(vvh);
		return 1;
	}
	return 0;
}

bool Para::deleteFace(OpenMesh::FaceHandle to_del)
{
	if (ref == nullptr) return false;
	std::vector<OpenMesh::VertexHandle> vvh; //For Creating Triangle Faces
	if (face_record.find(to_del) != face_record.end())
	{
		face_record.erase(to_del);

		para->clear();
		vertice_record.clear();
		map_vertices_para_to_ref.clear();

		std::map<std::vector<double>, int>::iterator it;
		int idx_ref, idx_para;
		for (std::set<OpenMesh::FaceHandle>::iterator fit = face_record.begin(); fit != face_record.end(); ++fit)
		{
			vvh.clear();

			for (OpenMesh::PolyConnectivity::FVIter fv_it = ref->fv_iter(*fit); fv_it; ++fv_it)
			{
				idx_ref = fv_it->idx();
				double* pt = ref->point(fv_it).data();
				std::vector<double> cmp = { pt[0], pt[1], pt[2] };
				if ((it = vertice_record.find(cmp)) == vertice_record.end())
				{
					idx_para = vertice_record[cmp] = para->add_vertex(OpenMesh::Vec3d(pt[0], pt[1], pt[2])).idx();
					map_vertices_para_to_ref[idx_para] = idx_ref;
				}
				else idx_para = it->second;

				vvh.push_back(para->vertex_handle(idx_para));
			}
			para->add_face(vvh);
		}
		return 1;
	}
	return 0;
}

//Helper Function
//Segregate Boundary Vertices & Inner Vertices & and Assign plot ID for them
void Para::buildVerticesMap()
{
	if (para == nullptr) return;
	map_vertices_to_plot.clear();
	boundary_vertices.clear();
	inner_vertices.clear();
	OpenMesh::PolyConnectivity::VertexIter	v_it;
	std::set<int> dupCheck;
	int count = 0;
	OpenMesh::PolyConnectivity::HalfedgeHandle heh_init = para->halfedges_end(), heh = para->halfedges_end();
	double color = 0.0;
	//heh_init = heh = halfedge_handle(first);
	for (OpenMesh::PolyConnectivity::HalfedgeIter h_it = para->halfedges_begin();
		h_it != para->halfedges_end(); ++h_it)
	{
		if (para->is_boundary(h_it)) 
		{
			heh_init = heh = h_it;
			break;
		}
	}
	if (heh != para->halfedges_end())
	{
		do {
			OpenMesh::PolyConnectivity::VertexHandle vh = para->to_vertex_handle(heh);
			int idx = vh.idx();
			if (dupCheck.find(idx) == dupCheck.end())
			{
				map_vertices_to_plot[idx] = count++;
				dupCheck.insert(idx);
				boundary_vertices.push_back(vh);
			}
			heh = para->next_halfedge_handle(heh);
		} while (heh != heh_init);
	}
	for (v_it = para->vertices_begin(); v_it != para->vertices_end(); ++v_it)
	{
		int idx = v_it->idx();
		if (dupCheck.find(idx) == dupCheck.end()) {
			//Plot ID assignment
			map_vertices_to_plot[v_it->idx()] = count++;
			inner_vertices.push_back(v_it.handle());
		}
	}
}

void Para::panVerticesToPlot()
{
	if (map_vertices_to_plot.empty()) return;
	plot = std::vector< std::vector<double>>(map_vertices_to_plot.size(), std::vector<double>(2));
	std::vector<double> boundary_length(boundary_vertices.size());
	double theta = 0, perimeter = 0;
	const double *oPoint[3]; //Outgoing vertex
	if (boundary_vertices.size() > 0)
	{
		oPoint[1] = para->point(boundary_vertices[boundary_vertices.size() - 1]).data();
		for (int i = 0; i < boundary_vertices.size(); ++i)
		{
			oPoint[0] = oPoint[1];
			oPoint[1] = para->point(boundary_vertices[i]).data();
			double v0v1 = 0;
			for (int j = 0; j < 3; ++j) v0v1 += pow((oPoint[0][j] - oPoint[1][j]), 2);
			boundary_length[i] = sqrt(v0v1);
			perimeter += boundary_length[i];
		}
		int larger = width >= height ? width : height;
		int i = 0;
		for (; i < boundary_vertices.size(); ++i)
		{
			//Pan vertices into circle first
			plot[i][0] = cos(theta * PI / 180);
			plot[i][1] = sin(theta * PI / 180);

			//Then Pan them into square by scaling it
			if (fabs(plot[i][0]) < fabs(plot[i][1]))
			{
				plot[i][0] *= (larger / width) / fabs(plot[i][1]);
				plot[i][1] *= (larger / height) / fabs(plot[i][1]);
			}
			else
			{
				plot[i][0] *= (larger / width) / fabs(plot[i][0]);
				plot[i][1] *= (larger / height) / fabs(plot[i][0]);
			}

			theta += 360 * boundary_length[i] / perimeter;
		}
		for (; i < map_vertices_to_plot.size(); ++i)
		{
			plot[i][0] = 0;
			plot[i][1] = 0;
		}
	}
}

void Para::megaCalculation()
{
	//////////////////////////////////////////////// MEGA MATHEMATICS BELOW//////// ATTENTION!!!!
	const double *oPoint[3]; //Outgoing vertex
	std::vector<double> WEIGHT(inner_vertices.size());
	std::vector<double> alpha(inner_vertices.size());
	std::vector< std::vector<std::pair<double, OpenMesh::VertexHandle>> > weight(inner_vertices.size()); //first for weight, second for para's vertices idx
	for (int i = 0; i < inner_vertices.size(); ++i)
	{
		OpenMesh::PolyConnectivity::VertexOHalfedgeIter voh_it = para->voh_iter(inner_vertices[i]);
		OpenMesh::PolyConnectivity::VertexOHalfedgeIter voh_it2 = voh_it;
		while (voh_it2) voh_it = voh_it2++; //get the last vertices

		std::vector<OpenMesh::VertexHandle> vertex; //Keep array to vertex idx.
		std::vector<double> alpha; //starts at i-1 end at n

		oPoint[2] = para->point(inner_vertices[i]).data();
		oPoint[1] = para->point(para->to_vertex_handle(voh_it)).data(); //This is the n-1 th point
		for (voh_it = para->voh_iter(inner_vertices[i]); voh_it; ++voh_it) {
			// Iterate over all incoming halfedges...
			oPoint[0] = oPoint[1];
			oPoint[1] = para->point(para->to_vertex_handle(voh_it)).data();
			double v1v2v3 = 0;
			for (int j = 0; j < 3; ++j) v1v2v3 += (oPoint[0][j] - oPoint[2][j]) * (oPoint[1][j] - oPoint[2][j]);
			double norm_v1(sqrt(pow(oPoint[0][0], 2) + pow(oPoint[0][1], 2) + pow(oPoint[0][2], 2)));
			double norm_v2(sqrt(pow(oPoint[1][0], 2) + pow(oPoint[1][1], 2) + pow(oPoint[1][2], 2)));
			vertex.push_back(para->to_vertex_handle(voh_it));
			alpha.push_back(acos(v1v2v3 / (norm_v1 * norm_v2)));
		}
		alpha.push_back(alpha[0]);
		for (int q = 0; q < vertex.size(); ++q)
		{
			double *pt = para->point(vertex[q]).data();
			double norm(sqrt(pow(oPoint[2][0] - pt[0], 2) + pow(oPoint[2][1] - pt[1], 2) + pow(oPoint[2][2] - pt[2], 2)));
			weight[i].push_back(std::make_pair(((tan(alpha[q] / 2) + tan(alpha[q + 1] / 2)) / norm), vertex[q]));
		}

		for (int k = 0; k < weight[i].size(); ++k)
		{
			WEIGHT[i] += weight[i][k].first;
		}
	}
	int size = inner_vertices.size();
	if (size > 0)
	{
		for (int j = 0; j < 1; ++j)
		{
			Eigen::SparseMatrix<double> A(size, size);
			std::vector<Eigen::VectorXd> B(2);
			B[0].resize(size);
			B[1].resize(size);
			B[0].setZero();
			B[1].setZero();
			for (int ai = 0; ai < size; ai++)
			{
				//A.startVec(ai);
				A.insert(ai, ai) = WEIGHT[ai];
				for (int bj = 0; bj < weight[ai].size(); bj++)
				{
					int idx = map_vertices_to_plot.at(weight[ai][bj].second.idx());
					int ind = idx - boundary_vertices.size();
					if (ind >= 0) //in inner
					{
						A.insert(ai, ind) = -weight[ai][bj].first;
					}
					else //in boundary
					{
						
						B[0][ai] += weight[ai][bj].first * plot[idx][0];
						B[1][ai] += weight[ai][bj].first * plot[idx][1];
					}
				}
			}
			A.makeCompressed();
			std::vector<Eigen::VectorXd> x;
			x.resize(2);

			Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> linearSolver;
			linearSolver.compute(A);
			x[0] = linearSolver.solve(B[0]);
			x[1] = linearSolver.solve(B[1]);
			for (int k = 0; k < inner_vertices.size(); k++)
			{
				plot[k + boundary_vertices.size()][0] = x[0][k];
				plot[k + boundary_vertices.size()][1] = x[1][k];
			}
		}
	}
}

void Para::setDimension(int height, int width)
{
	this->height = height;
	this->width = width;
}

void  Para::setTex(std::string name_)
{
	tex.setFilename(name_);
}
void  Para::Tex::setId(int view_, int id_)
{
	this->id[view_] = id_;
}
void  Para::Tex::setFilename(std::string filename_)
{
	if (filename_ != this->filename)
	{
		this->filename = filename_;
		for (int i = 0; i < id.size(); ++i) tex_garbage[i].push_back(id[i]);
		id.clear();
	}
}
void Para::Tex::setRepeatMode(unsigned int mode_)
{
	this->repeat_mode = mode_;
}
void Para::Tex::setRotate(float rotate_)
{
	this->rotate = rotate_;
}
void Para::Tex::setTranslateX(float trans_x)
{
	this->translate_x = trans_x;
}
void Para::Tex::setTranslateY(float trans_y)
{
	this->translate_y = trans_y;
}
void Para::Tex::setScaleX(float scale_x)
{
	this->scale_x = scale_x;
}

void Para::Tex::setScaleY(float scale_y)
{
	this->scale_y = scale_y;
}

void Para::Tex::setAlpha(float alpha_)
{
	if (alpha_ <= 0) this->alpha = 0;
	else if (alpha_ >= 1) this->alpha = 1;
	else this->alpha = alpha_;
}
void Para::Tex::setDepth(float depth_)
{
	this->depth = depth_;
}
const int Para::Tex::getId(int ref)
{
	std::map<int, int>::iterator it;
	if ((it = id.find(ref)) == id.end()) return -1;
	else return it->second;
}

void Para::Tex::clearGarbage(int view)
{
	std::vector<int> &tong = tex_garbage[view];
	if(!tong.empty())
	{
		GLuint catcher = tong.back();
		glDeleteTextures(1, &catcher);
		tong.pop_back();
	}
}