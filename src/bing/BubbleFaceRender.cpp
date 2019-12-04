
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

#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleTile.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>

using namespace std;
using namespace std::filesystem;
using namespace glm;
using namespace boostGeom;

namespace
{
	void doQuadBoxs(const vector<pair<string, Box> > & in, vector<pair<string, Box> > & out)
	{
		for(const auto & [prefix, b] : in)
		{
			const auto quad = quadBox(b);

			for(size_t i = 0; i < 4; ++i) { out.push_back(make_pair(prefix + to_string(i), quad[i])) ;}
		}
	}

	unordered_map<string, Renderable *> a_tileGrid;

	vector<string> a_tileIDs;

	const unordered_map<string, Renderable *> & ensureTileGrid()
	{
		if(a_tileGrid.size()) { return a_tileGrid ;}

		Box b = Box{{-1,-1},{1,1}};

		vector<pair<string, Box> > curr[2];

		curr[0].push_back({"", b});

		size_t i;

		//for(i = 0; i < 4; ++i)
		//for(i = 0; i < 3; ++i)
		//for(i = 0; i < 2; ++i)
		for(i = 0; i < 1; ++i)
		{
			doQuadBoxs(curr[i % 2], curr[(i + 1) % 2]);

			curr[i % 2].clear();
		}

		for(const auto & [id, b] : curr[i % 2])
		{
			const auto min = dvec2(b.min_corner().x(), b.min_corner().y());
			const auto max = dvec2(b.max_corner().x(), b.max_corner().y());

			a_tileGrid[id] = Renderable::create(geosUtil::makeBox(min, max), mat4(1.0f), AABB2D(min.x, min.y, max.x, max.y), true)->
											setRenderFill	(true)->
											setRenderOutline(false)->
											setShader		(TextureShader::getDefaultInstance());

			a_tileIDs.push_back(id);
		}

		return a_tileGrid;
	}
}

GLuint BubbleFaceRender::renderBubbleFace(FrameBuffer * frameBuffer, const Bubble * bubble, const size_t face)
{
	if(!bubble) { return 0 ;}

	return renderBubbleFace(frameBuffer, bubble->getQuadKey(), face);
}

GLuint BubbleFaceRender::renderBubbleFace(FrameBuffer * frameBuffer, const string & bubbleQuadKey, const size_t face)
{
	frameBuffer->bind();

	/*
	for(const auto [tileID, tileRenderable] : ensureTileGrid())
	{
		const auto tex = BubbleTile::requestBubbleTile(bubbleQuadKey, face, tileID);

		if(!tex) { continue ;}

		auto r = (RenderablePolygon *)tileRenderable;

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		r->ensureVAO();

		r->m_vertexArrayObject->bind(r->m_shader);

		glm::mat4 projection = glm::ortho(
			-1.f, 1.f, 1.f, -1.f, -10.0f, 10.0f
        );

        glm::mat4 view = lookAt(vec3(0,0,1),vec3(0,0,0),vec3(0,1,0));

        glm::mat4 model = mat4(1.0);

		auto MVP = model * view * projection;

		TextureShader::getDefaultInstance()->setTextureID(tex);

		TextureShader::getDefaultInstance()->bind(MVP);

		r->m_vertexArrayObject->bindTriangles();

		r->m_vertexArrayObject->drawTriangles();
	}
	*/

	frameBuffer->unbind();

	return frameBuffer->getTextureID();
}

const vector<string> & BubbleFaceRender::getTileIDs()
{
	ensureTileGrid();

	return a_tileIDs;
}