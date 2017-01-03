#pragma once
#include "Extras/OVR_Math.h"
#include "GL/CAPI_GLE.h"
#include <vector>
using namespace OVR;

namespace cv {
	class VideoCapture;
	class Mat;
};

class Model;
class ScreenCopy;
class videoDevice;
struct IMFActivate;
typedef unsigned int UINT32;

class Scene
{
private:
	std::vector<Model*>	Models;

	GLuint	*texId;
	GLuint	program;
	UINT32	numCam;
	IMFActivate		**ppDevices;
	videoDevice		*vd; 
	ScreenCopy		*screenCopy;

public:
	Scene();
	~Scene();

	void Init();
	void Release();
	void Render(Matrix4f stillview, Matrix4f view, Matrix4f proj);
	void InitShader();
	GLuint CreateShader(GLenum type, const GLchar* src);

	int InitCams();
	int OpenCamera(videoDevice *vd, IMFActivate * pActivate, unsigned int Id);
};