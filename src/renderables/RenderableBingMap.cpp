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
#include <filesystem>
#include <SDL_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/renderables/RasterTile.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

using namespace std;
using namespace std::filesystem;
using namespace glm;
using namespace ctpl;
using namespace geosUtil;
using namespace bingTileSystem;
using namespace curlUtil;

atomic<size_t> RenderableBingMap::s_numLoading		= {0};
atomic<size_t> RenderableBingMap::s_numDownloading	= {0};
atomic<size_t> RenderableBingMap::s_numUploading	= {0};
atomic<size_t> RenderableBingMap::s_numWriting		= {0};

size_t RenderableBingMap::s_numRendered = 0;

FrameBuffer * RenderableBingMap::s_textureBuffer = NULL;

bool RenderableBingMap::s_useCache	= true;
const bool useBindlessTextures		= true;

namespace
{
#ifndef __EMSCRIPTEN__

    thread_pool loaderPool	(64);
	//thread_pool loaderPool	(1);
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

	BufferStruct * downloadTile(const int ID, const string & quadKey)
	{
		CURL * myHandle = NULL;

		{
			lock_guard<mutex> _(loaderMutex);

			if(curlHandles.find(ID) == curlHandles.end()) { curlHandles[ID] = myHandle = curl_easy_init() ;}

			myHandle = curlHandles[ID];

			if (!myHandle) { dmessError("Could not create CURL handle!") ;}
		}

		CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
		BufferStruct * output = new BufferStruct; // Create an instance of out BufferStruct to accept LCs output
		
		//if(!myHandle) { myHandle = curl_easy_init() ;}

		/* Notice the lack of major error checking, for brevity */

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, output); // Passing our BufferStruct to LC

		//const string url =  "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + quadKey + "?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";
		const string url =  "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + quadKey + "?mkt=en-GB&it=A";

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
		
		//return TileBuffer(output.m_buffer, output.m_size);

		return output;
	}
}

void RenderableBingMap::fetchTile(RasterTile * tile)
{
	if(tile->m_loading) { return ;}
	
	tile->m_loading = true;

	loaderPool.push([tile](int ID) { fetchTile(ID, tile) ;});

	++s_numLoading;
}

void RenderableBingMap::markTileNoData(RasterTile* tile)
{
	--s_numLoading;

	tile->m_loading = false;

	tile->m_textureID = RasterTile::s_NO_DATA;
}

void RenderableBingMap::fetchTile(const int ID, RasterTile * tile)
{
	if (!tile->m_stillNeeded)
	{
		tile->m_loading = false;

		--s_numLoading;

		return;
	}

	const string quadKey = tileToQuadKey(latLongToTile(tile->m_center, tile->m_level), tile->m_level);

	const string tileCachePath = "./tiles/" + quadKey + ".jpg";

	if(s_useCache && fileExists(tileCachePath))
	{
		++s_numUploading;

		if(!file_size(tileCachePath.c_str())) { return markTileNoData(tile) ;}

		SDL_Surface * img = IMG_Load(tileCachePath.c_str());

		if (!img) { goto download ;}

		uploaderPool.push([tile, img, tileCachePath](int ID)
		{
			if (!tile->m_stillNeeded)
			{
				tile->m_loading = false;

				--s_numUploading;

				--s_numLoading;

				SDL_FreeSurface(img);

				return;
			}

			OpenGL::ensureSharedContext();

			tile->m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

			--s_numLoading;

			--s_numUploading;

			tile->m_loading = false;
		});
	}
	else
	{
	download:

		auto tileBuffer = shared_ptr<BufferStruct>(downloadTile(ID, quadKey));

		if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
		{
			markTileNoData(tile);

			if (!s_useCache) { return ;}

			FILE * fp = fopen(tileCachePath.c_str(), "wb");

			if(fp)
			{
				fclose(fp);
			}
			else { dmess("Warn could not write file: " << tileCachePath) ;}

			return;
		}

		++s_numUploading;

		uploaderPool.push([tile, tileBuffer, tileCachePath](int ID)
		{
			if (!tile->m_stillNeeded)
			{
				tile->m_loading = false;
				
				--s_numUploading;

				--s_numLoading;

				return;
			}

			OpenGL::ensureSharedContext();

			SDL_Surface * img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

			if(!img) { return markTileNoData(tile) ;}

			const auto bytesPerPixel = img->format->BytesPerPixel;

			if(bytesPerPixel < 3)
			{
				SDL_FreeSurface(img);

				// Must be the no data png image, mark as no data.
				return markTileNoData(tile);
			}

			tile->m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

			--s_numUploading;

			--s_numLoading;

			tile->m_loading = false;

			if (!s_useCache) { return ;}

			++s_numWriting;

			writerPool.push([tileCachePath, tileBuffer](int ID)
			{
				FILE * fp = fopen(tileCachePath.c_str(), "wb");

				if(fp)
				{
					fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

					fclose(fp);
				}
				else { dmess("Warn could not write file: " << tileCachePath) ;}
			});

			--s_numWriting;
		});
	}
}

void RenderableBingMap::getStartLevel()
{
	for (int i = 0; i <= 24; ++i)
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
	
	if(!useBindlessTextures) { return ;}
	
	glGenBuffers(1, &buffers.textureHandleBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffers.textureHandleBuffer);

	glBufferStorage(GL_UNIFORM_BUFFER, NUM_TEXTURES * sizeof(GLuint64) * 2, nullptr, GL_MAP_WRITE_BIT);
}

RenderableBingMap::~RenderableBingMap()
{

}

float resDelta = 1.0;

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

	const double tileSize = 256.0 * resDelta;

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
	m_tiles.insert(RasterTile::getTile(center, level, canvas->getFrameNumber()));
	m_tiles.insert(RasterTile::getTile(tMin + dvec2(0.001, 0.001), level, canvas->getFrameNumber())); // Should not need to do this. Rounding errors?

	return true;
}

namespace
{
	vector<RasterTile*> a_lastTilesRendered;

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

		for (size_t i = 0; i < toRender.size(); ++i)
		{
			RasterTile* t = toRender[i];

			if (useBindlessTextures)
            {
                t->m_renderable->setShader(BindlessTextureShader::getDefaultInstance());

                BindlessTextureShader::getDefaultInstance()->setTextureSlot(i);
            }
			else { t->m_renderable->setShader(TextureShader::getDefaultInstance()); }

			t->m_renderable->render(canvas);
		}
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

	const auto fbSize = canvas->getFrameBufferSize();

	s_textureBuffer->setBufferSize(fbSize);

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
		if(tileSet.find(i) == tileSet.end()) { i->m_stillNeeded = false ;}
	}

	unordered_set<RasterTile*> fallBackTiles;

	for (const auto tile : m_tiles)
	{
		if (tile->textureReady()) { continue; }

		RasterTile* currTile = tile;

		for (int parentLevel = int(currTile->m_level) - 1; parentLevel >= m_startLevel; --parentLevel)
		{
			currTile = currTile->getParentTile(canvas->getFrameNumber());

			const size_t textureID = currTile->m_textureID;

			if(textureID == RasterTile::s_NO_DATA) { continue ;}

			if (!textureID)
			{
				fetchTile(currTile);

				continue;
			}
			
			fallBackTiles.insert(currTile);

			break;
		}
	}

	for (const auto tile : fallBackTiles) { m_tiles.insert(tile) ;}

	vector<RasterTile*> tiles = toVec(m_tiles);

	sort(tiles.begin(), tiles.end(), [](const RasterTile * A, const RasterTile * B) { return A->m_level < B->m_level ;});

	size_t numRendered = 0;

	vector<RasterTile*> toRender;

	for (auto tile : tiles)
	{
		if(tile->m_loading) { continue ;}
		
		if (tile->textureReady())
		{
			if(!tile->m_renderable)
			{
				const ivec2 iTile = latLongToTile(tile->m_center, tile->m_level);

				const dvec2 tMin = tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), tile->m_level);
				const dvec2 tMax = tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), tile->m_level);
				
				tile->m_renderable = Renderable::create(makeBox(tMin, tMax), m_trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

				tile->m_renderable->ensureVAO();

				if(useBindlessTextures) { tile->m_renderable->setShader(BindlessTextureShader	::getDefaultInstance()) ;}
				else					{ tile->m_renderable->setShader(TextureShader			::getDefaultInstance()) ;}

				tile->m_renderable->setRenderOutline (false);
				tile->m_renderable->setRenderFill    (true);
			}

			toRender.push_back(tile);

			++numRendered;
		}
		else { fetchTile(tile) ;}
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

	//if(!(canvas->getFrameNumber() % 50)) { RasterTile::pruneTiles() ;}
	//if(!(canvas->getFrameNumber() % 5)) { RasterTile::pruneTiles() ;}

	RasterTile::pruneTiles();
}

FrameBuffer* RenderableBingMap::getFrameBuffer() { return s_textureBuffer ;}
