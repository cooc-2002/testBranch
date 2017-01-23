#include "BackgroundScreen.h"
#define PI 3.14159265359

BackgroundScreen::BackgroundScreen()
{
}

BackgroundScreen::BackgroundScreen(Vector3f pos, GLuint prog) : Model(pos, prog)
{
}

BackgroundScreen::~BackgroundScreen()
{
}

void BackgroundScreen::initScreen(float x, float y, float z, float w, float h){
	int i, division = 100, index = 0;
	float radian = w / z;

	structure.Pos = Vector3f(x, y, z);
	structure.U = w;
	structure.V = h;

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
	float ir = (PI + radian) / 2.0f;
	//float ir = acos(x / z);

	// Generate a quad for each box face
	Vertex temp;
	if(texId)
		temp.C = 0x00ffffff;

	temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f; temp.V = 1.0f;
	Vertices.push_back(temp);
	temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f; temp.V = 0.0f;
	Vertices.push_back(temp);

	for (i = 1; i <= division; i++) {
		ir -= rs;
		temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 0;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 1.0f;
		Vertices.push_back(temp);
	}
}