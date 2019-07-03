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
#pragma once

#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/Types.h>

typedef std::vector<std::pair<std::string, GLint &>> Variables;

typedef std::vector<std::pair<size_t, const std::string>> GLSL;

class ShaderProgram
{
public:

	static ShaderProgram * create(	const GLSL		& programs,
									const Variables	& attributes	= Variables(),
									const Variables	& uniforms		= Variables(),
									const Variables & uniformBlocks = Variables());

    void bind();

    GLuint getProgramHandle() const;

    GLint getUniformLoc  (const std::string & name) const;
    GLint getAttributeLoc(const std::string & name) const;

    void setUniform  (const GLint location, const glm::mat4 & value) const;
    void setUniform  (const GLint location, const glm::vec4 & value) const;
    void setUniform  (const GLint location, const glm::vec3 & value) const;
    void setUniformf (const GLint location, const float     & value) const;
    void setUniformi (const GLint location, const GLint     & value) const;

private:

    ShaderProgram(	const GLuint                                   shaderProgram,
					const std::unordered_map<std::string, GLint> & attributes,
					const std::unordered_map<std::string, GLint> & uniforms,
					const std::unordered_map<std::string, GLint> & uniformBlocks);

    ~ShaderProgram();

    GLuint m_shaderProgram;
    
    std::unordered_map<std::string, GLint> m_attributes;
	std::unordered_map<std::string, GLint> m_uniforms;
	std::unordered_map<std::string, GLint> m_uniformBlocks;
};
