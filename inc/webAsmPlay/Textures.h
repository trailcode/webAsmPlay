#ifndef __WEB_ASM_PLAY_TEXTURES_H__
#define __WEB_ASM_PLAY_TEXTURES_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <vector>
#include <string>

class Textures
{
public:

    static Textures * getInstance();

    static GLuint load(const std::string & filename);

    static GLuint loadCube(const std::vector<std::string> & files);

private:

    Textures();
    ~Textures();

};

#endif // __WEB_ASM_PLAY_TEXTURES_H__