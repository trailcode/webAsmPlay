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
#endif

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>

#include <webAsmPlay/GeosUtil.h>

using namespace std;
using namespace glm;
using namespace geosUtil;
using namespace BingTileSystem;

namespace
{
    const size_t levelOfDetail = 15;

    vector<Renderable *> tiles;

    // Define our struct for accepting LCs output
    struct BufferStruct
    {
        char * buffer;
        size_t size;
    };

    // This is the function we pass to LC, which writes the output to a BufferStruct
    static size_t writeMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        dmess("nmemb " << nmemb);

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

    class BingTile
    {
    public:

        BingTile(const string & quadKey) : quadKey(quadKey)
        {
            fetchTile();
        }

        ~BingTile()
        {

        }

        void fetchTile()
        {
            CURL * myHandle;
            CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
            struct BufferStruct output; // Create an instance of out BufferStruct to accept LCs output
            output.buffer = NULL;
            output.size = 0;
            myHandle = curl_easy_init ( ) ;

            /* Notice the lack of major error checking, for brevity */

            curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
            curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, (void *)&output); // Passing our BufferStruct to LC

            const string url =  "https://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + 
                                quadKey +
                                "?mkt=en-GB&it=A,G,RL&shading=hill&n=z&og=146&c4w=1";

            curl_easy_setopt(myHandle, CURLOPT_URL, url.c_str());
            result = curl_easy_perform( myHandle );
            curl_easy_cleanup( myHandle );

            Textures::createFromJpeg(output.buffer, output.size);

            FILE * fp;
            string outPath = "./tiles/" + quadKey + ".jpg";

            fp = fopen(outPath.c_str(), "wb");
            //if( !fp )
            dmess("output.size " << output.size);
            //return;
            //fprintf(fp, output.buffer );
            fwrite(output.buffer, sizeof(char), output.size, fp);
            fclose( fp );

            if( output.buffer )
            {
                free ( output.buffer );
                output.buffer = 0;
                output.size = 0;
            }

            dmess("done " << quadKey);
        }

        const string quadKey;

    private:
    };
}

RenderableBingMap::RenderableBingMap(const AABB2D & bounds, const dmat4 & trans) : bounds(bounds)
{
    minTile = latLongToTile(dvec2(get<1>(bounds), get<0>(bounds)), levelOfDetail);

    maxTile = latLongToTile(dvec2(get<3>(bounds), get<2>(bounds)), levelOfDetail);

    int tmp = minTile.y;
    minTile.y = maxTile.y;
    maxTile.y = tmp;

    dmess("minTile " << minTile.x << " " << minTile.y);

    for(int x = minTile.x; x <= maxTile.x; ++x)
    for(int y = minTile.y; y <= maxTile.y; ++y)
    {
        dvec2 tMin = tileToLatLong(ivec2(x + 0, y + 0), levelOfDetail);
        dvec2 tMax = tileToLatLong(ivec2(x + 1, y + 1), levelOfDetail);

        string quadKey = tileToQuadKey(ivec2(x, y), levelOfDetail);

        dmess("quadKey " << quadKey);

        new BingTile(quadKey);

        double tmp = tMin.x; tMin.x = tMin.y; tMin.y = tmp;

        tmp = tMax.x; tMax.x = tMax.y; tMax.y = tmp;

        tiles.push_back(Renderable::create(makeBox(tMin, tMax), trans));

        
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
    //return;

    //dmess("minTile " << maxTile.x - minTile.x << " " << maxTile.y - minTile.y);

    for(const auto r : tiles)
    {
        r->render(canvas, 0);
    }
}
