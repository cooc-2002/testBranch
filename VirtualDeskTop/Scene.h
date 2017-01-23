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
class SelectedScreen;
struct IMFActivate;
typedef unsigned int UINT32;

class Scene
{
private:
	int swapScreen;
	SelectedScreen *pM;
	std::vector<Model*>	Models;
	std::vector<Model*>::iterator sM;

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

	void NextScreen();
	void PrevScreen();
	void SetNextScreen();
	void SetPrevScreen();
	void RotationY(float diff);
	void Translate(float x, float y, float z);

	int InitCams();
	int OpenCamera(videoDevice *vd, IMFActivate * pActivate, unsigned int Id);
};