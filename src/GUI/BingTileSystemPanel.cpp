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
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#pragma warning( pop ) 

using namespace std;
using namespace glm;
using namespace bingTileSystem;

extern float resDelta;

void GUI::bingTileSystemPanel()
{
    if(!s_showBingTileSystemPanel) { return ;}

    dvec4 pos(s_canvas->getCursorPosWC(), 1.0);

    pos = s_client->getInverseTrans() * pos;

    const size_t zoomLevel = 17;

	const ivec2  pix	 = latLongToPixel(pos, zoomLevel);
	const dvec2  pos2	 = pixelToLatLong(pix, zoomLevel);
	const ivec2  tile	 = pixelToTile(pix);
    const string quadKey = tileToQuadKey(tile, zoomLevel);

	const dvec2 tMin = tileToLatLong(ivec2(tile.x + 0, tile.y + 1), zoomLevel);
	const dvec2 tMax = tileToLatLong(ivec2(tile.x + 1, tile.y + 0), zoomLevel);

    ImGui::Begin("Bing Tile System", &s_showBingTileSystemPanel);

	ImGui::Text(("      Local pos: " + toStr(vec2(s_canvas->getCursorPosWC()))).c_str());
    ImGui::Text(("            pos: " + toStr(vec2(pos))).c_str());
    ImGui::Text(("            pix: " + toStr(pix)).c_str());
    ImGui::Text(("           tile: " + toStr(tile)).c_str());
    ImGui::Text(("        quadKey: " + quadKey).c_str());
	ImGui::Text(("      num tiles: " + to_string(RasterTile::getNumTiles())).c_str());
	ImGui::Text(("    num loading: " + to_string(RenderableBingMap::s_numLoading)).c_str());
	ImGui::Text(("num downloading: " + to_string(RenderableBingMap::s_numDownloading)).c_str());
	ImGui::Text(("  num uploading: " + to_string(RenderableBingMap::s_numUploading)).c_str());
	ImGui::Text(("    num writing: " + to_string(RenderableBingMap::s_numWriting)).c_str());
	ImGui::Text(("   num rendered: " + to_string(RenderableBingMap::s_numRendered)).c_str());

	int maxDesiredNumTiles = (int)RasterTile::s_desiredMaxNumTiles;

	if (ImGui::SliderInt("Max Tile Cache Size", &maxDesiredNumTiles, 150, 6000))
	{
		dmess("maxDesiredNumTiles " << maxDesiredNumTiles);

		RasterTile::s_desiredMaxNumTiles = (size_t)maxDesiredNumTiles; 
	}
	
	ImGui::SliderFloat("###ResDelta", &resDelta, 0.125f, 5);
	
	if (ImGui::Button("StreetSide"))
	{
		//StreetSide::queryViewport();
	}

	const dvec3 P1 = dvec4(tMin.x, tMax.y, 0, 1);
	const dvec3 P2 = dvec4(tMax.x, tMax.y, 0, 1);
	const dvec3 P3 = dvec4(tMax.x, tMin.y, 0, 1);
	const dvec3 P4 = dvec4(tMin.x, tMin.y, 0, 1);
	/*
	const dvec3 transP1 = m_trans * dvec4(P1, 1);
	const dvec3 transP2 = m_trans * dvec4(P2, 1);
	const dvec3 transP3 = m_trans * dvec4(P3, 1);
	const dvec3 transP4 = m_trans * dvec4(P4, 1);

	DeferredRenderable::addLine();
	*/

    ImGui::End();
}

