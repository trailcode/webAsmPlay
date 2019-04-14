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
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/renderables/RenderableBingMap.h>
#pragma warning( pop ) 

using namespace std;
using namespace glm;
using namespace bingTileSystem;

void GUI::bingTileSystemPanel()
{
    if(!s_showBingTileSystemPanel) { return ;}

    dvec4 pos(s_canvas->getCursorPosWC(), 1.0);

    pos = s_client->getInverseTrans() * pos;

    const size_t zoomLevel = 16;

	const ivec2  pix	 = latLongToPixel(pos, zoomLevel);
	const dvec2  pos2	 = pixelToLatLong(pix, zoomLevel);
	const ivec2  tile	 = pixelToTile(pix);
    const string quadKey = tileToQuadKey(tile, zoomLevel);

    ImGui::Begin("Bing Tile System", &s_showBingTileSystemPanel);

	ImGui::Text(("      Local pos: " + toStr(vec2(s_canvas->getCursorPosWC()))).c_str());
    ImGui::Text(("            pos: " + toStr(vec2(pos))).c_str());
    ImGui::Text(("            pix: " + toStr(pix)).c_str());
    ImGui::Text(("           tile: " + toStr(tile)).c_str());
    ImGui::Text(("        quadKey: " + quadKey).c_str());
	ImGui::Text(("      num tiles: " + to_string(RenderableBingMap::s_numTiles)).c_str());
	ImGui::Text(("    num loading: " + to_string(RenderableBingMap::s_numLoading)).c_str());
	ImGui::Text(("num downloading: " + to_string(RenderableBingMap::s_numDownloading)).c_str());
	ImGui::Text(("  num uploading: " + to_string(RenderableBingMap::s_numUploading)).c_str());
	ImGui::Text(("    num writing: " + to_string(RenderableBingMap::s_numWriting)).c_str());
	ImGui::Text(("   num rendered: " + to_string(RenderableBingMap::s_numRendered)).c_str());

    ImGui::End();
}

