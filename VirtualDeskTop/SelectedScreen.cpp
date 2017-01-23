#include "SelectedScreen.h"
#include "OVR_Buffers.h"

#define PI 3.14159265359

SelectedScreen::SelectedScreen()
{
}

SelectedScreen::SelectedScreen(Vector3f pos, GLuint prog) : Model(pos, prog)
{
}

SelectedScreen::~SelectedScreen()
{
}

void SelectedScreen::initScreen(Model *temp) {
	sM = temp;

	Vertex tempStruct = sM->getStruct();
	float x = tempStruct.Pos.x;
	float y = tempStruct.Pos.y;
	float z = tempStruct.Pos.z;
	float w = tempStruct.U;
	float h = tempStruct.V;

	initScreen(x, y, z, w, h);
}

void SelectedScreen::initScreen(float x, float y, float z, float w, float h) {
	int i, division = 100, index = 0;
	float radian = w / z;
	SetStruct(x, y, z, w, h);

	float rs = radian / division;
	float ir = x;

	FreeBuffers();
	Vertices.clear();
	Indices.clear();

	// Generate a quad for each box face
	Vertex temp;
	temp.C = 0xff0000ff;

	temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir));
	Vertices.push_back(temp);
	for (i = 1; i <= division; i++) {
		ir -= rs;
		temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir));
		Vertices.push_back(temp);
	}

	temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir));
	Vertices.push_back(temp);
	for (i = 1; i <= division; i++) {
		ir += rs;
		temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir));
		Vertices.push_back(temp);
	}

	for (i = 1; i <= Vertices.size(); i++)
		Indices.push_back(index++);
}

void SelectedScreen::Render(Matrix4f view, Matrix4f stillView, Matrix4f proj) {
	Matrix4f combined = proj * view * sM->GetMatrix();

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "Texture0"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->buffer);

	GLuint posLoc = glGetAttribLocation(program, "Position");
	GLuint colorLoc = glGetAttribLocation(program, "Color");
	GLuint uvLoc = glGetAttribLocation(program, "TexCoord");

	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(colorLoc);
	glEnableVertexAttribArray(uvLoc);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, Pos));
	glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, C));
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, U));

	glLineWidth(7);
	glDrawElements(GL_LINE_LOOP, Indices.size(), GL_UNSIGNED_SHORT, NULL);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(colorLoc);
	glDisableVertexAttribArray(uvLoc);
	//glDisable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}