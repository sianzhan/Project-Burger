#include "Texture.h"
#include "Mesh/GUA_OM.h"
#include "Mesh/DP.h"
#include "Eigen/Sparse"
#include <memory>
#include <unordered_map>
#pragma once
#define PI 3.14159
//defined in MyForm.h, should be the same
#define PATH "Texture\\" 
class Para
{
	class Tex
	{
		std::string filename;
		std::map<int, int> id;
		unsigned int repeat_mode = GL_CLAMP_TO_EDGE;
		float scale_x = 1;
		float scale_y = 1;
		float translate_x = 0;
		float translate_y = 0;
		float rotate = 0;
		float alpha = 1.0;
		float depth = 2.0;

		static std::map<int, std::vector<int> >tex_garbage;

	public:
		void setId(int view, int id);
		void setFilename(std::string);
		void setRepeatMode(unsigned int);
		void setRotate(float);
		void setTranslateX(float);
		void setTranslateY(float);
		void setScaleX(float);
		void setScaleY(float);
		void setAlpha(float);
		void setDepth(float);
		const int getId(int ref);
		const std::string& getFilename() { return this->filename; }
		const unsigned int getRepeatMode() { return this->repeat_mode; }
		const float& getRotate() { return this->rotate; }
		const float& getTranslateX() { return this->translate_x; }
		const float& getTranslateY() { return this->translate_y; }
		const float& getScaleX() { return this->scale_x; }
		const float& getScaleY() { return this->scale_y; }
		const float& getAlpha() { return this->alpha; }
		const float& getDepth() { return this->depth; }
		static void clearGarbage(int view);
	};
	//Map from Para vertex to Mesh vertex
	//Reverse map of vertice_record
	std::unordered_map<int, int> map_vertices_para_to_ref;
	std::unordered_map<int, int> map_vertices_to_plot; //Map from Para Vertices IDX to Plot Vector ID

	std::vector< std::vector<double> > plot; //Panned Mesh into plan by parameterization
	std::vector<OpenMesh::VertexHandle> boundary_vertices;
	std::vector<OpenMesh::VertexHandle> inner_vertices;

	std::set<OpenMesh::FaceHandle> face_record; //keep track of Mesh's faces
	std::map<std::vector<double>, int> vertice_record; //keep track of Mesh's vertices

	std::shared_ptr<Tri_Mesh> ref = nullptr;
	std::unique_ptr<Tri_Mesh> para = nullptr;
	Tex tex;

	static int height;
	static int width;

	bool hasUpdate = 1;

	/*Helper Function Begin*/
	void buildVerticesMap();
	void panVerticesToPlot();
	void megaCalculation();
	/*Helper Function End*/

public:
	void setRef(std::shared_ptr<Tri_Mesh>);

	void setTex(std::string);
	Tex& getTex() { return this->tex; }
	const std::set<OpenMesh::FaceHandle>& getFaceRecord() { return this->face_record; }
	void setDimension(int height, int width);

	bool addFace(OpenMesh::FaceHandle);
	bool deleteFace(OpenMesh::FaceHandle);
	void build();
	void pan();
	void drawSolidWireframe(int view,bool = 0) ;
	void drawPlot(int view) ;

};