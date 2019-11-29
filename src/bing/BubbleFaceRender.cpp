
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
#include <unordered_set>
#include <tbb/concurrent_unordered_map.h>
#include <ctpl/ctpl.h>
#include <SDL_image.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>

using namespace std;
using namespace std::filesystem;
using namespace tbb;
using namespace glm;
using namespace ctpl;
using namespace curlUtil;
using namespace boostGeom;

namespace
{
	concurrent_unordered_map<string, size_t> a_bubbleTiles; // TODO also in Bubble!

	const auto a_faceKeys = vector<string>{"01","02","03","10","11","12"}; // TODO also in Bubble!

	thread_pool a_loaderQueue(1);

	GLuint requestBubbleTile(const string & bubbleQuadKey, const size_t face, const string & tileID)
	{
		const auto faceQuadKey = bubbleQuadKey + a_faceKeys[face] + tileID;

		static unordered_set<string> checkedBubbleTiles;

		if(checkedBubbleTiles.find(faceQuadKey) != checkedBubbleTiles.end())
		{
			return a_bubbleTiles[faceQuadKey];
		}

		checkedBubbleTiles.insert(faceQuadKey);

		const auto i = a_bubbleTiles.find(faceQuadKey);

		if(i != a_bubbleTiles.end()) { return i->second ;}

		const string tileCachePath = "./bubbles/face_" + faceQuadKey;

		a_loaderQueue.push([tileCachePath, faceQuadKey](int id)
		{
			//OpenGL::ensureSharedContext();

			if(fileExists(tileCachePath))
			{
				if(!file_size(tileCachePath.c_str())) { return ;}

				auto img = IMG_Load(tileCachePath.c_str());

				if (!img) { goto doDownload ;}

				OpenGL::ensureSharedContext();

				const auto ret = Textures::load(img);

				SDL_FreeSurface(img);

				a_bubbleTiles[faceQuadKey] = ret;

				return;
			}

			doDownload:

			//const string streetsideImagesApi = "https://t.ssl.ak.tiles.virtualearth.net/tiles/hs";
			// TODO With https curl gives 60 error code. Try to fix.
			const string streetsideImagesApi = "http://t.ssl.ak.tiles.virtualearth.net/tiles/hs";

			const string imgUrlSuffix = ".jpg?g=6338&n=z";

			const auto url = streetsideImagesApi + faceQuadKey + imgUrlSuffix;

			//dmess("url " << url);

			static unordered_set<string> reur;

			if(reur.find(url) != reur.end())
			{
				dmessError("Already seen! " << url);
			}

			reur.insert(url);

			download(url, [faceQuadKey, tileCachePath](BufferStruct * buf)
			{
				// TODO code dup here.
				//auto tileBuffer = shared_ptr<BufferStruct>(download(url));
				auto tileBuffer = shared_ptr<BufferStruct>(buf);

				if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
				{
					dmess("No data!");

					return;
				}

				auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

				if(!img)
				{
					dmess("Bad data!");

					return;
				}

				const auto bytesPerPixel = img->format->BytesPerPixel;

				if(bytesPerPixel < 3)
				{
					SDL_FreeSurface(img);

					// Must be the no data png image, mark as no data.
					//return markTileNoData(tile);

					dmess("No data!");

					return;
				}

				////////////////////////////
				FILE * fp = fopen(tileCachePath.c_str(), "wb");

				if(fp)
				{
					fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

					fclose(fp);
				}
				else { dmess("Warn could not write file: " << tileCachePath) ;}
				///////////////////////////

				Textures::s_queue.push([img, faceQuadKey](int ID)
				{
					const auto ret = Textures::load(img);

					a_bubbleTiles[faceQuadKey] = ret;

					SDL_FreeSurface(img);
				});
			});
		});

		return 0;
	}

	void doQuadBox(const string & prefix, const Box & b, vector<pair<string, Box> > & out)
	{
		const auto quad = quadBox(b);

		for(size_t i = 0; i < 4; ++i) { out.push_back(make_pair(prefix + to_string(i), quad[i])) ;}
	}

	void doQuadBox(const vector<pair<string, Box> > & in, vector<pair<string, Box> > & out)
	{
		for(const auto & [prefix, b] : in) { doQuadBox(prefix, b, out) ;}
	}

	unordered_map<string, Renderable *> a_tileGrid;

	const unordered_map<string, Renderable *> & ensureTileGrid()
	{
		if(a_tileGrid.size()) { return a_tileGrid ;}

		Box b = Box{{-1,-1},{1,1}};

		vector<pair<string, Box> > curr[2];

		curr[0].push_back({"", b});

		size_t i;

		//for(i = 0; i < 4; ++i)
		//for(i = 0; i < 3; ++i)
		for(i = 0; i < 2; ++i)
		//for(i = 0; i < 1; ++i)
		{
			auto & in = curr[i % 2];
			auto & out = curr[(i + 1) % 2];

			doQuadBox(in, out);

			in.clear();
		}

		for(const auto & [id, b] : curr[i % 2])
		{
			const auto min = dvec2(b.min_corner().x(), b.min_corner().y());
			const auto max = dvec2(b.max_corner().x(), b.max_corner().y());

			a_tileGrid[id] = Renderable::create(geosUtil::makeBox(min, max), mat4(1.0f), AABB2D(min.x, min.y, max.x, max.y), true)->
											setRenderFill	(true)->
											setRenderOutline(false)->
											setShader		(TextureShader::getDefaultInstance());
		}

		return a_tileGrid;
	}
}

GLuint BubbleFaceRender::renderBubbleFace(FrameBuffer * frameBuffer, const Bubble * bubble, const size_t face)
{
	return renderBubbleFace(frameBuffer, bubble->getQuadKey(), face);
}

#include <glm/gtc/matrix_transform.hpp>

GLuint BubbleFaceRender::renderBubbleFace(FrameBuffer * frameBuffer, const string & bubbleQuadKey, const size_t face)
{
	frameBuffer->bind();

	for(const auto [tileID, tileRenderable] : ensureTileGrid())
	{
		const auto tex = requestBubbleTile(bubbleQuadKey, face, tileID);

		if(!tex) { continue ;}

		//dmess("ID " << tileID << " tileRenderable " << tileRenderable << " " << tex);

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

	frameBuffer->unbind();

	return frameBuffer->getTextureID();
}

