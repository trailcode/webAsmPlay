#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>

using namespace std;
using namespace glm;

Shader * Shader::create(const GLchar * vertexSource,
                        const GLchar * fragmentSource,
                        const GLchar * geometrySource)
{
    GLuint shaderProgram        = 0;
    GLint  vertInAttrib         = 0;
    GLint  colorInAttrib        = 0;
    GLint  MVP_In_Uniform       = 0;
    GLint  colorUniform         = 0;
    GLint  textureCoordsUniform = 0;
    GLint  success              = 0;

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    GLchar infoLog[512];

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

        dmess("ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog);

        return NULL;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);

        dmess("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog);

        return NULL;
    }

    GLuint geometryShader = 0;

    if(geometrySource)
    {
        #ifdef __EMSCRIPTEN__

            dmess("Error: WebGL does not support geometry shaders :(");

            abort();

        #endif

        // WebGL does not support geometry shaders :(

        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometrySource, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);

            dmess("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED: " << infoLog);

            return NULL;
        }
        
    }

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    if(geometrySource) { glAttachShader(shaderProgram, geometryShader) ;}
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram (shaderProgram);
    glUseProgram  (shaderProgram);

    // Specify the layout of the vertex data
    vertInAttrib         = glGetAttribLocation (shaderProgram, "vertIn");
    colorInAttrib        = glGetAttribLocation (shaderProgram, "vertColorIn");
    MVP_In_Uniform       = glGetUniformLocation(shaderProgram, "MVP");
    colorUniform         = glGetUniformLocation(shaderProgram, "colorIn");
    textureCoordsUniform = glGetUniformLocation(shaderProgram, "cube_texture");

    dmess("vertInAttrib  "         << vertInAttrib);
    dmess("colorInAttrib "         << colorInAttrib);
    dmess("MVP_In_Uniform "        << MVP_In_Uniform);
    dmess("colorUniform "          << colorUniform);
    dmess("textureCoordsUniform "  << textureCoordsUniform);

    glEnableVertexAttribArray(vertInAttrib);
    glVertexAttribPointer(vertInAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(colorInAttrib);

    glVertexAttribPointer(colorInAttrib, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    return new Shader(  shaderProgram,
                        vertInAttrib,
                        colorInAttrib,
                        MVP_In_Uniform,
                        colorUniform,
                        textureCoordsUniform);
}

Shader::Shader( const GLuint shaderProgram,
                const GLint  vertInAttrib,
                const GLint  colorInAttrib,
                const GLint  MVP_In_Uniform,
                const GLint  colorUniform,
                const GLint  textureCoordsUniform) : shaderProgram        (shaderProgram),
                                                     vertInAttrib         (vertInAttrib),
                                                     colorInAttrib        (colorInAttrib),
                                                     MVP_In_Uniform       (MVP_In_Uniform),
                                                     colorUniform         (colorUniform),
                                                     textureCoordsUniform (textureCoordsUniform)
{
} 

Shader::~Shader()
{
    // TODO Cleanup!
}

void Shader::bind()
{
    glUseProgram(shaderProgram);
}

void Shader::unbind()
{
    glUseProgram(0);
}

GLuint Shader::getProgramHandle() const { return shaderProgram ;}

vec4 Shader::setColor(const vec4 & color)
{
    glUniform4f(colorUniform, color.x, color.y, color.z, color.w);

    return color;
}

mat4 Shader::setMVP(const mat4 & MVP)
{
    glUniformMatrix4fv(MVP_In_Uniform, 1, false, glm::value_ptr(MVP));

    return MVP;
}

void Shader::enableVertexAttribArray(   const GLint       size,
                                        const GLenum      type,
                                        const GLboolean   normalized,
                                        const GLsizei     stride,
                                        const GLvoid    * pointer)
{
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(vertInAttrib);

    glVertexAttribPointer(vertInAttrib, size, type, normalized, stride, pointer);
}

void Shader::enableColorAttribArray(    const GLint       size,
                                        const GLenum      type,
                                        const GLboolean   normalized,
                                        const GLsizei     stride,
                                        const GLvoid    * pointer)
{
    // Specify the layout of the vertex data
    dmess("colorInAttrib aaa " << colorInAttrib); 
    glEnableVertexAttribArray(colorInAttrib);

    glVertexAttribPointer(colorInAttrib, size, type, normalized, stride, pointer);
}

GLuint Shader::setTexture1Slot(const GLuint slot) const
{
    glUniform1i(textureCoordsUniform, slot);

    return slot;
}
