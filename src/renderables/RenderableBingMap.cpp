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
#include <webAsmPlay/bing/BingTileSystem.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>
#include <webAsmPlay/shaders/TileBoundaryShader.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/bing/RasterTile.h>
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

FrameBuffer * RenderableBingMap::s_textureBuffer = nullptr;

bool RenderableBingMap::s_useCache				= true;
bool RenderableBingMap::s_useBindlessTextures	= false; // TODO Appears to be broken!

namespace
{
#ifndef __EMSCRIPTEN__

    thread_pool a_loaderPool(1);
	thread_pool a_writerPool(1);

	class MyCleanup
	{
	public:

		~MyCleanup()
		{
			a_loaderPool.stop();
			a_writerPool.stop();
		}
	};

	static MyCleanup s_cleanup;

#endif

	enum
	{
		NUM_TEXTURES  = 2048,
		//NUM_TEXTURES  = 4096,
	};

	struct
	{
		GLuint m_textureHandleBuffer;

	} buffers;
}

void RenderableBingMap::fetchTile(RasterTile * tile)
{
	tile->m_stillNeeded = true;

	tile->m_loading = true;

	++s_numLoading;

	a_loaderPool.push([tile](int ID) { fetchTile(ID, tile) ;});
}

void RenderableBingMap::markTileNoData(RasterTile* tile)
{
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
		--s_numLoading;

		if(!file_size(tileCachePath.c_str())) { return markTileNoData(tile) ;}

		auto img = IMG_Load(tileCachePath.c_str());

		if (!img) { goto download ;}

		++s_numUploading;

		a_loaderPool.push([tile, img, tileCachePath](int ID)
		{
			--s_numUploading;

			if (!tile->m_stillNeeded)
			{
				tile->m_loading = false;

				SDL_FreeSurface(img);

				return;
			}

			OpenGL::ensureSharedContext();

			tile->m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(s_useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

			tile->m_loading = false;
		});

		return;
	}
	
	download:

	const auto url = "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + quadKey + "?mkt=en-GB&it=A"; 

	download(url, 
		[tile]() -> bool
		{
			--s_numLoading;

			if(!tile->m_stillNeeded) { tile->m_loading = false ;}

			return tile->m_stillNeeded;
		},
		[tile, tileCachePath, url](BufferStruct * tileBuffer)
		{
			if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
			{
				markTileNoData(tile);

				if (!s_useCache) { return ;}

				FILE * fp = fopen(tileCachePath.c_str(), "wb");

				if(fp) { fclose(fp) ;}

				else { dmess("Warn could not write file: " << tileCachePath) ;}

				return;
			}

			++s_numUploading;

			a_loaderPool.push([tile, tileBuffer, tileCachePath, url](int ID)
			{
				--s_numUploading;

				if (!tile->m_stillNeeded)
				{
					tile->m_loading = false;
				
					return;
				}

				auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

				if(!img) { return markTileNoData(tile) ;}

				const auto bytesPerPixel = img->format->BytesPerPixel;

				if(bytesPerPixel < 3)
				{
					SDL_FreeSurface(img);

					// Must be the no data png image, mark as no data.
					return markTileNoData(tile);
				}

				OpenGL::ensureSharedContext();

				tile->m_textureID = Textures::load(img);

				SDL_FreeSurface(img);

				if(s_useBindlessTextures) { tile->m_handle = glGetTextureHandleARB(tile->m_textureID) ;}

				tile->m_loading = false;

				if (!s_useCache) { return ;}

				++s_numWriting;

				a_writerPool.push([tileCachePath, tileBuffer](int ID)
				{
					FILE * fp = fopen(tileCachePath.c_str(), "wb");

					if(fp)
					{
						fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

						fclose(fp);
					}
					else { dmess("Warn could not write file: " << tileCachePath) ;}

					--s_numWriting;
				});
			});
		});
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
	
	if(!s_useBindlessTextures) { return ;}
	
	glGenBuffers(1,					&buffers.m_textureHandleBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER,  buffers.m_textureHandleBuffer);

	glBufferStorage(GL_UNIFORM_BUFFER, NUM_TEXTURES * sizeof(GLuint64) * 2, nullptr, GL_MAP_WRITE_BIT);
}

RenderableBingMap::~RenderableBingMap()
{

}

float resDelta = 1.0;

bool RenderableBingMap::getTilesToRender(Canvas * canvas, const dvec2 & min, const dvec2 & max, const size_t level)
{
	const dvec2 center = (min + max) * 0.5;

	auto tile = RasterTile::getTile(center, level, canvas->getFrameNumber());

	if(level >= 24)
	{
		tile->m_textureID = RasterTile::s_NO_DATA;

		return false;
	}

	if(tile->m_textureID == RasterTile::s_NO_DATA)
	{
		return false;
	}

	const ivec2 iTile = latLongToTile(center, level);

	const dvec2 tMin = tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), level);
	const dvec2 tMax = tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), level);

	const dvec3 P1 = dvec4(tMin.x, tMax.y, 0, 1);
	const dvec3 P2 = dvec4(tMax.x, tMax.y, 0, 1);
	const dvec3 P3 = dvec4(tMax.x, tMin.y, 0, 1);
	const dvec3 P4 = dvec4(tMin.x, tMin.y, 0, 1);

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

	const double D1 = distance(screenP1, screenP2);
	const double D2 = distance(screenP2, screenP3);
	const double D3 = distance(screenP3, screenP4);
	const double D4 = distance(screenP4, screenP1);

	const double tileSize = 256.0 * resDelta;

	if ((D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	{
		const dvec3 subPoints[] = {	/* 0 */dvec3(tMin.x, tMax.y,	0),		/* 1 */dvec3(center.x, tMax.y,   0), /* 2 */dvec3(tMax.x, tMax.y,   0),
									/* 3 */dvec3(tMin.x, center.y,  0),		/* 4 */dvec3(center.x, center.y, 0), /* 5 */dvec3(tMax.x, center.y, 0),
									/* 6 */dvec3(tMin.x, tMin.y,	0),		/* 7 */dvec3(center.x, tMin.y,   0), /* 8 */dvec3(tMax.x, tMin.y,   0)};

		const dvec3 subPointsTrans[] = {	/* 0 */m_trans * dvec4(tMin.x, tMax.y,   0, 1), /* 1 */m_trans * dvec4(center.x, tMax.y,   0, 1), /* 2 */m_trans * dvec4(tMax.x, tMax.y,   0, 1),
											/* 3 */m_trans * dvec4(tMin.x, center.y, 0, 1), /* 4 */m_trans * dvec4(center.x, center.y, 0, 1), /* 5 */m_trans * dvec4(tMax.x, center.y, 0, 1),
											/* 6 */m_trans * dvec4(tMin.x, tMin.y,   0, 1), /* 7 */m_trans * dvec4(center.x, tMin.y,   0, 1), /* 8 */m_trans * dvec4(tMax.x, tMin.y,   0, 1)};

		const auto frust = canvas->getCameraFrustum();
		
		size_t numSubTiles		= 0;
		size_t numIntersecting	= 0;

		if(frust->intersects(subPointsTrans[0], subPointsTrans[1], subPointsTrans[4], subPointsTrans[3])) { ++numIntersecting; numSubTiles += getTilesToRender(canvas, subPoints[3], subPoints[1], level + 1) ;}
		if(frust->intersects(subPointsTrans[1], subPointsTrans[2], subPointsTrans[5], subPointsTrans[4])) { ++numIntersecting; numSubTiles += getTilesToRender(canvas, subPoints[4], subPoints[2], level + 1) ;}
		if(frust->intersects(subPointsTrans[3], subPointsTrans[4], subPointsTrans[7], subPointsTrans[6])) { ++numIntersecting; numSubTiles += getTilesToRender(canvas, subPoints[6], subPoints[4], level + 1) ;}
		if(frust->intersects(subPointsTrans[4], subPointsTrans[5], subPointsTrans[8], subPointsTrans[7])) { ++numIntersecting; numSubTiles += getTilesToRender(canvas, subPoints[7], subPoints[5], level + 1) ;}

		if(numSubTiles == numIntersecting) { return true ;}
	}
	
	m_tiles.insert(tile);

	return true;
}

namespace
{
	vector<RasterTile*> a_lastTilesRendered;

	BindlessTextureShader a_bindlessShader();
}

//#include <webAsmPlay/shaders/ColorVertexShader.h>

void RenderableBingMap::renderBindlessTextures(Canvas* canvas, const vector<RasterTile*>& toRender, const size_t renderStage)
{
	if (!toRender.size()) { return; }

	glBindBufferBase(GL_UNIFORM_BUFFER, 6, buffers.m_textureHandleBuffer);

	GLuint64* pHandles = (GLuint64*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, toRender.size() * sizeof(GLuint64) * 2, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (size_t i = 0; i < toRender.size(); ++i)
	{
		auto tile = toRender[i];

		if (!tile->m_textureResident)
		{
			glMakeTextureHandleResidentARB(tile->m_handle);

			tile->m_textureResident = true;
		}

		pHandles[i * 2] = tile->m_handle;
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);

	for (size_t i = 0; i < toRender.size(); ++i)
	{
		auto tile = toRender[i];

        tile->m_renderable->setShader(BindlessTextureShader::getDefaultInstance());

        //BindlessTextureShader::getDefaultInstance()->setTextureSlot(1);
		BindlessTextureShader::getDefaultInstance()->setTextureSlot(i);
        
		//tile->m_renderable->setShader(ColorVertexShader::getDefaultInstance());

		//glDisable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);

		tile->m_renderable->render(canvas, renderStage);
		//tile->m_renderable->render(canvas, 0);
	}
	
	//return;

	vector<vec4> centers;

	for (size_t i = 0; i < toRender.size(); ++i)
	{
		auto tile = toRender[i];

		//tile->m_renderable->setShader(TileBoundaryShader::getDefaultInstance());

		//tile->m_renderable->render(canvas, renderStage);

		const ivec2 iTile = latLongToTile(tile->m_center, tile->m_level);

		const dvec2 tMin = m_trans * vec4(tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), tile->m_level), 0, 1);
		const dvec2 tMax = m_trans * vec4(tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), tile->m_level), 0, 1);

		const auto width = tMax.x - tMin.x;
		const auto height = tMax.y - tMin.y;

		if(width != height)
		{

			//dmess("DIff! " << width << " " << height);
		}

		//auto nCenter = (tile->m_max + tile->m_min) * 0.5;

		const auto center = m_trans * vec4(tile->m_center, 0, 1);
		//const auto center = m_trans * vec4(nCenter, 0, 1);
		const auto widthHeight = m_trans * vec4(tile->m_widthHeight, 0, 1);

		centers.push_back(vec4(center.x, center.y, width, height));

		//tile->
	}

	//dmess("centers " << centers.size());

	if(!centers.size()) { return ;}

	//dmess("pos " << centers[0]);

	TileBoundaryShader::getDefaultInstance()->bind(canvas, false, renderStage);

	//GLuint ebo  = 0;
    //GLuint ebo2 = 0;
    GLuint vao = 0;
	GLuint vbo  = 0;
    
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

    glGenBuffers(1, &vbo);
    //glGenBuffers(1, &ebo);
    //glGenBuffers(1, &ebo2);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, centers.size() * sizeof(vec4), &centers[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);

	size_t sizeVertex = 4;
	size_t totalSize = sizeVertex * sizeof(GLfloat);

	glVertexAttribPointer(0, (GLint)sizeVertex, GL_FLOAT, GL_FALSE, (GLsizei)totalSize, 0);
	glLineWidth(3.0);
	glDrawArrays(GL_POINTS, 0, centers.size());
	glLineWidth(1.0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glBindVertexArray(0);
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

	auto prevTiles = m_tiles;

	m_tiles.clear();

	getTilesToRender(canvas, tMin, tMax, m_startLevel);

	auto tileSet = m_tiles;

	for (auto i : m_tiles) { i->m_stillNeeded = true ;}

	for (auto i : prevTiles)
	{
		if(tileSet.find(i) == tileSet.end())
		{
			i->m_stillNeeded = false;
		}
	}

	unordered_set<RasterTile*> fallBackTiles;

	for (const auto tile : m_tiles)
	{
		if (tile->textureReady()) { continue; }

		auto currTile = tile;

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

	auto tiles = toVec(m_tiles);

	sort(tiles.begin(), tiles.end(), [](const RasterTile * A, const RasterTile * B) { return A->m_level < B->m_level ;});

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
				//tile->m_renderable = Renderable::create(makeWireBox(tMin, tMax), m_trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

				tile->m_renderable->ensureVAO();

				if(s_useBindlessTextures)	{ tile->m_renderable->setShader(BindlessTextureShader	::getDefaultInstance()) ;}
				else						{ tile->m_renderable->setShader(TextureShader			::getDefaultInstance()) ;}

				tile->m_renderable->setRenderOutline (false);
				tile->m_renderable->setRenderOutline (true);
				tile->m_renderable->setRenderFill    (true);
			}

			toRender.push_back(tile);
		}
		else if(tile->m_textureID != RasterTile::s_NO_DATA) { fetchTile(tile) ;}
	}

	if(s_useBindlessTextures) { renderBindlessTextures(canvas, toRender, renderStage) ;}
	else
	{
        for(auto tile : toRender)
        { 
            TextureShader::getDefaultInstance()->setTextureID(tile->m_textureID);
            
            tile->m_renderable->render(canvas, renderStage);
        }
	}

	RenderableBingMap::s_numRendered = toRender.size();

	s_textureBuffer->unbind();

	RasterTile::pruneTiles();
}

FrameBuffer* RenderableBingMap::getFrameBuffer() { return s_textureBuffer ;}
