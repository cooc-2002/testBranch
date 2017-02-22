#include "videoScreen.h"

videoScreen::videoScreen(){}

videoScreen::videoScreen(Vector3f pos, GLuint prog) : Model(pos, prog){}

videoScreen::~videoScreen(){}

void videoScreen::initScreen(float x, float y, float z, float w, float h) {
	int i, division = 100, index = 0;
	float radian = w / z;
	SetStruct(x, y, z, w, h);

	Indices.push_back(index++);
	Indices.push_back(index++);
	Indices.push_back(index++);
	Indices.push_back(index);

	for (i = 1; i < division; i++) {
		Indices.push_back(index--);
		Indices.push_back(index);
		index += 2;
		Indices.push_back(index++);
		Indices.push_back(index);
	}

	float ts = 1.0 / division;
	float rs = radian / division;
	float ir = x;

	// Generate a quad for each box face
	Vertex temp;
	if (texId)
		temp.C = 0x00ffffff;
	else
		temp.C = 0xff0000ff;

	temp.Pos = Vector3f(ir, y + h, z); temp.U = 1.0f; temp.V = 0.0f;
	Vertices.push_back(temp);
	temp.Pos = Vector3f(ir, y, z); temp.U = 1.0f; temp.V = 1.0f;
	Vertices.push_back(temp);

	for (i = 1; i <= division; i++) {
		ir -= rs;
		temp.Pos = Vector3f(ir, y, z); temp.U = 1.0f - ts*i; temp.V = 1.0f;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(ir, y + h, z); temp.U = 1.0f - ts*i; temp.V = 0.0f;
		Vertices.push_back(temp);
	}
}