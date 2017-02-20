#include "Scene.h"
#include "Model.h"
#include "ScreenCopy.h"
#include "videoScreen.h"
#include "BackgroundScreen.h"
#include "SelectedScreen.h"
#include "SeeThroughScreen.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "videoDevice/RawImage.h"
#include "videoDevice/VideoDevice.h"

#include <Mfidl.h>
#include <mfapi.h>
#pragma comment(lib, "mf")
#pragma comment(lib, "Mfplat")

#ifndef OVR_DEBUG_LOG
#define OVR_DEBUG_LOG(x)
#endif

#define PI 3.14159265359

Scene::Scene() {
	bgImg = NULL;
	width = height = 0;
	scale = 0.0f;
	screenCopy = new ScreenCopy;
	Init();
}

void Scene::Release(){
	Models.clear();
	if (program[0]){
		glDeleteProgram(program[0]);
		program[0] = 0;
	}

	if (program[1]) {
		glDeleteProgram(program[1]);
		program[1] = 0;
	}
}

Scene::~Scene(){
	if(screenCopy != NULL) delete screenCopy;
	vd.clear();

	Release();
}

void Scene::Render(Matrix4f stillview, Matrix4f view, Matrix4f proj){
	screenCopy->ScreenUpdate();
	if (swapScreen == 1)
		NextScreen();
	if (swapScreen == 2)
		PrevScreen();

	for (int i = 0; i < Models.size(); ++i)
		Models[i]->Render(view, stillview, proj);
}

GLuint Scene::CreateShader(GLenum type, const GLchar* src){
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint r;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if (!r){
		GLchar msg[1024];
		glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
		if (msg[0])
			OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
		return 0;
	}

	return shader;
}

void Scene::Init()
{
	//Initialize shader
	InitShader();

	//Initialize cameras
	InitCams();

	GLuint screenTexId;
	glGenTextures(1, &screenTexId);

	Model *m;
	unsigned char *texData;
	//texData = (vd[0].getRawImageOut())->getpPixels();

	m = new SeeThroughScreen(Vector3f(0, 0, 0), program[0]);
	//m = new BackgroundScreen(Vector3f(0, 0, 0), program);  // See through screen
	//m->setTexture(texId[0], texData, vd[0].getWidth(), vd[0].getHeight());
	//m->setTexture(texId[0], bgImg, width, height);
	//m->setTexture(screenTexId, screenCopy->screenData, screenCopy->width, screenCopy->height);
	m->initScreen(-96.0f / 2.0f, -60.0f / 2.0f, 40.0f, 96.0f, 60.0f);
	m->AllocateBuffers();
	Models.push_back(m);

	pM = new SelectedScreen(Vector3f(0, 0, 0), program[0]);
	m = pM;
	Models.push_back(m);

	float initPoint = 3.141592f/2.0f;

	for (int i = 0; i < vd.capacity(); i++) {
		texData = (vd[i]->getRawImageOut())->getpPixels();
		//m = new BackgroundScreen(Vector3f(0, 0, -10), program[1]);  // See through screen
		m = new videoScreen(Vector3f(0, 0, -14.5), program[1]);
		//m->setTexture(screenTexId, screenCopy->screenData, screenCopy->width, screenCopy->height);
		m->setTexture(texId[i], texData, vd[i]->getWidth(), vd[i]->getHeight());
		m->initScreen(initPoint + i*0.64f/15.0f, 1.0f, 15.0f, 0.64f, 0.48f);
		m->AllocateBuffers();
		Models.push_back(m);
	}

	////m = new SeeThroughScreen(Vector3f(0, 0, 0), program);
	//m = new BackgroundScreen(Vector3f(0, 0, 0), program[0]);  // See through screen
	//m->setTexture(screenTexId, screenCopy->screenData, screenCopy->width, screenCopy->height);
	//m->initScreen(-96.0f / 2.0f, -60.0f / 2.0f, 40.0f, 96.0f, 60.0f);
	//m->AllocateBuffers();
	//Models.push_back(m);

	//pM = new SelectedScreen(Vector3f(0, 0, 0), program[0]);
	//m = pM;
	//Models.push_back(m);

	//float initPoint = PI / 2.0f;

	//for (int i = 1; i < numCam; i++) {
	//	texData = (vd[i].getRawImageOut())->getpPixels();
	//	m = new videoScreen(Vector3f(0, 0, 0), program[1]);
	//	m->setTexture(texId[i], texData, vd[i].getWidth(), vd[i].getHeight());
	//	m->initScreen(initPoint + i*6.4f/10.0f, -2.0f, 15.0f, 6.4f*1.5, 4.8f*1.5);
	//	//m->initScreen(initPoint + i*PI / 4.0f, 0.5f, 1.0f, 0.64f, 0.48f);
	//	m->AllocateBuffers();
	//	Models.push_back(m);
	//}

	sM = Models.begin()+2;
	pM->initScreen(*sM);
	pM->AllocateBuffers();
}

void Scene::NextScreen() {
	swapScreen = 0;
	if (sM < Models.end() - 1) {
		sM++;
		pM->initScreen(Models[3]);
		pM->AllocateBuffers();
	}
}

void Scene::PrevScreen() {
	swapScreen = 0;
	if (sM > Models.begin() + numCam) {
		sM--;
		pM->initScreen(Models[2]);
		pM->AllocateBuffers();
	}
}

void Scene::SetNextScreen() {
	swapScreen = 1;
}

void Scene::SetPrevScreen() {
	swapScreen = 2;
}

void Scene::RotationY(float diff) {
	(*sM)->RotationY(diff);
}

void Scene::Translate(float x, float y, float z) {
	(*sM)->Translate(x, y, z);
}

void Scene::ZoomIn() {
	//scale += 0.1f;
	(*sM)->setScale(1.1f);
}

void Scene::ZoomOut() {
	//scale -= 0.1f;
	(*sM)->setScale(1.0f/1.1f);
}

int Scene::InitCams() {
	int i;
	HRESULT hr = S_OK;
	IMFAttributes *pAttributes;
	hr = MFCreateAttributes(&pAttributes, 1);
	if (SUCCEEDED(hr))
		hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &numCam);

	wchar_t *name;
	
	vd.clear();
	texId = new GLuint [numCam];
	videoDevice *temp;
	for (i = 0; i < numCam; i++) {
		ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &name, NULL);
		if (wcscmp(name, L"OvrvisionPro")) {
			temp = new videoDevice;
			OpenCamera(temp, ppDevices[i], i);
			vd.push_back(temp);
		}
		CoTaskMemFree(name);
	}
	glGenTextures(numCam, texId);

	return numCam;
}

void Scene::SetCamImage(unsigned char *img, unsigned int _width, unsigned int _height) {
	Models[0]->setTexture(texId[0], img, _width, _height);
	bgImg = img;
	width = _width;
	height = _height;
}

int Scene::OpenCamera(videoDevice *vd, IMFActivate * pActivate, unsigned int Id) {
	int mediaType = 0;

	if (SUCCEEDED(vd->readInfoOfDevice(pActivate, Id))) {
		pActivate->Release();
		pActivate = NULL;

		//vd->setupDevice(6);
		//vd->setupDevice(320, 240, 15);
		int fps = 60;
		int numFormat = vd->getCountFormats();
		for (int i = 0; i < numFormat; i++) {
			MediaType param = vd->getFormat(i);
			//printf("%d x %d with %d fps\n", param.height, param.width, param.MF_MT_FRAME_RATE);

			if (param.width == 320 && fps > param.MF_MT_FRAME_RATE) {
				fps = param.MF_MT_FRAME_RATE;
				mediaType = i;
			}
		}
		vd->setupDevice(mediaType);
		while (vd->isFrameNew() && vd->isDeviceSetup() == 0)
			Sleep(10);

		return 1;
	}
	else
		return 0;
}

void Scene::InitShader() {
	//for shader and make program and Construct geometry
	static const GLchar* VertexShaderSrc =
		"#version 150\n"
		"uniform mat4 matWVP;\n"
		"uniform float Scale;\n"
		"in      vec4 Position;\n"
		"in      vec4 Color;\n"
		"in      vec2 TexCoord;\n"
		"out     vec2 oTexCoord;\n"
		"out     vec4 oColor;\n"
		"void main()\n"
		"{\n"
		"	vec4 Vertex;"
		"	Vertex = Position;\n"
		"	Vertex.w = 1;\n"
		"   gl_Position = (matWVP * Vertex);\n"
		"   oTexCoord   = TexCoord;\n"
		"   oColor.rgb  = pow(Color.rgb, vec3(2.2));\n"   // convert from sRGB to linear
		"   oColor.a    = Color.a;\n"
		"}\n";

	static const char* FragmentShaderSrc =
		"#version 150\n"
		"uniform sampler2D Texture0;\n"
		"in      vec4      oColor;\n"
		"in      vec2      oTexCoord;\n"
		"out     vec4      FragColor;\n"
		"void main()\n"
		"{\n"
		"	if(oColor.a == 0)\n"
		"	{\n"
		"		FragColor = texture2D(Texture0, oTexCoord);\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		FragColor = oColor;\n"
		"	}\n"
		"}\n";

	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);
	program[0] = glCreateProgram();
	glAttachShader(program[0], vshader);
	glAttachShader(program[0], fshader);

	glLinkProgram(program[0]);

	glDetachShader(program[0], vshader);
	glDetachShader(program[0], fshader);

	GLint r;
	glGetProgramiv(program[0], GL_LINK_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetProgramInfoLog(program[0], sizeof(msg), 0, msg);
		OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	static const GLchar* VertexShaderSrc2 =
		"#version 150\n"
		"uniform mat4 matWVP;\n"
		"uniform float Scale;\n"
		"in      vec4 Position;\n"
		"in      vec4 Color;\n"
		"in      vec2 TexCoord;\n"
		"out     vec2 oTexCoord;\n"
		"out     vec4 oColor;\n"
		"void main()\n"
		"{\n"
		"	vec4 Vertex;"
		"	Vertex.x = Position.z * cos(Position.x * Scale);\n"
		"	Vertex.y = Position.y * Scale;\n"
		"	Vertex.z = Position.z * sin(Position.x * Scale);\n"
		"	Vertex.w = 1;\n"
		"   gl_Position = (matWVP * Vertex);\n"
		"   oTexCoord   = TexCoord;\n"
		"   oColor.rgb  = pow(Color.rgb, vec3(2.2));\n"   // convert from sRGB to linear
		"   oColor.a    = Color.a;\n"
		"}\n";

	static const char* FragmentShaderSrc2 =
		"#version 150\n"
		"uniform sampler2D Texture0;\n"
		"in      vec4      oColor;\n"
		"in      vec2      oTexCoord;\n"
		"out     vec4      FragColor;\n"
		"void main()\n"
		"{\n"
		"	if(oColor.a == 0)\n"
		"	{\n"
		"		FragColor = texture2D(Texture0, oTexCoord);\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		FragColor = oColor;\n"
		"	}\n"
		"}\n";

	vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc2);
	fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc2);
	program[1] = glCreateProgram();
	glAttachShader(program[1], vshader);
	glAttachShader(program[1], fshader);

	glLinkProgram(program[1]);

	glDetachShader(program[1], vshader);
	glDetachShader(program[1], fshader);

	r;
	glGetProgramiv(program[1], GL_LINK_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetProgramInfoLog(program[1], sizeof(msg), 0, msg);
		OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
}
