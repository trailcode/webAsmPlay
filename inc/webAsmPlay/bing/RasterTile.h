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

#pragma once

#include <atomic>
#include <mutex>
#include <glm/vec2.hpp>
#include <webAsmPlay/Texture.h>

class Renderable;

class RasterTile : public Texture
{
public:

	RasterTile(const std::string & ID, const glm::dvec2& center, const glm::dvec2& widthHeight, const size_t level);

	~RasterTile();

	static RasterTile* getTile(const glm::dvec2& center, const size_t level, const size_t accessTime);

	std::string getDownloadURL() const override;

	//static size_t pruneTiles();

	//static size_t getNumTiles();

	RasterTile* getParentTile(const size_t accessTime) const;

	//bool textureReady() const;

	const glm::dvec2	m_center;
	const glm::dvec2	m_widthHeight;
	const size_t		m_level;

	//std::atomic_bool m_loading = { false };

	//std::atomic_bool m_stillNeeded = { true };

	Renderable* m_renderable = nullptr;

	//static GLuint s_NO_DATA;

	//std::atomic<GLuint> m_textureID = { 0 };

	//GLuint64 m_handle = 0;

	//bool m_textureResident = false;

	//size_t m_lastAccessTime = 0;

	//static std::atomic_size_t s_desiredMaxNumTiles;
};