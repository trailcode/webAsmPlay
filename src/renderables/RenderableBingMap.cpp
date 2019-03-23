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
#include <unordered_set>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/shaders/BindlessTextureShader.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/TextureLookupShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

using namespace std;
using namespace glm;
using namespace geosUtil;
using namespace bingTileSystem;

atomic<size_t> RenderableBingMap::numTiles			= {0};
atomic<size_t> RenderableBingMap::numLoading		= {0};
atomic<size_t> RenderableBingMap::numDownloading	= {0};
atomic<size_t> RenderableBingMap::numUploading		= {0};

size_t RenderableBingMap::numRendered = 0;

namespace
{
#ifndef __EMSCRIPTEN__

    ctpl::thread_pool loaderPool(16);

    ctpl::thread_pool uploaderPool(1);

    mutex loaderMutex;
    mutex uploaderMutex;

    unordered_map<int, CURL *> curlHandles;

#endif

	enum
	{
		NUM_TEXTURES  = 2048,
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

    unordered_set<int> createdContexts;

    bool contextCreated = false;
	bool contextSet = false;

	static GLFWwindow * threadWin = NULL;

	typedef pair<const char *, const size_t> TileBuffer;

	TileBuffer downloadTile(const int ID, const string & quadKey)
	{
		CURL * myHandle = NULL;

		{
			lock_guard<mutex> _(loaderMutex);

			if(curlHandles.find(ID) == curlHandles.end())
			{
				curlHandles[ID] = myHandle = curl_easy_init();
			}

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
		result = curl_easy_perform( myHandle );
		//dmess("result " << result << " myHandle " << myHandle);
		//curl_easy_cleanup( myHandle );

		const char * ret = output->buffer;

		const size_t size = output->size;

		//if (!ret) { dmess("Error!") ;} // It will retry later

		delete output;

		return TileBuffer(ret, size);
	}

	FrameBuffer * textureBuffer = NULL;

	class Tile
	{
	public:

		Tile(const dvec2 & center, const size_t level) : center(center), level(level) {}

		const dvec2		center;
		const size_t	level;

		// TODO skip loading if no longer needed

		atomic_bool loading = {false};

		Renderable * r = NULL;

		GLuint textureID = 0;

		GLuint64    handle = 0;

		bool textureResident = false;
	};

	void fetchTile(const int ID, Tile * tile)
	{
		const string quadKey = tileToQuadKey(latLongToTile(tile->center, tile->level), tile->level);

		const string tileCachePath = "./tiles/" + quadKey + ".jpg";

		if(fileExists(tileCachePath))
		{
			++RenderableBingMap::numUploading;

			uploaderPool.push([tile, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				tile->textureID = Textures::load(tileCachePath);

				tile->handle = glGetTextureHandleARB(tile->textureID);

				++RenderableBingMap::numTiles;

				--RenderableBingMap::numLoading;

				--RenderableBingMap::numUploading;
			});
		}
		else
		{
			TileBuffer tileBuffer = downloadTile(ID, quadKey);

			if (!get<0>(tileBuffer))
			{
				dmess("Error! No buffer!");

				return;
			}

			++RenderableBingMap::numUploading;

			uploaderPool.push([tile, tileBuffer, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				tile->textureID = Textures::createFromJpeg(get<0>(tileBuffer), get<1>(tileBuffer));

				tile->handle = glGetTextureHandleARB(tile->textureID);

				++RenderableBingMap::numTiles;

				--RenderableBingMap::numUploading;

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
		}
	}

	unordered_map<string, Tile *> currTileSet;

	Tile * getTile(const dvec2 & center, const size_t level)
	{
		char buf[1024];

		sprintf(buf, "%f %f %zi", center.x, center.y, level);

		unordered_map<string, Tile *>::const_iterator i = currTileSet.find(buf);

		if (i != currTileSet.end()) { return i->second ;}
	
		return currTileSet[buf] = new Tile(center, level);
	}

	vector<Tile *> tiles;
}

void RenderableBingMap::getStartLevel()
{
	for (int i = 0; i < 24; ++i)
	{
		const ivec2 minTile = latLongToTile(dvec2(get<0>(bounds), get<1>(bounds)), i);
		const ivec2 maxTile = latLongToTile(dvec2(get<2>(bounds), get<3>(bounds)), i);

		if(minTile == maxTile) { continue ;}

		startLevel = i - 1;

		break;
	}
}

Renderable * RenderableBingMap::create(const AABB2D & bounds, const dmat4 & trans) { return new RenderableBingMap(bounds, trans) ;}

RenderableBingMap::RenderableBingMap(const AABB2D & bounds, const dmat4 & trans) : bounds(bounds), trans(trans)
{
	getStartLevel();
	//return;

	if(!contextCreated)
	{
		// TODO Create a openGL context class;
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		threadWin = glfwCreateWindow(1, 1, "Thread Window", NULL, GUI::getMainWindow());

		contextCreated = true;
	}

	glGenBuffers(1,					&buffers.textureHandleBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER,  buffers.textureHandleBuffer);

	glBufferStorage(GL_UNIFORM_BUFFER, NUM_TEXTURES * sizeof(GLuint64) * 2, nullptr, GL_MAP_WRITE_BIT);
}

RenderableBingMap::~RenderableBingMap()
{

}

void RenderableBingMap::getTilesToRender(Canvas * canvas, const dvec2 & tMin, const dvec2 & tMax, const size_t level) const
{
	const dvec3 center = dvec4(((tMin + tMax) * 0.5), 0.0, 1.0);

	const dvec3 P1 = dvec4(tMin.x, tMax.y, 0, 1);
	const dvec3 P2 = dvec4(tMax.x, tMax.y, 0, 1);
	const dvec3 P3 = dvec4(tMax.x, tMin.y, 0, 1);
	const dvec3 P4 = dvec4(tMin.x, tMin.y, 0, 1);

	const dvec3 transCenter = trans * dvec4(center, 1.0);

	const dvec3 transP1 = trans * dvec4(P1, 1);
	const dvec3 transP2 = trans * dvec4(P2, 1);
	const dvec3 transP3 = trans * dvec4(P3, 1);
	const dvec3 transP4 = trans * dvec4(P4, 1);

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

	if (level < 23 && (D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	//if (level < 12 && (D1 >= tileSize || D2 >= tileSize || D3 >= tileSize || D4 >= tileSize))
	{
		const dvec3 subPoints[] = {	/* 0 */dvec3(tMin.x, tMax.y,	0),		/* 1 */dvec3(center.x, tMax.y,   0), /* 2 */dvec3(tMax.x, tMax.y,   0),
									/* 3 */dvec3(tMin.x, center.y,  0),		/* 4 */dvec3(center.x, center.y, 0), /* 5 */dvec3(tMax.x, center.y, 0),
									/* 6 */dvec3(tMin.x, tMin.y,	0),		/* 7 */dvec3(center.x, tMin.y,   0), /* 8 */dvec3(tMax.x, tMin.y,   0)};

		const dvec3 subPointsTrans[] = {	/* 0 */trans * dvec4(tMin.x, tMax.y,   0, 1), /* 1 */trans * dvec4(center.x, tMax.y,   0, 1), /* 2 */trans * dvec4(tMax.x, tMax.y,   0, 1),
											/* 3 */trans * dvec4(tMin.x, center.y, 0, 1), /* 4 */trans * dvec4(center.x, center.y, 0, 1), /* 5 */trans * dvec4(tMax.x, center.y, 0, 1),
											/* 6 */trans * dvec4(tMin.x, tMin.y,   0, 1), /* 7 */trans * dvec4(center.x, tMin.y,   0, 1), /* 8 */trans * dvec4(tMax.x, tMin.y,   0, 1)};

		
		if(frust->intersects(subPointsTrans[0], subPointsTrans[1], subPointsTrans[4], subPointsTrans[3])) { getTilesToRender(canvas, subPoints[3], subPoints[1], level + 1) ;}
		if(frust->intersects(subPointsTrans[1], subPointsTrans[2], subPointsTrans[5], subPointsTrans[4])) { getTilesToRender(canvas, subPoints[4], subPoints[2], level + 1) ;}
		if(frust->intersects(subPointsTrans[3], subPointsTrans[4], subPointsTrans[7], subPointsTrans[6])) { getTilesToRender(canvas, subPoints[6], subPoints[4], level + 1) ;}
		if(frust->intersects(subPointsTrans[4], subPointsTrans[5], subPointsTrans[8], subPointsTrans[7])) { getTilesToRender(canvas, subPoints[7], subPoints[5], level + 1) ;}
		// TODO try projecting into a different space and doing a simpler check.

		return;
	}

	tiles.push_back(getTile(center, level));
}

extern GLuint theTex;

void RenderableBingMap::render(Canvas * canvas, const size_t renderStage) const
{
    if(!getRenderFill()) { return ;}

	/*
	FrameBuffer::ensureFrameBuffer(	textureBuffer,
									canvas->getFrameBufferSize(),
									GL_RGB32UI,
									GL_RGB,
									GL_UNSIGNED_INT);
									*/

	FrameBuffer::ensureFrameBuffer(	textureBuffer,
									canvas->getFrameBufferSize());

	textureBuffer->bind();
	
	//GL_CHECK(glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	theTex = textureBuffer->getTextureID();

	//dmess("theTex " << theTex);

	const ivec2 minTile = latLongToTile(dvec2(get<0>(bounds), get<1>(bounds)), startLevel);

	const dvec2 tMin = tileToLatLong(ivec2(minTile.x + 0, minTile.y + 0), startLevel);
	const dvec2 tMax = tileToLatLong(ivec2(minTile.x + 1, minTile.y + 1), startLevel);

	tiles.clear();

	getTilesToRender(canvas, tMin, tMax, startLevel);

	sort(tiles.begin(), tiles.end(), [](const Tile * A, const Tile * B) { return A->level < B->level ;});

	size_t numRendered = 0;

	vector<Tile *> toRender;

	const bool useBindlessTextures = false;

	for (auto i : tiles)
	{
		if (i->textureID)
		{
			if(!i->r)
			{
				const ivec2 iTile = latLongToTile(i->center, i->level);

				const dvec2 tMin = tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), i->level);
				const dvec2 tMax = tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), i->level);
				
				i->r = Renderable::create(makeBox(tMin, tMax), trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

				i->r->ensureVAO();

				if(useBindlessTextures) { i->r->setShader(BindlessTextureShader	::getDefaultInstance()) ;}
				else					{ i->r->setShader(TextureShader			::getDefaultInstance()) ;}

				i->r->setRenderOutline (false);
				i->r->setRenderFill    (true);
			}

			toRender.push_back(i);

			++numRendered;
		}
		else if (!i->loading)
		{
			i->loading = true;

			loaderPool.push([i](int ID) { fetchTile(ID, i) ;});

			++numLoading;
		}
	}

	if(useBindlessTextures)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 6, buffers.textureHandleBuffer);

		GLuint64 * pHandles = (GLuint64*)glMapBufferRange(GL_UNIFORM_BUFFER, 0,  toRender.size() * sizeof(GLuint64) * 2, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for(size_t i = 0; i < toRender.size(); ++i)
		{
			Tile * t = toRender[i];

			if(!t->textureResident)
			{ 
				glMakeTextureHandleResidentARB(t->handle);

				t->textureResident = true;
			}

			pHandles[i * 2] = t->handle;		
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		for(size_t i = 0; i < toRender.size(); ++i)
		{
			Tile * t = toRender[i];

			if(useBindlessTextures) { t->r->setShader(BindlessTextureShader	::getDefaultInstance()) ;}
			else					{ t->r->setShader(TextureShader			::getDefaultInstance()) ;}

			BindlessTextureShader::getDefaultInstance()->setTextureSlot(i);

			t->r->render(canvas);
		}
	}
	else
	{
        for(auto tile : toRender)
        {
            TextureShader::getDefaultInstance()->setTextureID(tile->textureID);
            
            tile->r->render(canvas);
        }
	}

	RenderableBingMap::numRendered = toRender.size();

	textureBuffer->unbind();
}
