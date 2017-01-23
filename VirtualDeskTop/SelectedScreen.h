#pragma once
#include "Model.h"
class SelectedScreen :
	public Model
{
private:
	Model *sM;

public:
	SelectedScreen();
	SelectedScreen(Vector3f pos, GLuint prog);
	~SelectedScreen();

	void initScreen(Model *temp);
	virtual void initScreen(float x, float y, float z, float width, float height);
	virtual void Render(Matrix4f view, Matrix4f stillView, Matrix4f proj);
};

