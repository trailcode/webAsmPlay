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

#include <algorithm>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/ShaderProgram.h>

using namespace std;
using namespace glm;

namespace
{
	bool invalidChar (char c)  { return !(c>=0 && c <128); } 

	string stripUnicode(string str) 
	{ 
		str.erase(remove_if(str.begin(),str.end(), invalidChar), str.end());  

		return str;
	}

	bool compile(const pair<GLenum, const string> & typeAndFile, const GLuint program)
	{
		GLint success = 0;

		const string   _source = stripUnicode(readFile(get<1>(typeAndFile)));

		const GLchar * source  = _source.c_str();

		const GLuint shader = glCreateShader(get<0>(typeAndFile));

		GL_CHECK(glShaderSource(shader, 1, &source, NULL));

		GL_CHECK(glCompileShader(shader));
    
		GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));

		GLchar infoLog[2048];

		if (!success) 
		{
			GL_CHECK(glGetShaderInfoLog(shader, 2048, NULL, infoLog));

			dmess("GLSL compilation failed: " << _source);

			dmess("   " << infoLog);

			return false;
		}

		GL_CHECK(glAttachShader(program, shader));
	}
}

ShaderProgram * ShaderProgram::create(const GLSL & programs, const Variables& attributes, const Variables& uniforms)
{
	GLint  success       = 0;
	GLuint shaderProgram = glCreateProgram();

	for(const auto & i : programs)
	{
		if(!compile(i, shaderProgram)) { return nullptr ;}
	}

	GL_CHECK(glLinkProgram (shaderProgram));
    GL_CHECK(glUseProgram  (shaderProgram));

	unordered_map<string, GLint> uniformMap;
    unordered_map<string, GLint> attributeMap;

    for(const auto & variable : attributes)
    {
        variable.second = glGetAttribLocation(shaderProgram, variable.first.c_str());

        if(variable.second == -1)
        {
            dmess("Warning could not find shader attribute: " << variable.first << " Check it is being used in shader.");

            continue;
        }

        attributeMap[variable.first] = variable.second;
    }

    for(const auto & variable : uniforms)
    {
        variable.second = glGetUniformLocation(shaderProgram, variable.first.c_str());

        if(variable.second == -1)
        {
            dmess("Warning could not find shader uniform: " << variable.first << " Check it is being used in shader.");

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
                                const unordered_map<string, GLint> & attributes) :  m_shaderProgram (shaderProgram),
                                                                                    m_uniforms      (uniforms),
                                                                                    m_attributes    (attributes)
{
} 

ShaderProgram::~ShaderProgram()
{
    // TODO Cleanup!
}

void ShaderProgram::bind() { GL_CHECK(glUseProgram(m_shaderProgram)) ;}

GLuint ShaderProgram::getProgramHandle() const { return m_shaderProgram ;}

GLint ShaderProgram::getUniformLoc(const string & name) const
{
    const auto i = m_uniforms.find(name);

    if(i == m_uniforms.end())
    {
        dmess("Error uniform: " << name << " not found!");

        return -1;
    }

    return i->second;
}

GLint ShaderProgram::getAttributeLoc(const string & name) const
{
    const auto i = m_attributes.find(name);

    if(i == m_attributes.end())
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