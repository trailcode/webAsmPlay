
#ifdef WORKING

//#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <tceRender/vertexBuffer.h>

using namespace tce::geom;

VertexBuffer::VertexBuffer() {
	verts = NULL;
	texCoords = NULL;
	texCoords2 = NULL;
	size = -1;
}

VertexBuffer::~VertexBuffer() {
	// TODO: put destructor code here
}

Vec3f * VertexBuffer::GetVertices() {
	return verts;
}

Vec2f * VertexBuffer::GetTexCoords() {
	return texCoords;
}

Vec2f * VertexBuffer::GetTexCoords2() {
	return texCoords2;
}

void VertexBuffer::AllocBuffer(int size) {
	verts = new Vec3f[size];
	texCoords = new Vec2f[size];
	texCoords2 = new Vec2f[size];
	this->size = size;
}

void VertexBuffer::SetTexCoordPointer(char which) {
	switch (which) {
	case 1:

		// Assign the first texture pass to point to the normal texture coordinates
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vec2f), texCoords);

		break;
	case 2:

		// Assign the second texture pass to point to the light map texture coordinates
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vec2f), texCoords2);

		break;
	}
}

void VertexBuffer::SetPointers() {
	// Give OpenGL our vertices to use for vertex arrays
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), verts);

}

int VertexBuffer::GetSize() {
	return size;
}

#endif