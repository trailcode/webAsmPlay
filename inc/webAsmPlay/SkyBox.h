#ifndef __WEB_ASM_PLAY_SKY_BOX_H__
#define __WEB_ASM_PLAY_SKY_BOX_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <glm/mat4x4.hpp>

class SkyBox
{
public:

    SkyBox();

    virtual ~SkyBox();

    void render(const glm::mat4 & view, const glm::mat4 & projection);

private:

    static void ensureShader();

    GLuint texID;
    GLuint vbo;
    GLuint vao;

    glm::mat4 model;
};

#endif // __WEB_ASM_PLAY_SKY_BOX_H__