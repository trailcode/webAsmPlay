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
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/Frustum.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

using namespace std;
using namespace glm;
using namespace geosUtil;
using namespace bingTileSystem;

namespace
{
#ifndef __EMSCRIPTEN__

    ctpl::thread_pool loaderPool(16);

    ctpl::thread_pool uploaderPool(1);

    mutex loaderMutex;
    mutex uploaderMutex;

    unordered_map<int, CURL *> curlHandles;

#endif

    //const size_t levelOfDetail = 19;
    //const size_t levelOfDetail = 18;
    //const size_t levelOfDetail = 17;
    const size_t levelOfDetail = 15;

    // Define our struct for accepting LCs output
    struct BufferStruct // TODO code dupilcation
    {
        char * buffer;
        size_t size;
    };

    // This is the function we pass to LC, which writes the output to a BufferStruct
    static size_t writeMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) // TODO code dupilcation
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

		//dmess("Start!");

		CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
		struct BufferStruct * output = new BufferStruct; // Create an instance of out BufferStruct to accept LCs output
		output->buffer = NULL;
		output->size = 0;

		//if(!myHandle) { myHandle = curl_easy_init() ;}

		/* Notice the lack of major error checking, for brevity */

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, (void *)output); // Passing our BufferStruct to LC

																	   /*
																	   const string url =  "https://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + 
																	   quadKey +
																	   "?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";
																	   */

		const string url =  "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + 
			quadKey +
			"?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";

		//dmess("url " << url);

		curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());
		result = curl_easy_perform( myHandle );
		//dmess("result " << result << " myHandle " << myHandle);
		//curl_easy_cleanup( myHandle );

		const char * ret = output->buffer;

		const size_t size = output->size;

		if (!ret)
		{
			dmess("Error!");
		}

		delete output;

		return TileBuffer(ret, size);
	}

	void fetchTile(const int ID, GLuint * textureID, const string & quadKey)
	{
#ifndef __EMSCRIPTEN__

		const string tileCachePath = "./tiles/" + quadKey + ".jpg";

		if(fileExists(tileCachePath))
		{
			uploaderPool.push([textureID, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				*textureID = Textures::load(tileCachePath);
			});
		}
		else
		{
			TileBuffer tileBuffer = downloadTile(ID, quadKey);

			uploaderPool.push([textureID, tileBuffer, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				*textureID = Textures::createFromJpeg(get<0>(tileBuffer), get<1>(tileBuffer));

				if(!fileExists(tileCachePath))
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
				}

				if( get<0>(tileBuffer) )
				{
					free ( (void *)get<0>(tileBuffer) );
				}
			});
		}

		//dmess("done " << quadKey);
#endif
	}

    class BingTile
    {
    public:

        BingTile(const string & quadKey, Renderable * r) : quadKey(quadKey), r(r)
        {
#ifndef __EMSCRIPTEN__

            loaderPool.push([this, quadKey](int ID) { fetchTile(ID, &textureID, quadKey) ;});

#endif
        }

        ~BingTile()
        {
            // TODO cleanup
        }

		void render(Canvas * canvas) const { r->render(canvas) ;}

        const string quadKey;

        Renderable * r = NULL;

        GLuint textureID = 0;

    private:
    };

    vector<BingTile *> tiles;

	FrameBuffer * textureBuffer = NULL;

	class Tile
	{
	public:

		Tile(const dvec2 & center, const size_t level) : center(center), level(level)
		{
			
		}

		const dvec2 center;
		const size_t level;

		// TODO skip loading if no longer needed

		bool loading = false;

		Renderable * r = NULL;

		GLuint textureID = 0;
	};

	void fetchTile(const int ID, Tile * tile)
	{
		//dmess("fetchTile " << tile);

		const string quadKey = tileToQuadKey(latLongToTile(tile->center, tile->level), tile->level);

		const string tileCachePath = "./tiles/" + quadKey + ".jpg";

		if(fileExists(tileCachePath))
		{
			uploaderPool.push([tile, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				tile->textureID = Textures::load(tileCachePath);
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

			uploaderPool.push([tile, tileBuffer, tileCachePath](int ID)
			{
				if(!contextSet)
				{ 
					glfwMakeContextCurrent(threadWin);

					contextSet = true;
				}

				tile->textureID = Textures::createFromJpeg(get<0>(tileBuffer), get<1>(tileBuffer));

				//dmess("tile->textureID " << tile->textureID);

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

		sprintf(buf, "%f %f %i", center.x, center.y, level);

		unordered_map<string, Tile *>::const_iterator i = currTileSet.find(buf);

		Tile * tile;

		if (i != currTileSet.end())
		{
			tile = i->second;
		}
		else
		{
			tile = new Tile(center, level);

			currTileSet[buf] = tile;
		}

		/*
		switch (tile->state)
		{
		case Tile::UNLOADED:
			break;
		case Tile:
		}
		*/

		return tile;
	}

	vector<Tile *> tiles2;
}

void RenderableBingMap::getStartLevel()
{
	for (int i = 0; i < 24; ++i)
	{
		const ivec2 minTile = latLongToTile(dvec2(get<0>(bounds), get<1>(bounds)), i);
		const ivec2 maxTile = latLongToTile(dvec2(get<2>(bounds), get<3>(bounds)), i);

		dmess(i << " minTile " << minTile.x << " " << minTile.y << " " << maxTile.x << " " << maxTile.y);

		//if(minTile.x == maxTile.x && minTile.y == maxTile.y)
		if(minTile == maxTile)
		{
			dmess("     ddd ");

			continue;
		}

		startLevel = i - 1;

		//startLevel = i;

		break;
	}

	dmess("startLevel " << startLevel);

	//startLevel = 14;
}

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

    minTile = latLongToTile(dvec2(get<0>(bounds), get<1>(bounds)), levelOfDetail);
    maxTile = latLongToTile(dvec2(get<2>(bounds), get<3>(bounds)), levelOfDetail);

	if(minTile.x > maxTile.x)
	{
		int tmp = minTile.x;  // TODO does this happen anymore?
		minTile.x = maxTile.x;
		maxTile.x = tmp;
	}

    for(int x = minTile.x; x <= maxTile.x; ++x)
    for(int y = minTile.y; y <= maxTile.y; ++y)
    {
		//*
        const dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 1), levelOfDetail);
        const dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 0), levelOfDetail);
		//*/

		/*
		const dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 0), levelOfDetail);
		const dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 1), levelOfDetail);
		*/

        const string quadKey = tileToQuadKey(ivec2(x, y), levelOfDetail);

        Renderable * r = Renderable::create(makeBox(tMin, tMax), trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

        r->setShader(TextureShader::getDefaultInstance());

        r->setRenderOutline (false);
        r->setRenderFill    (true);

        tiles.push_back(new BingTile(quadKey, r));
    }
}

RenderableBingMap::~RenderableBingMap()
{

}

Renderable * RenderableBingMap::create(const AABB2D & bounds, const dmat4 & trans)
{
    return new RenderableBingMap(bounds, trans);
}

size_t culled = 0;
size_t rendered = 0;

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

	// https://stackoverflow.com/questions/35261192/how-to-use-glmproject-to-get-the-coordinates-of-a-point-in-world-space

	const dvec3 screenCenter = project(transCenter, canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);

	//dmess("screenCenter " << screenCenter);

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

		/*
		dvec3 screenPoints[6];

		for(size_t i = 0; i < 6; ++i)
		{
			screenPoints[i] = project(subPointsTrans[i], canvas->getMV_Ref(), canvas->getProjectionRef(), viewport);

			dvec4 p = canvas->getMVP_Ref() * dvec4(subPointsTrans[i], 1.0);

			//dmess("screen " << i << " " << screenPoints[i]);
			dmess("screen " << i << " " << p);
		}
		*/
		
		if(frust->intersects(subPointsTrans[0], subPointsTrans[1], subPointsTrans[4], subPointsTrans[3])) { getTilesToRender(canvas, subPoints[3], subPoints[1], level + 1) ;} else { ++culled ;}
		if(frust->intersects(subPointsTrans[1], subPointsTrans[2], subPointsTrans[5], subPointsTrans[4])) { getTilesToRender(canvas, subPoints[4], subPoints[2], level + 1) ;} else { ++culled ;}
		if(frust->intersects(subPointsTrans[3], subPointsTrans[4], subPointsTrans[7], subPointsTrans[6])) { getTilesToRender(canvas, subPoints[6], subPoints[4], level + 1) ;} else { ++culled ;}
		if(frust->intersects(subPointsTrans[4], subPointsTrans[5], subPointsTrans[8], subPointsTrans[7])) { getTilesToRender(canvas, subPoints[7], subPoints[5], level + 1) ;} else { ++culled ;}
		// TODO try projecting into a different space and doing a simpler check.

		return;
	}

	//dmess("screenPos " << screenCenter << " center " << center << " D1 " << D1 << " D2 " << D2 << " D3 " << D3 << " D4 " << D4);

	tiles2.push_back(getTile(center, level));

	/*
	const dvec3 cameraPos = canvas->getCamera()->getCenter();

	//dmess("P1 " << frustum.getPlane(0).classifyPoint(P1) << " " << distance(cameraPos, P1));

	Renderable * r = Renderable::create(makeBox(tMin, tMax), trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

	r->setShader(ColorShader::getDefaultInstance());

	//ColorShader::getDefaultInstance()->

	r->setRenderFill(true);
	//r->setRenderFill(false);
	r->setRenderOutline(true);

	r->render(canvas);

	delete r;

	++rendered;
	*/
}

void RenderableBingMap::render(Canvas * canvas, const size_t renderStage) const
{
    if(!getRenderFill()) { return ;}

	FrameBuffer::ensureFrameBuffer(textureBuffer, canvas->getFrameBufferSize());

    for(const auto r : tiles)
    {
        if(!r->textureID) { continue ;}

        TextureShader::getDefaultInstance()->setTextureID(r->textureID);
		
        r->render(canvas);
    }

	//Frustum frustum(canvas->getMVP_Ref());

	size_t startLevel = 11;

	const ivec2 minTile = latLongToTile(dvec2(get<0>(bounds), get<1>(bounds)), startLevel);

	const dvec2 tMin = tileToLatLong(ivec2(minTile.x + 0, minTile.y + 0), startLevel);
	const dvec2 tMax = tileToLatLong(ivec2(minTile.x + 1, minTile.y + 1), startLevel);

	culled = 0;
	rendered = 0;

	tiles2.clear();

	getTilesToRender(canvas, tMin, tMax, startLevel);

	sort(tiles2.begin(), tiles2.end(), [](const Tile * A, const Tile * B)
	{
		return A->level < B->level;
	});

	for (auto i : tiles2)
	{
		if (i->textureID)
		{
			if(!i->r)
			{
				const ivec2 iTile = latLongToTile(i->center, i->level);

				const dvec2 tMin = tileToLatLong(ivec2(iTile.x + 0, iTile.y + 1), i->level);
				const dvec2 tMax = tileToLatLong(ivec2(iTile.x + 1, iTile.y + 0), i->level);
				//*/

				/*
				const dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 0), levelOfDetail);
				const dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 1), levelOfDetail);
				*/

				//const string quadKey = tileToQuadKey(ivec2(x, y), levelOfDetail);

				i->r = Renderable::create(makeBox(tMin, tMax), trans, AABB2D(tMin.x, tMin.y, tMax.x, tMax.y));

				i->r->setShader(TextureShader::getDefaultInstance());

				i->r->setRenderOutline (false);
				i->r->setRenderFill    (true);
			}

			TextureShader::getDefaultInstance()->setTextureID(i->textureID);

			i->r->render(canvas);
		}
		else if (!i->loading)
		{
			i->loading = true;

			loaderPool.push([i](int ID) { fetchTile(ID, i) ;});
		}
	}

	//dmess("rendered " << rendered << " culled " << culled);
}
