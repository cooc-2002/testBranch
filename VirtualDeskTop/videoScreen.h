#pragma once
#include "Model.h"
class videoScreen :
	public Model
{
public:
	videoScreen();
	videoScreen(Vector3f pos, GLuint prog);
	~videoScreen();

	virtual void initScreen(float x, float y, float z, float width, float height);
};

