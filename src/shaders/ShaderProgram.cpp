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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/ShaderProgram.h>

using namespace std;
using namespace glm;

ShaderProgram * ShaderProgram::create(const GLchar    * vertexSource,
                                      const GLchar    * fragmentSource,
                                      const Variables & attributes,
                                      const Variables & uniforms)
{
    return create(vertexSource, fragmentSource, NULL, attributes, uniforms);
}

ShaderProgram * ShaderProgram::create(const GLchar    * vertexSource,
                                      const GLchar    * fragmentSource,
                                      const GLchar    * geometrySource,
                                      const Variables & attributes,
                                      const Variables & uniforms)
{
    GLuint shaderProgram = 0;
    GLint  success       = 0;

    // Create and compile the vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    GL_CHECK(glShaderSource(vertexShader, 1, &vertexSource, NULL));

    GL_CHECK(glCompileShader(vertexShader));
    
    GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));

    GLchar infoLog[512];

    if (!success)
    {
        GL_CHECK(glGetShaderInfoLog(vertexShader, 512, NULL, infoLog));

        dmess("ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog);

        return NULL;
    }

    // Create and compile the fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    GL_CHECK(glShaderSource(fragmentShader, 1, &fragmentSource, NULL));

    GL_CHECK(glCompileShader(fragmentShader));

    GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));

    if (!success)
    {
        GL_CHECK(glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog));

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

        GL_CHECK(glShaderSource(geometryShader, 1, &geometrySource, NULL));

        GL_CHECK(glCompileShader(geometryShader));

        GL_CHECK(glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success));

        if (!success)
        {
            GL_CHECK(glGetShaderInfoLog(geometryShader, 512, NULL, infoLog));

            dmess("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED: " << infoLog);

            return NULL;
        }
        
    }

    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();

    GL_CHECK(glAttachShader(shaderProgram, vertexShader));

    if(geometrySource) { GL_CHECK(glAttachShader(shaderProgram, geometryShader)) ;}

    GL_CHECK(glAttachShader(shaderProgram, fragmentShader));

    GL_CHECK(glLinkProgram (shaderProgram));
    GL_CHECK(glUseProgram  (shaderProgram));

    unordered_map<string, GLint> uniformMap;
    unordered_map<string, GLint> attributeMap;

    for(const auto & variable : attributes)
    {
        variable.second = glGetAttribLocation(shaderProgram, variable.first.c_str());

        if(variable.second == -1)
        {
            dmess("Warning could not find shader attribute: " << variable.first);

            continue;
        }

        attributeMap[variable.first] = variable.second;
    }

    for(const auto & variable : uniforms)
    {
        variable.second = glGetUniformLocation(shaderProgram, variable.first.c_str());

        if(variable.second == -1)
        {
            dmess("Warning could not find shader uniform: " << variable.first);

            continue;
        }

        uniformMap[variable.first] = variable.second;
    }

    return new ShaderProgram(shaderProgram,
                             uniformMap,
                             attributeMap);
}

ShaderProgram::ShaderProgram(   const GLuint                         shaderProgram,
                                const unordered_map<string, GLint> & uniforms,
                                const unordered_map<string, GLint> & attributes) :  shaderProgram        (shaderProgram),
                                                                                    uniforms             (uniforms),
                                                                                    attributes           (attributes)
{
} 

ShaderProgram::~ShaderProgram()
{
    // TODO Cleanup!
}

void ShaderProgram::bind() { GL_CHECK(glUseProgram(shaderProgram)) ;}

GLuint ShaderProgram::getProgramHandle() const { return shaderProgram ;}

void ShaderProgram::enableVertexAttribArray(const GLint       vertexLoc,
                                            const GLint       size,
                                            const GLenum      type,
                                            const GLboolean   normalized,
                                            const GLsizei     stride,
                                            const GLvoid    * pointer)
{
    // Specify the layout of the vertex data
    GL_CHECK(glEnableVertexAttribArray(vertexLoc));

    GL_CHECK(glVertexAttribPointer(vertexLoc, size, type, normalized, stride, pointer));
}

GLint ShaderProgram::getUniformLoc(const string & name) const
{
    const auto i = uniforms.find(name);

    if(i == uniforms.end())
    {
        dmess("Error uniform: " << name << " not found!");

        return -1;
    }

    return i->second;
}

GLint ShaderProgram::getAttributeLoc(const string & name) const
{
    const auto i = attributes.find(name);

    if(i == attributes.end())
    {
        dmess("Error uniform: " << name << " not found!");

        return -1;
    }

    return i->second;
}

void ShaderProgram::setUniform (const GLint location, const mat4   & value) const { GL_CHECK(glUniformMatrix4fv(location, 1, false, value_ptr(value))) ;}
void ShaderProgram::setUniform (const GLint location, const vec4   & value) const { GL_CHECK(glUniform4fv      (location, 1,        value_ptr(value))) ;}
void ShaderProgram::setUniform (const GLint location, const vec3   & value) const { GL_CHECK(glUniform3fv      (location, 1,        value_ptr(value))) ;}
void ShaderProgram::setUniformf(const GLint location, const float  & value) const { GL_CHECK(glUniform1f       (location,                     value))  ;}
void ShaderProgram::setUniformi(const GLint location, const GLuint & value) const { GL_CHECK(glUniform1i       (location,                     value))  ;}