#pragma once
#include "Model.h"
class SeeThroughScreen : public Model {
public:
	SeeThroughScreen();
	SeeThroughScreen(Vector3f pos, GLuint prog);
	~SeeThroughScreen();

	virtual void Render(Matrix4f view, Matrix4f stillView, Matrix4f proj);
	virtual void initScreen(float x, float y, float z, float w, float h);
};

