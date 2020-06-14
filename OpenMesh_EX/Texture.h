#pragma once
#include <vgl.h>

#include <string.h>
#include <vector>

#include <opencv\cv.h>
#include <opencv\highgui.h>

namespace Texture {
	bool	LoadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
	GLuint	GenTexture(const char* filepath);
	void ScreenShot(std::string& file);
}