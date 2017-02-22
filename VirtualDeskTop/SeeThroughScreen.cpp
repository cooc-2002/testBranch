#include "SeeThroughScreen.h"
#include "OVR_Buffers.h"

SeeThroughScreen::SeeThroughScreen() {}

SeeThroughScreen::SeeThroughScreen(Vector3f pos, GLuint prog) : Model(pos, prog) {}

SeeThroughScreen::~SeeThroughScreen() {}

void SeeThroughScreen::initScreen(float x, float y, float z, float w, float h) {
	SetStruct(x, y, z, w, h);

	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(3);

	Vertex temp;
	temp.C = 0x00ffffff;
	//temp.Pos = Vector3f(1.58, 0.83, 0.99); temp.U = 1.0f; temp.V = 0.0f;
	//Vertices.push_back(temp);
	//temp.Pos = Vector3f(1.58, -0.67, 0.99); temp.U = 1.0f; temp.V = 1.0f;
	//Vertices.push_back(temp);
	//temp.Pos = Vector3f(-1.27, -0.67, 0.99); temp.U = 0.0f; temp.V = 1.0f;
	//Vertices.push_back(temp);
	//temp.Pos = Vector3f(-1.27, 0.83, 0.99); temp.U = 0.0f; temp.V = 0.0f;
	//Vertices.push_back(temp);
	temp.Pos = Vector3f(x, y + h, z); temp.U = 1.0f; temp.V = 0.0f;
	Vertices.push_back(temp);
	temp.Pos = Vector3f(x, y, z); temp.U = 1.0f; temp.V = 1.0f;
	Vertices.push_back(temp);
	temp.Pos = Vector3f(x + w, y, z); temp.U = 0.0f; temp.V = 1.0f;
	Vertices.push_back(temp);
	temp.Pos = Vector3f(x + w, y + h, z); temp.U = 0.0f; temp.V = 0.0f;
	Vertices.push_back(temp);	
}

void SeeThroughScreen::Render(Matrix4f view, Matrix4f stillView, Matrix4f proj) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_BGRA, GL_UNSIGNED_BYTE, texData);

	Matrix4f combined = proj * stillView * GetMatrix();

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

	glDrawElements(GL_QUADS, Indices.size(), GL_UNSIGNED_SHORT, NULL);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(colorLoc);
	glDisableVertexAttribArray(uvLoc);
	//glDisable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(0);
}