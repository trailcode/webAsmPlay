#ifndef VERTEXBUFFER_H_
#define VERTEXBUFFER_H_

#include <tceGeom/vec2.h>
#include <tceGeom/vec3.h>

class VertexBuffer
{
public:
	VertexBuffer();
	virtual ~VertexBuffer();
	
	tce::geom::Vec3f *GetVertices();
	tce::geom::Vec2f *GetTexCoords();
	tce::geom::Vec2f *GetTexCoords2();
	void AllocBuffer(int size);
	void SetTexCoordPointer(char which);
	void SetPointers();
	int GetSize();
private:
	tce::geom::Vec3f *verts;
	tce::geom::Vec2f *texCoords;
	tce::geom::Vec2f *texCoords2;
    int size;

};

#endif /*VERTEXBUFFER_H_*/
