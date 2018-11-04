/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>

using namespace std;
using namespace glm;

Shader * Shader::create(const GLchar * vertexSource,
                        const GLchar * fragmentSource,
                        const StrVec & uniforms,
                        const StrVec & attributes)
{
    return create(vertexSource, fragmentSource, NULL, uniforms, attributes);
}

Shader * Shader::create(const GLchar * vertexSource,
                        const GLchar * fragmentSource,
                        const GLchar * geometrySource,
                        const StrVec & uniforms,
                        const StrVec & attributes)
{
    GLuint shaderProgram        = 0;
    GLint  vertInAttrib         = 0;
    GLint  colorInAttrib        = 0;
    GLint  colorsInUniform      = 0;
    GLint  MVP_In_Uniform       = 0;
    GLint  colorUniform         = 0;
    GLint  textureCoordsUniform = 0;
    GLint  success              = 0;

    // Create and compile the vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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
    colorsInUniform      = glGetUniformLocation(shaderProgram, "colorsIn");
    textureCoordsUniform = glGetUniformLocation(shaderProgram, "cube_texture");

    unordered_map<string, GLint> uniformMap({   {"MVP",             MVP_In_Uniform},
                                                {"colorIn",         colorUniform},
                                                {"cube_texture",    textureCoordsUniform},
                                                {"colorsIn",        colorsInUniform}});

    unordered_map<string, GLint> attributeMap({ {"vertIn",          vertInAttrib},
                                                {"vertColorIn",     colorInAttrib}});

    for(const auto & variable : uniforms)   { uniformMap  [variable] = glGetUniformLocation(shaderProgram, variable.c_str()) ;}
    for(const auto & variable : attributes) { attributeMap[variable] = glGetAttribLocation(shaderProgram,  variable.c_str()) ;}

    glEnableVertexAttribArray(vertInAttrib);
    glVertexAttribPointer(vertInAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(colorInAttrib);

    glVertexAttribPointer(colorInAttrib, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    
    return new Shader(  shaderProgram,
                        vertInAttrib,
                        colorInAttrib,
                        colorsInUniform,
                        MVP_In_Uniform,
                        colorUniform,
                        textureCoordsUniform,
                        uniformMap,
                        attributeMap);
}

Shader::Shader( const GLuint                         shaderProgram,
                const GLint                          vertInAttrib,
                const GLint                          colorInAttrib,
                const GLint                          colorsInUniform,
                const GLint                          MVP_In_Uniform,
                const GLint                          colorUniform,
                const GLint                          textureCoordsUniform,
                const unordered_map<string, GLint> & uniforms,
                const unordered_map<string, GLint> & attributes) :  shaderProgram        (shaderProgram),
                                                                    vertInAttrib         (vertInAttrib),
                                                                    colorInAttrib        (colorInAttrib),
                                                                    colorsInUniform      (colorsInUniform),
                                                                    MVP_In_Uniform       (MVP_In_Uniform),
                                                                    colorUniform         (colorUniform),
                                                                    textureCoordsUniform (textureCoordsUniform),
                                                                    uniforms             (uniforms),
                                                                    attributes           (attributes)
{
} 

Shader::~Shader()
{
    // TODO Cleanup!
}

void Shader::bind(const mat4 & MVP, const mat4 & MV)
{
    glUseProgram(shaderProgram);

    setUniform(MVP_In_Uniform, MVP);
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
    glEnableVertexAttribArray(colorInAttrib);

    glVertexAttribPointer(colorInAttrib, size, type, normalized, stride, pointer);
}

GLuint Shader::setTexture1Slot(const GLuint slot) const
{
    glUniform1i(textureCoordsUniform, slot);

    return slot;
}

GLint Shader::getUniformLoc(const string & name) const
{
    const auto i = uniforms.find(name);

    if(i == uniforms.end())
    {
        dmess("Error uniform: " << name << " not found!");

        return -1;
    }

    return i->second;
}

GLint Shader::getAttributeLoc(const string & name) const
{
    const auto i = attributes.find(name);

    if(i == attributes.end())
    {
        dmess("Error uniform: " << name << " not found!");

        return -1;
    }

    return i->second;
}

void Shader::setUniform (const GLint location, const mat4   & value) const { glUniformMatrix4fv(location, 1, false, value_ptr(value)) ;}
void Shader::setUniform (const GLint location, const vec4   & value) const { glUniform4fv      (location, 1,        value_ptr(value)) ;}
void Shader::setUniform (const GLint location, const float  & value) const { glUniform1f       (location,                     value)  ;}
void Shader::setUniformi(const GLint location, const GLuint & value) const { glUniform1i       (location,                     value)  ;}