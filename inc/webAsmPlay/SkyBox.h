#ifndef __WEB_ASM_PLAY_SKY_BOX_H__
#define __WEB_ASM_PLAY_SKY_BOX_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

class SkyBox
{
public:

    SkyBox();

    virtual ~SkyBox();

    void render();

private:

    static void ensureShader();

    const GLuint xpos;
    const GLuint xneg;
    const GLuint ypos;
    const GLuint yneg;
    const GLuint zpos;
    const GLuint zneg;
};

#endif // __WEB_ASM_PLAY_SKY_BOX_H__