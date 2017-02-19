#include "Model.h"
#include "OVR_Buffers.h"

#define PI 3.14159265359

Model::Model(){
	Pos = Vector3f(0, 0, 0);
	program = 0;
	texId = 0;
	width = 0; height = 0;
	Yaw = 0;
	scale = 1.0f;
	vertexBuffer = NULL;
	indexBuffer = NULL;
}

Model::Model(Vector3f pos, GLuint prog) :
	Pos(pos),
	Rot(),
	Mat(),
	vertexBuffer(nullptr),
	indexBuffer(nullptr),
	program(prog)
{
	Yaw = 0;
	texId = 0;
	width = 0; height = 0;
	scale = 1.0f;
}

Model::~Model() {
	FreeBuffers(); 
}

Matrix4f& Model::GetMatrix()
{
	Mat = Matrix4f::RotationY(Yaw);
	Mat = Matrix4f::Translation(Pos) * Mat;
	return Mat;
}

void Model::RotationY(float angle) {
	Yaw += angle;
}

void Model::Translate(float x, float y, float z) {
	Pos.x += x;
	Pos.y += y;
	Pos.z += z;
}

void Model::SetStruct(float x, float y, float z, float w, float h) {
	structure.Pos.x = x;
	structure.Pos.y = y;
	structure.Pos.z = z;
	structure.U = w;
	structure.V = h;
}

void Model::AllocateBuffers()
{
	vertexBuffer = new VertexBuffer(&(Vertices[0]), Vertices.size() * sizeof(Vertices[0]));
	indexBuffer = new IndexBuffer(&(Indices[0]), Indices.size() * sizeof(Indices[0]));
}

void Model::FreeBuffers()
{
	if (vertexBuffer != NULL) delete vertexBuffer; vertexBuffer = nullptr;
	if (indexBuffer != NULL) delete indexBuffer; indexBuffer = nullptr;
}

void Model::initScreen(float x, float y, float z, float w, float h, bool reversing, bool curved)
{
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
		index+=2;
		Indices.push_back(index++);
		Indices.push_back(index);
	}

	float s = w / division;
	float ts = 1.0 / division;
	float rs = radian / division;
	//float ir = (PI + radian) / 2.0f;
	float ir = acos(x/z);

	// Generate a quad for each box face
	Vertex temp;
	char state = (reversing << 1) | curved;
	switch (state) {
	case 3:
		temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);

		for (i = 1; i <= division; i++) {
			ir -= rs;
			temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 0; temp.C = 0xffffffff;
			Vertices.push_back(temp);
			temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 1.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
		}
		break;

	case 2:
		temp.Pos = Vector3f(x, y + h, z); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x, y, z); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);

		for (i = 1; i <= division; i++) {
			temp.Pos = Vector3f(x + s*i, y, z); temp.U = 1.0f - ts*i; temp.V = 0; temp.C = 0xffffffff;
			Vertices.push_back(temp);
			temp.Pos = Vector3f(x + s*i, y + h, z); temp.U = 1.0f - ts*i; temp.V = 1.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
		}
		break;

	case 1:
		temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);

		for (i = 1; i <= division; i++) {
			ir -= rs;
			temp.Pos = Vector3f(z*cos(ir), y, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 1.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
			temp.Pos = Vector3f(z*cos(ir), y + h, z*sin(ir)); temp.U = 1.0f - ts*i; temp.V = 0.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
		}
		break;

	case 0: default:
		temp.Pos = Vector3f(x, y + h, z); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x, y, z); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);

		for (i = 1; i <= division; i++) {
			temp.Pos = Vector3f(x + s*i, y, z); temp.U = 1.0f - ts*i; temp.V = 1.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
			temp.Pos = Vector3f(x + s*i, y + h, z); temp.U = 1.0f - ts*i; temp.V = 0.0f; temp.C = 0xffffffff;
			Vertices.push_back(temp);
		}
		break;
	}
}

void Model::setTexture(GLuint _texId, unsigned char *_texData, int _width, int _height) {
	texId = _texId;
	texData = _texData;
	width = _width;
	height = _height;
}

void Model::setScale(float _scale) {
	scale *= _scale;
	//Pos.z += _scale;
}

void Model::Render(Matrix4f view, Matrix4f stillView, Matrix4f proj)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, texData);

	Matrix4f combined = proj * view * GetMatrix();

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "Texture0"), 0);
	glUniform1f(glGetUniformLocation(program, "Scale"), scale);
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