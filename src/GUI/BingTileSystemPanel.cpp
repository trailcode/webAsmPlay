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
#pragma warning( push )
#pragma warning( disable : 4005)
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/bing/BingTileSystem.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/bing/RasterTile.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#pragma warning( pop ) 

using namespace std;
using namespace glm;
using namespace bingTileSystem;

extern float resDelta;

namespace
{
	unique_ptr<Renderable> a_tileSystemGeom;

	string a_quadKey = "";

	dvec2 a_tMin;
	dvec2 a_tMax;
}

void GUI::bingTileSystemPanel()
{
    if(!s_showBingTileSystemPanel) { return ;}

    dvec4 pos(s_canvas->getCursorPosWC(), 1.0);

    pos = s_client->getInverseTrans() * pos;

    const size_t zoomLevel = 16;

	const ivec2  pix	 = latLongToPixel(pos, zoomLevel);
	const dvec2  pos2	 = pixelToLatLong(pix, zoomLevel);
	const ivec2  tile	 = pixelToTile(pix);
    
	a_quadKey = tileToQuadKey(tile, zoomLevel);

	a_tMin = tileToLatLong(ivec2(tile.x + 0, tile.y + 1), zoomLevel);
	a_tMax = tileToLatLong(ivec2(tile.x + 1, tile.y + 0), zoomLevel);

    ImGui::Begin("Bing Tile System", &s_showBingTileSystemPanel);

	ImGui::Text(("      Local pos: " + toStr(vec2(s_canvas->getCursorPosWC()))).c_str());
    ImGui::Text(("            pos: " + toStr(vec2(pos))).c_str());
    ImGui::Text(("            pix: " + toStr(pix)).c_str());
    ImGui::Text(("           tile: " + toStr(tile)).c_str());
    ImGui::Text(("        quadKey: " + a_quadKey).c_str());
	ImGui::Text((" Tiles rendered: " + to_string(RenderableBingMap::s_numRendered)).c_str());

	ImGui::SliderFloat("###ResDelta", &resDelta, 0.125f, 5);
	
	const vec3 P1 = vec3(a_tMin.x, a_tMax.y, 0);
	const vec3 P2 = vec3(a_tMax.x, a_tMax.y, 0);
	const vec3 P3 = vec3(a_tMax.x, a_tMin.y, 0);
	const vec3 P4 = vec3(a_tMin.x, a_tMin.y, 0);

	DeferredRenderable::addLine(P1, P2, {0,1,0,1}, DeferredRenderable::GUI);
	DeferredRenderable::addLine(P2, P3, {0,1,0,1}, DeferredRenderable::GUI);
	DeferredRenderable::addLine(P3, P4, {0,1,0,1}, DeferredRenderable::GUI);
	DeferredRenderable::addLine(P4, P1, {0,1,0,1}, DeferredRenderable::GUI);

    ImGui::End();
}

void GUI::initBingTileSystemPanel(const dmat4 & trans)
{
	GUI::addUpdatable([trans]()
    {
		if (!a_tileSystemGeom) { a_tileSystemGeom = unique_ptr<Renderable>(DeferredRenderable::createFromQueued(DeferredRenderable::GUI, trans)); }
		else
		{
			((DeferredRenderable*)a_tileSystemGeom.get())->setFromQueued(DeferredRenderable::GUI, trans);
		}

		if(a_tileSystemGeom) { a_tileSystemGeom->render(getMainCanvas(), POST_G_BUFFER) ;}

	});
}

