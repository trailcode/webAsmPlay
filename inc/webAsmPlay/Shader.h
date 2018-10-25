#ifndef __WEB_ASM_PLAY_SHADER_H__
#define __WEB_ASM_PLAY_SHADER_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif
#include <string>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Shader
{
public:

    static Shader * create( const GLchar * vertexSource,
                            const GLchar * fragmentSource,
                            const GLchar * geometrySource = NULL);

    virtual ~Shader();

    void bind();

    void unbind();

    GLuint getProgramHandle() const;

    glm::vec4 setColor(const glm::vec4 & color);

    glm::mat4 setMVP(const glm::mat4 & MVP);

    void enableVertexAttribArray(   const GLint       size          = 2,
                                    const GLenum      type          = GL_FLOAT,
                                    const GLboolean   normalized    = GL_FALSE,
                                    const GLsizei     stride        = 0,
                                    const GLvoid    * pointer       = NULL);

    void enableColorAttribArray(    const GLint       size          = 4,
                                    const GLenum      type          = GL_FLOAT,
                                    const GLboolean   normalized    = GL_FALSE,
                                    const GLsizei     stride        = 0,
                                    const GLvoid    * pointer       = NULL);

    GLuint setTexture1Slot(const GLuint slot) const;

//protected:

    Shader( const GLuint shaderProgram,
            const GLint  vertInAttrib,
            const GLint  colorInAttrib,
            const GLint  MVP_In_Uniform,
            const GLint  colorUniform,
            const GLint  textureCoordsUniform);

//private:

    GLuint shaderProgram;
    GLint  vertInAttrib;
    GLint  colorInAttrib;
    GLint  MVP_In_Uniform;
    GLint  colorUniform;
    GLint  textureCoordsUniform;
};

#endif // __WEB_ASM_PLAY_SHADER_H__