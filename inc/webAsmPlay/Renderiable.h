#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <vector>
#include <glm/glm.hpp>

namespace geos
{
    namespace geom
    {
        class Geometry;
    }
}

class Renderiable
{
public:

    virtual ~Renderiable();

    virtual void render(const glm::mat4 & MVP) const = 0;

    static Renderiable * create(const geos::geom::Geometry * geom);

    glm::vec4 setFillColor(const glm::vec4 & fillColor);
    glm::vec4 getFillColor() const;

    glm::vec4 setOutlineColor(const glm::vec4 & outlineColor);
    glm::vec4 getOutlineColor() const;

protected:

    Renderiable();

    static void ensureShader();

    glm::vec4 fillColor;
    glm::vec4 outlineColor;

    static GLuint  shaderProgram;
    static GLint   posAttrib;
    static GLint   MVP_Attrib;
    static GLint   colorAttrib;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__