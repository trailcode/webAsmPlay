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

#ifndef __EMSCRIPTEN__
    #include <curl/curl.h>
    #include <ctpl.h>
#endif

#include <algorithm>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <SDL_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/RasterTile.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

using namespace std;
using namespace glm;
using namespace ctpl;
using namespace geosUtil;
using namespace bingTileSystem;

atomic<size_t> RenderableBingMap::s_numTiles		= {0};
atomic<size_t> RenderableBingMap::s_numLoading		= {0};
atomic<size_t> RenderableBingMap::s_numDownloading	= {0};
atomic<size_t> RenderableBingMap::s_numUploading	= {0};
atomic<size_t> RenderableBingMap::s_numWriting		= {0};

size_t RenderableBingMap::s_numRendered = 0;

FrameBuffer * RenderableBingMap::s_textureBuffer = NULL;

bool RenderableBingMap::s_useCache = true;

namespace
{
#ifndef __EMSCRIPTEN__

    thread_pool loaderPool	(16);
    thread_pool uploaderPool(1);
	thread_pool writerPool	(1);

    mutex loaderMutex;
    mutex uploaderMutex;

    unordered_map<int, CURL *> curlHandles;

#endif

	enum
	{
		//NUM_TEXTURES  = 2048,
		NUM_TEXTURES  = 4096,
	};

	struct
	{
		//GLuint      transformBuffer;
		GLuint      textureHandleBuffer;
	} buffers;

	//GLuint64 * pHandles = NULL;

    // Define our struct for accepting LCs output
    struct BufferStruct // TODO code duplication
    {
        char * buffer;
        size_t size;
    };

    // This is the function we pass to LC, which writes the output to a BufferStruct
    static size_t writeMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) // TODO code duplication
    {
        size_t realsize = size * nmemb;

        struct BufferStruct * mem = (struct BufferStruct *) data;

        mem->buffer = (char *)realloc(mem->buffer, mem->size + realsize + 1);

        if ( mem->buffer )
        {
            memcpy( &( mem->buffer[ mem->size ] ), ptr, realsize );
            mem->size += realsize;
            mem->buffer[ mem->size ] = 0;
        }
        
        return realsize;
    }

	typedef pair<const char *, const size_t> TileBuffer;

	TileBuffer downloadTile(const int ID, const string & quadKey)
	{
		CURL * myHandle = NULL;

		{
			lock_guard<mutex> _(loaderMutex);

			if(curlHandles.find(ID) == curlHandles.end()) { curlHandles[ID] = myHandle = curl_easy_init() ;}

			myHandle = curlHandles[ID];

			if (!myHandle) { dmessError("Could not create CURL handle!") ;}
		}

		CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
		struct BufferStruct * output = new BufferStruct; // Create an instance of out BufferStruct to accept LCs output
		output->buffer = NULL;
		output->size = 0;

		//if(!myHandle) { myHandle = curl_easy_init() ;}

		/* Notice the lack of major error checking, for brevity */

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, (void *)output); // Passing our BufferStruct to LC

		const string url =  "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + quadKey + "?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";

		//dmess("url " << url);

		// http://ecn.{subdomain}.tiles.virtualearth.net/tiles/hs0203232101212100{faceId}{tileId}?g=6617&key={BingMapsKey}
															   //0231010301213112
															   //0231010301210232
															   //0201012211200132
		
		//dmess("url " << url);

		curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());

		if (result = curl_easy_perform(myHandle))
		{
			dmess("result " << result << " myHandle " << myHandle);
		}
		//curl_easy_cleanup( myHandle );

		const char * ret = output->buffer;

		const size_t size = output->size;

		if (!ret)
		{
			//dmess("Error!");
		} // It will retry later

		delete output;

		return TileBuffer(ret, size);
	}

	const bool useBindlessTextures = false;

	void fetchTile(const int ID, RasterTile * tile)
	{
		if (!tile->m_stillNeeded)
		{
			//dmess("Skip here!");

			--RenderableBingMap::s_numLoading;

			return;
		}

		const string quadKey = tileToQuadKey(latLongToTile(tile->m_center, tile->m_level), tile->m_level);

		const string tileCachePath = "./tiles/" + quadKey + ".jpg";

		if(fileExists(tileCachePath))
		{
			++RenderableBingMap::s_numUploading;

			SDL_Surface * img = IMG_Load(tileCachePath.c_str());

			if (!img) { goto download; }

			uploaderPool.push([tile, img](int ID)
			{
				if (!tile->m_stillNeeded)
				{
					--RenderableBingMap::s_numUploading;

					--RenderableBingMap::s_numLoading;

					SDL_FreeSurface(img);
					 
					return;
				}

				OpenGL::ensureSharedContext();

				tile->m_textureID = Textures::load(img);

				SDL_FreeSurface(img);

				if(useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

				++RenderableBingMap::s_numTiles;

				--RenderableBingMap::s_numLoading;

				--RenderableBingMap::s_numUploading;
			});
		}
		else
		{
		download:

			TileBuffer tileBuffer = downloadTile(ID, quadKey);

			if (!get<0>(tileBuffer))
			{
				//dmess("Error! No buffer!");

				--RenderableBingMap::s_numLoading;

				return;
			}

			++RenderableBingMap::s_numUploading;

			uploaderPool.push([tile, tileBuffer, tileCachePath](int ID)
			{
				if (!tile->m_stillNeeded)
				{
					//dmess("Skip here!");

					--RenderableBingMap::s_numUploading;

					--RenderableBingMap::s_numLoading;

					return;
				}

				OpenGL::ensureSharedContext();

				tile->m_textureID = Textures::createFromJpeg(get<0>(tileBuffer), get<1>(tileBuffer));

				if(useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

				++RenderableBingMap::s_numTiles;

				--RenderableBingMap::s_numUploading;

				--RenderableBingMap::s_numLoading;

				++RenderableBingMap::s_numWriting;

				writerPool.push([tileCachePath, tileBuffer](int ID)
				{
					FILE * fp = fopen(tileCachePath.c_str(), "wb");

					if(fp)
					{
						fwrite(get<0>(tileBuffer), sizeof(char), get<1>(tileBuffer), fp);

						fclose(fp);
					}
					else
					{
						dmess("Warn could not write file: " << tileCachePath);
					}
				
					if( get<0>(tileBuffer) )
					{
						free ( (void *)get<0>(tileBuffer) );
					}
				});

				--RenderableBingMap::s_numWriting;
			});
		}
	}
}

void RenderableBingMap::getStartLevel()
{
	for (int i = 0; i < 24; ++i)
	{
		const ivec2 minTile = latLongToTile(dvec2(get<0>(m_bounds), get<1>(m_bounds)), i);
		const ivec2 maxTile = latLongToTile(dvec2(get<2>(m_bounds), get<3>(m_bounds)), i);

		if(minTile == maxTile) { continue ;}

		m_startLevel = i - 1;

		break;
	}
}

Renderable * RenderableBingMap::create(const AABB2D & bounds, const dmat4 & trans) { return new RenderableBingMap(bounds, trans) ;}

RenderableBingMap::RenderableBingMap(const AABB2D & bounds, const dmat4 & trans) : m_bounds(bounds), m_trans(trans)
{
	getStartLevel();
	
	if (useBindlessTextures)
	{
		glGenBuffers(1, &buffers.textureHandleBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers.textureHandleBuffer);

		glBufferStorage(GL_UNIFORM_BUFFER, NUM_TEXTURES * sizeof(GLuint64) * 2, nullptr, GL_MAP_WRITE_BIT);
	}
}

RenderableBingMap::~RenderableBingMap()
{

}

bool RenderableBingMap::getTilesToRender(Canvas * canvas, const dvec2 & tMin, const dvec2 & tMax, const size_t level)
{
	const dvec3 center = dvec4(((tMin + tMax) * 0.5), 0.0, 1.0);

	const dvec3 P1 = dvec4(tMin.x, tMax.y, 0, 1);
	const dvec3 P2 = dvec4(tMax.x, tMax.y, 0, 1);
	const dvec3 P3 = dvec4(tMax.x, tMin.y, 0, 1);
	const dvec3 P4 = dvec4(tMin.x, tMin.y, 0, 1);

	const dvec3 transCenter = m_trans * dvec4(center, 1.0);

	const dvec3 transP1 = m_trans * dvec4(P1, 1);
	const dvec3 transP2 = m_trans * dvec4(P2, 1);
	const dvec3 transP3 = m_trans * dvec4(P3, 1);
	const dvec3 transP4 = m_trans * dvec4(P4, 1);

	const ivec2 fbSize = canvas->getFrameBufferSize();

	const dvec4 viewport(0.0, 0.0, fbSize.x, fbSize.y);

	const dvec3 screenP1 = project(transP1, canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);
	const dvec3 screenP2 = project(transP2, canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);
	const dvec3 screenP3 = project(transP3, canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);
	const dvec3 screenP4 = project(transP4, canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);

	const double tileSize = 256.0;

	const double D1 = distance(screenP1, screenP2);
	const double D2 = distance(screenP2, screenP3);
	const double D3 = distance(screenP3, screenP4);
	const double D4 = distance(screenP4, screenP1);

	const Frustum * frust = canvas->getCameraFrustum();

	//if (level < 23 && (D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	if (level < 24 && (D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	//if (level < 12 && (D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	{
		const dvec3 subPoints[] = {	/* 0 */dvec3(tMin.x, tMax.y,	0),		/* 1 */dvec3(center.x, tMax.y,   0), /* 2 */dvec3(tMax.x, tMax.y,   0),
									/* 3 */dvec3(tMin.x, center.y,  0),		/* 4 */dvec3(center.x, center.y, 0), /* 5 */dvec3(tMax.x, center.y, 0),
									/* 6 */dvec3(tMin.x, tMin.y,	0),		/* 7 */dvec3(center.x, tMin.y,   0), /* 8 */dvec3(tMax.x, tMin.y,   0)};

		const dvec3 subPointsTrans[] = {	/* 0 */m_trans * dvec4(tMin.x, tMax.y,   0, 1), /* 1 */m_trans * dvec4(center.x, tMax.y,   0, 1), /* 2 */m_trans * dvec4(tMax.x, tMax.y,   0, 1),
											/* 3 */m_trans * dvec4(tMin.x, center.y, 0, 1), /* 4 */m_trans * dvec4(center.x, center.y, 0, 1), /* 5 */m_trans * dvec4(tMax.x, center.y, 0, 1),
											/* 6 */m_trans * dvec4(tMin.x, tMin.y,   0, 1), /* 7 */m_trans * dvec4(center.x, tMin.y,   0, 1), /* 8 */m_trans * dvec4(tMax.x, tMin.y,   0, 1)};

		
		bool gotTile = false;

		if(frust->intersects(subPointsTrans[0], subPointsTrans[1], subPointsTrans[4], subPointsTrans[3])) { gotTile |= getTilesToRender(canvas, subPoints[3], subPoints[1], level + 1) ;}
		if(frust->intersects(subPointsTrans[1], subPointsTrans[2], subPointsTrans[5], subPointsTrans[4])) { gotTile |= getTilesToRender(canvas, subPoints[4], subPoints[2], level + 1) ;}
		if(frust->intersects(subPointsTrans[3], subPointsTrans[4], subPointsTrans[7], subPointsTrans[6])) { gotTile |= getTilesToRender(canvas, subPoints[6], subPoints[4], level + 1) ;}
		if(frust->intersects(subPointsTrans[4], subPointsTrans[5], subPointsTrans[8], subPointsTrans[7])) { gotTile |= getTilesToRender(canvas, subPoints[7], subPoints[5], level + 1) ;}
		// TODO try projecting into a different space and doing a simpler check.

		//if (gotTile) { return true; }

		return gotTile;

		//dmess("Here!"); // This Should not be happening
	}

	//RasterTile * tile = RasterTile::getTile(center, level);
	//RasterTile * tile = RasterTile::getTile(tMin + dvec2(0.001, 0.001), level);

	//m_tiles.push_back(tile);
	m_tiles.insert(RasterTile::getTile(center, level));
	m_tiles.insert(RasterTile::getTile(tMin + dvec2(0.001, 0.001), level)); // Should not need to do this. Rounding errors?

	return true;
}

#include <webAsmPlay/shaders/ColorShader.h>

namespace
{
	void renderBindlessTextures(Canvas* canvas, const vector<RasterTile*>& toRender)
	{
		if (!toRender.size()) { return; }

		glBindBufferBase(GL_UNIFORM_BUFFER, 6, buffers.textureHandleBuffer);

		GLuint64* pHandles = (GLuint64*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, toRender.size() * sizeof(GLuint64) * 2, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for (size_t i = 0; i < toRender.size(); ++i)
		{
			RasterTile* t = toRender[i];

			if (!t->m_textureResident)
			{
				glMakeTextureHandleResidentARB(t->m_handle);

				t->m_textureResident = true;
			}

			pHandles[i * 2] = t->m_handle;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		for (size_t i = 0; i < toRender.size(); ++i)
		{
			RasterTile* t = toRender[i];

			if (useBindlessTextures)
            {
                t->m_renderable->setShader(BindlessTextureShader::getDefaultInstance());

                BindlessTextureShader::getDefaultInstance()->setTextureSlot(i);
            }
			else { t->m_renderable->setShader(TextureShader::getDefaultInstance()); }

			//t->m_renderable->setRenderOutline(true);

			t->m_renderable->render(canvas);
		}

		//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
}

void RenderableBingMap::render(Canvas * canvas, const size_t renderStage)
{
    if(!getRenderFill()) { return ;}

	if (!s_textureBuffer) // TODO refactor
	{
		s_textureBuffer = new FrameBuffer(canvas->getFrameBufferSize(),
											{ FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)})});
	}

	s_textureBuffer->setBufferSize(canvas->getFrameBufferSize());

	s_textureBuffer->bind();
	
	const ivec2 minTile = latLongToTile(dvec2(get<0>(m_bounds), get<1>(m_bounds)), m_startLevel);

	const dvec2 tMin = tileToLatLong(ivec2(minTile.x + 0, minTile.y + 0), m_startLevel);
	const dvec2 tMax = tileToLatLong(ivec2(minTile.x + 1, minTile.y + 1), m_startLevel);

	unordered_set<RasterTile *> prevTiles = m_tiles;

	m_tiles.clear();

	getTilesToRender(canvas, tMin, tMax, m_startLevel);

	unordered_set<RasterTile*> tileSet = m_tiles;

	for (auto i : m_tiles) { i->m_stillNeeded = true ;}

	for (auto i : prevTiles)
	{
		if(tileSet.find(i) == tileSet.end())
		{
			i->m_loading = false;

			i->m_stillNeeded = false;
		}
	}

	unordered_set<RasterTile*> fallBackTiles;

	for (const auto tile : m_tiles)
	{
		if (tile->m_textureID) { continue; }

		RasterTile* currTile = tile;

		for (int parentLevel = int(currTile->m_level) - 1; parentLevel >= m_startLevel; --parentLevel)
		{
			currTile = currTile->getParentTile();

			if (!currTile->m_textureID) { continue; }
			
			fallBackTiles.insert(currTile);

			break;
		}
	}

	//if(fallBackTiles.size()) dmess("fallBackTiles " << fallBackTiles.size());

	for (const auto tile : fallBackTiles)
	{
		if (tileSet.find(tile) == tileSet.end()) { m_tiles.insert(tile); }
	}

	vector<RasterTile*> tiles = toVec(m_tiles);

	sort(tiles.begin(), tiles.end(), [](const RasterTile * A, const RasterTile * B) { return A->m_level < B->m_level ;});

	size_t numRendered = 0;

	vector<RasterTile*> toRender;

	for (auto i : tiles)
	{
		if (i->m_textureID)
		{
			if(!i->m_renderable)
			{
				const ivec2 iTile = latLongToTile(i->m_center, i->m_level);

				const dvec2 tMin = tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), i->m_level);
				const dvec2 tMax = tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), i->m_level);
				
				i->m_renderable = Renderable::create(makeBox(tMin, tMax), m_trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

				i->m_renderable->ensureVAO();

				if(useBindlessTextures) { i->m_renderable->setShader(BindlessTextureShader	::getDefaultInstance()) ;}
				else					{ i->m_renderable->setShader(TextureShader			::getDefaultInstance()) ;}

				i->m_renderable->setRenderOutline (false);
				i->m_renderable->setRenderFill    (true);
			}

			toRender.push_back(i);

			++numRendered;
		}
		else if (!i->m_loading)
		{
			i->m_loading = true;

			loaderPool.push([i](int ID) { fetchTile(ID, i) ;});

			++s_numLoading;
		}
	}

	if(useBindlessTextures) { renderBindlessTextures(canvas, toRender) ;}
	else
	{
        for(auto tile : toRender)
        {
            TextureShader::getDefaultInstance()->setTextureID(tile->m_textureID);
            
            tile->m_renderable->render(canvas);
        }
	}

	RenderableBingMap::s_numRendered = toRender.size();

	s_textureBuffer->unbind();
}

FrameBuffer* RenderableBingMap::getFrameBuffer() { return s_textureBuffer ;}
