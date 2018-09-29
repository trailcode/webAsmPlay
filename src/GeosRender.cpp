#define GLEW_STATIC
#include <GL/glew.h>
#include <webAsmPlay/debug.h>
#include <webAsmPlay/GeosRender.h>

using namespace geos::geom;

namespace
{
    GLuint vao;
    GLuint shaderProgram2;
    GLint posAttrib;
    GLint MVP_Attrib;
}

bool GeosRender::initOpenGL()
{

    return true;
}

Polygon * GeosRender::render(Polygon * poly)
{
    
    return poly;
}