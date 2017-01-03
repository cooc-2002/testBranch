#include "Model.h"
#include "OVR_Buffers.h"

Model::Model(Vector3f pos, GLuint prog) :
	Pos(pos),
	Rot(),
	Mat(),
	vertexBuffer(nullptr),
	indexBuffer(nullptr),
	program(prog)
{
	texId = 0;
	width = 0; height = 0;
}

Model::~Model() {
	FreeBuffers(); 
}

Matrix4f& Model::GetMatrix()
{
	Mat = Matrix4f(Rot);
	Mat = Matrix4f::Translation(Pos) * Mat;
	return Mat;
}

void Model::AllocateBuffers()
{
	vertexBuffer = new VertexBuffer(&(Vertices[0]), Vertices.size() * sizeof(Vertices[0]));
	indexBuffer = new IndexBuffer(&(Indices[0]), Indices.size() * sizeof(Indices[0]));
}

void Model::FreeBuffers()
{
	delete vertexBuffer; vertexBuffer = nullptr;
	delete indexBuffer; indexBuffer = nullptr;
}

void Model::AddSolidQuad(float x, float y, float z, float _width, float _height, bool reversing)
{
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(3);

	// Generate a quad for each box face
	Vertex temp;
	if (reversing) {
		temp.Pos = Vector3f(x, y, z); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x + _width, y, z); temp.U = 0.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x + _width, y + _height, z); temp.U = 0.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x, y + _height, z); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
	}
	else {
		temp.Pos = Vector3f(x, y, z); temp.U = 1.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x + _width, y, z); temp.U = 0.0f; temp.V = 1.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x + _width, y + _height, z); temp.U = 0.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
		temp.Pos = Vector3f(x, y + _height, z); temp.U = 1.0f; temp.V = 0.0f; temp.C = 0xffffffff;
		Vertices.push_back(temp);
	}
}

void Model::setTexture(GLuint _texId, unsigned char *_texData, int _width, int _height) {
	texId = _texId;
	texData = _texData;
	width = _width;
	height = _height;
}

void Model::Render(Matrix4f view, Matrix4f proj)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		0, GL_BGR, GL_UNSIGNED_BYTE, texData);

	Matrix4f combined = proj * view * GetMatrix();
	
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