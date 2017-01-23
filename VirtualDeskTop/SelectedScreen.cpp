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

	Vertex tempStruct = temp->getStruct();
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

	structure.Pos = Vector3f(x, y, z);
	structure.U = w;
	structure.V = h;

	float rs = radian / division;
	float ir = x;

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

	for (i = 0; i < Vertices.size(); i++)
		Indices.push_back(index++);

	AllocateBuffers();
}

void SelectedScreen::Render(Matrix4f view, Matrix4f stillView, Matrix4f proj) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_BGR, GL_UNSIGNED_BYTE, texData);

	Matrix4f combined = proj * view * sM->GetMatrix();

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "Texture0"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
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

	glLineWidth(5);
	glDrawElements(GL_LINE_LOOP, Indices.size(), GL_UNSIGNED_SHORT, NULL);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(colorLoc);
	glDisableVertexAttribArray(uvLoc);
	//glDisable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}