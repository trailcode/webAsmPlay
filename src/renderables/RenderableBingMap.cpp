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

#ifdef WIN32

#else
#include <unistd.h>
#endif
#include <algorithm>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace glm;
using namespace geosUtil;
using namespace bingTileSystem;

namespace
{
#ifndef __EMSCRIPTEN__

    ctpl::thread_pool loaderPool(1);

    ctpl::thread_pool uploaderPool(1);

    mutex loaderMutex;
    mutex uploaderMutex;

    unordered_map<int, CURL *> curlHandles;

#endif

    const size_t levelOfDetail = 19;
    //const size_t levelOfDetail = 18;
    //const size_t levelOfDetail = 17;
    //const size_t levelOfDetail = 15;

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

    class BingTile
    {
    public:

        BingTile(const string & quadKey, Renderable * r) : quadKey(quadKey), r(r)
        {
#ifndef __EMSCRIPTEN__

            loaderPool.push([this](int ID) { fetchTile(ID) ;});

#endif
        }

        ~BingTile()
        {
            // TODO cleanup
        }

        void fetchTile(const int ID)
        {
#ifndef __EMSCRIPTEN__

            const string tileCachePath = "./tiles/" + quadKey + ".jpg";

			if(fileExists(tileCachePath))
            {
                uploaderPool.push([this, tileCachePath](int ID)
                {
					if(!contextSet)
					{ 
						glfwMakeContextCurrent(threadWin);

						contextSet = true;
					}

                    textureID = Textures::load(tileCachePath);
                });
            }
            else
            {
                //return;

                CURL * myHandle = NULL;

                {
                    lock_guard<mutex> _(loaderMutex);

                    if(curlHandles.find(ID) == curlHandles.end())
                    {
                        curlHandles[ID] = myHandle = curl_easy_init();
                    }

                    myHandle = curlHandles[ID];

					if (!myHandle)
					{
						dmess("Error!");

						abort();
					}
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

                const string url =  "https://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + 
                                    quadKey +
                                    "?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";

				//dmess("url " << url);

                curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());
                result = curl_easy_perform( myHandle );
                //dmess("result " << result << " myHandle " << myHandle);
                //curl_easy_cleanup( myHandle );

				if (!output->buffer)
				{
					dmess("Error!");

					abort();
				}

				//static GLFWwindow * threadWin = NULL;

                uploaderPool.push([this, output, tileCachePath](int ID)
                {
					if(!contextSet)
					{ 
						glfwMakeContextCurrent(threadWin);

						contextSet = true;
					}

                    textureID = Textures::createFromJpeg(output->buffer, output->size);

                    if(!fileExists(tileCachePath))
                    {
                        FILE * fp = fopen(tileCachePath.c_str(), "wb");
                        
						if(fp)
						{
							fwrite(output->buffer, sizeof(char), output->size, fp);

							fclose(fp);
						}
						else
						{
							dmess("Warn could not write file: " << tileCachePath);
						}
                    }

                    if( output->buffer )
                    {
                        free ( output->buffer );
                        //output.buffer = 0;
                        //output.size = 0;
                    }

                    delete output;
                });
            }

            //dmess("done " << quadKey);
#endif
        }

        const string quadKey;

        Renderable * r = NULL;

        GLuint textureID = 0;

    private:
    };

    vector<BingTile *> tiles;

	FrameBuffer * textureBuffer = NULL;
}

RenderableBingMap::RenderableBingMap(const AABB2D & bounds, const dmat4 & trans) : bounds(bounds)
{
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
        const dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 1), levelOfDetail);
        const dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 0), levelOfDetail);

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

void RenderableBingMap::render(Canvas * canvas, const size_t renderStage) const
{
    if(!getRenderFill()) { return ;}

	//textureBuffer = FrameBuffer::ensureFrameBuffer(textureBuffer, canvas->frameBufferSize);

    for(const auto r : tiles)
    {
        if(!r->textureID) { continue ;}

        TextureShader::getDefaultInstance()->setTextureID(r->textureID);
		
        r->r->render(canvas, 0);
    }
}
