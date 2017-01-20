#pragma once

#include <vector>
#include "Extras/OVR_Math.h"
#include "GL/CAPI_GLE.h"

using namespace OVR;

struct VertexBuffer;
struct IndexBuffer;

class Model
{
private:
	Vector3f        Pos;
	Quatf           Rot;
	Matrix4f        Mat;
	GLuint			program;
	VertexBuffer  * vertexBuffer;
	IndexBuffer   * indexBuffer;

	int				width, height;
	GLuint			texId;
	unsigned char *	texData;

protected:
	struct Vertex
	{
		Vector3f  Pos;
		DWORD     C;
		float     U, V;
	};

	std::vector<Vertex>		Vertices;
	std::vector<GLushort>	Indices;

public:
	Model();
	Model(Vector3f pos, GLuint prog);
	~Model();

	Matrix4f& GetMatrix();
	void AllocateBuffers();
	void FreeBuffers();
	virtual void initScreen(float x, float y, float z, float width, float height, bool reversing = 0, bool curved = 0);
	void setTexture(GLuint _texId, unsigned char *_texData, int _width, int _height);
	void Render(Matrix4f view, Matrix4f proj);
};