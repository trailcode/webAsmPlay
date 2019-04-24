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

#include <atomic>
#include <webAsmPlay/renderables/Renderable.h>

class Frustum;

class RasterTile;
class FrameBuffer;

class RenderableBingMap : public Renderable
{
public:

    static Renderable * create(const AABB2D & bounds, const glm::dmat4 & trans = glm::dmat4(1.0));

    void render(Canvas * canvas, const size_t renderStage = 0) override;

	static std::atomic<size_t> s_numTiles;
	static std::atomic<size_t> s_numLoading;
	static std::atomic<size_t> s_numDownloading;
	static std::atomic<size_t> s_numUploading;
	static std::atomic<size_t> s_numWriting;

	static size_t s_numRendered;

	static FrameBuffer* getFrameBuffer();

private:

    RenderableBingMap(const AABB2D & bounds, const glm::dmat4 & trans);
    ~RenderableBingMap();

	void getStartLevel();

	void getTilesToRender(Canvas * canvas, const glm::dvec2 & tMin, const glm::dvec2 & tMax, const size_t level);

    const AABB2D m_bounds;

	size_t m_startLevel = 0;

    glm::ivec2 m_minTile;
    glm::ivec2 m_maxTile;

	const glm::dmat4 m_trans;

	std::vector<RasterTile*> m_tiles;

	static FrameBuffer* s_textureBuffer;
};
