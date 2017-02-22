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
	float			Yaw, scale;
	Vector3f        Pos;
	Quatf           Rot;
	Matrix4f        Mat;

protected:
	GLuint			program;
	VertexBuffer  * vertexBuffer;
	IndexBuffer   * indexBuffer;

	int				width, height;
	GLuint			texId;
	unsigned char *	texData;

	struct Vertex
	{
		Vector3f  Pos;
		DWORD     C;
		float     U, V;
	};

	Vertex structure;
	std::vector<Vertex>		Vertices;
	std::vector<GLushort>	Indices;

public:
	Model();
	Model(Vector3f pos, GLuint prog);
	~Model();

	Matrix4f& GetMatrix();
	void AllocateBuffers();
	void FreeBuffers();
	void setTexture(GLuint _texId, unsigned char *_texData, int _width, int _height);
	virtual void Render(Matrix4f view, Matrix4f stillView, Matrix4f proj);

	Vertex getStruct() { return structure; };
	float getScale() { return scale; };
	void SetStruct(float x, float y, float z, float w, float h);
	void RotationY(float diff);
	void setScale(float _scale);
	void Translate(float x, float y, float z);
	virtual void initScreen(float x, float y, float z, float width, float height) = 0;
	void initScreen(float x, float y, float z, float width, float height, bool reversing, bool curved);
};