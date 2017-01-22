#pragma once
#include "Model.h"
class BackgroundScreen :
	public Model
{
public:
	BackgroundScreen();
	BackgroundScreen(Vector3f pos, GLuint prog);
	~BackgroundScreen();

	virtual void initScreen(float x, float y, float z, float width, float height);
};

