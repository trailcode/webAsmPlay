/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#ifdef __EMSCRIPTEN__
#include <emscripten/fetch.h>
#else
#include <curl/curl.h>
#endif

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <geos/index/quadtree/Quadtree.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/geom/GeometryConverter.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/GeoClient.h>

#ifdef min
#undef min
#endif

using namespace std;
using namespace std::chrono;
using namespace glm;
using namespace geos::geom;
using namespace geos::index::quadtree;
using namespace curlUtil;

GeoClient::GeoClient(Canvas * canvas) : m_canvas(canvas)
{

    m_quadTreePolygons		= new Quadtree();
    m_quadTreeLineStrings	= new Quadtree();
    m_quadTreePoints		= new Quadtree();
    m_network				= new Network(this);
}

GeoClient::~GeoClient()
{
    // TODO cleanup!
}

namespace
{
#ifndef __EMSCRIPTEN__

	CURL * myHandle = nullptr; // TODO code dup

#endif
}

void GeoClient::loadGeometry(const string fileName)
{
    dmess("GeoClient::loadGeometry " << this);

#ifdef __EMSCRIPTEN__

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    attr.onprogress = downloadProgress;
    attr.userData = this;
    emscripten_fetch(&attr, fileName.c_str());

#else

	if (!fileName.rfind("http", 1))
	{
		CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
		
		BufferStruct output; // Create an instance of out BufferStruct to accept LCs output
		
		if (!myHandle) { myHandle = curl_easy_init(); }

		/* Notice the lack of major error checking, for brevity */

		curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, writeMemoryCallback); // Passing the function pointer to LC
		curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &output); // Passing our BufferStruct to LC

		curl_easy_setopt(myHandle, CURLOPT_URL, fileName.c_str());
		result = curl_easy_perform(myHandle);
		
		createWorld(output.m_buffer);

		GUI::progress("", 1.0);

		if (output.m_buffer) { free(output.m_buffer) ;}

		return;
	}

    auto fp = fopen(fileName.c_str(), "rb");

    if(!fp)
    {
        dmess("Error! Could not open: " << fileName);

		abort();
    }

    fseek(fp, 0, SEEK_END); // seek to end of file

    const size_t size = ftell(fp); // get current file pointer

    fseek(fp, 0, SEEK_SET);

    vector<char> data(size);

    fread(&data[0], sizeof(char), size, fp);

    fclose(fp);

	createWorld(&data[0]);

    GUI::progress("", 1.0);

#endif
}

void GeoClient::addBingMap(bool enabled)
{
    auto r = RenderableBingMap::create(getBounds(), m_trans);

    r->setRenderFill(enabled);

    getCanvas()->addRenderable(r);
}

AABB2D		GeoClient::getBounds()			const { return m_bounds			;}
dmat4		GeoClient::getTrans()			const { return m_trans			;}
dmat4		GeoClient::getInverseTrans()	const { return m_inverseTrans	;}
Network*	GeoClient::getNetwork()			const { return m_network		;}
Canvas*		GeoClient::getCanvas()			const { return m_canvas			;}

