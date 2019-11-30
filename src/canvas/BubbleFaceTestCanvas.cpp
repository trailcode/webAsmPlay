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

//#define STB_TRUETYPE_IMPLEMENTATION
//#include <imgui/imstb_truetype.h>

#include <webAsmPlay/GUI/ImguiInclude.h>

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     IM_ASSERT(x)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef IMGUI_STB_TRUETYPE_FILENAME
#include IMGUI_STB_TRUETYPE_FILENAME
#else
#include "imstb_truetype.h"
#endif
#endif

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/renderables/RenderableText.h>
#include <webAsmPlay/canvas/BubbleFaceTestCanvas.h>

using namespace std;
using namespace glm;
using namespace boost::geometry;
using namespace boostGeom;

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
	void doQuadBox(const string & prefix, const Box & b, vector<pair<string, Box> > & out)
	{
		const auto quad = quadBox(b);

		for(size_t i = 0; i < 4; ++i) { out.push_back(make_pair(prefix + to_string(i), quad[i])) ;}
	}

	void doQuadBox(const vector<pair<string, Box> > & in, vector<pair<string, Box> > & out)
	{
		for(const auto & [prefix, b] : in) { doQuadBox(prefix, b, out) ;}
	}
}

#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/shaders/TextureShader.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <SDL_image.h>

BubbleFaceTestCanvas::BubbleFaceTestCanvas()
{
	Box b = Box{{-1,-1},{1,1}};

	vector<pair<string, Box> > curr[2];

	curr[0].push_back({"", b});

	size_t i;

	//for(i = 0; i < 4; ++i)
	for(i = 0; i < 1; ++i)
	{
		auto & in = curr[i % 2];
		auto & out = curr[(i + 1) % 2];

		doQuadBox(in, out);

		in.clear();
	}

	for(const auto & [id, b] : curr[i % 2])
	{
		const string url = "http://t.ssl.ak.tiles.virtualearth.net/tiles/hs020101231322332110" + id + ".jpg?g=6338&n=z";

		dmess("url " << url);

		curlUtil::BufferStruct * buf = curlUtil::download(url).get();

		auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(buf->m_buffer, buf->m_size));

		auto tex = Textures::load(img);

		auto bb = AABB2D(0.0,0.0,1.0,1.0);

		//auto r = Renderable::create(toPolygon(b), mat4(1.0f), 0, bb); // TODO Get this to work!

		const auto min = dvec2(b.min_corner().x(), b.min_corner().y());
		const auto max = dvec2(b.max_corner().x(), b.max_corner().y());

		addRenderable(Renderable::create(geosUtil::makeBox(min, max), mat4(1.0f), AABB2D(min.x, min.y, max.x, max.y), true)->
										setRenderFill	(true)->
										setRenderOutline(false)->
										setShader		((new TextureShader())->setTextureID(tex)));

		const auto pos = getCentroid(b);

		addRenderable(RenderableText::create(id, {pos.x(), pos.y(), 0}));
	}
}

BubbleFaceTestCanvas::~BubbleFaceTestCanvas()
{

}
