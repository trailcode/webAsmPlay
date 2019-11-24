#pragma once

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
  \copyright 2019
*/

#include <cstdlib>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <webAsmPlay/OpenGL_Util.h>

class Bubble
{
public:

	static Bubble * create(const nlohmann::json & bubble);

	Bubble();

	Bubble(	const size_t		  ID,
			const glm::dvec2	& pos,
			const glm::dvec2	& rollPitch,
			const double		  altitude);

	static void save(const std::string & fileName, const std::vector<Bubble *> & bubbles);

	static std::vector<Bubble *> load(const std::string & fileName);

	std::string getQuadKey() const;

	GLuint getCubeFaceTexture(const size_t face) const;

	GLuint getCachedCubeFaceTexture(const size_t face) const;

	const size_t		m_ID;
	const glm::dvec2	m_pos;
	const glm::dvec2	m_rollPitch;
	const double		m_altitude;

private:
};

