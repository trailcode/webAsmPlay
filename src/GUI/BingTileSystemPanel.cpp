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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/BingTileSystem.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;
using namespace bingTileSystem;

void GUI::bingTileSystemPanel()
{
    if(!showBingTileSystemPanel) { return ;}

    dvec4 pos(canvas->getCursorPosWC(), 1.0);

    pos = client->getInverseTrans() * pos;

    double tmp = pos.x;
    pos.x = pos.y;
    pos.y = tmp;

    const size_t zoomLevel = 19;

    ivec2 pix = latLongToPixel(pos, zoomLevel);

    dvec2 pos2 = pixelToLatLong(pix, zoomLevel);

    ivec2 tile = pixelToTile(pix);

    string quadKey = tileToQuadKey(tile, zoomLevel);

    //dmess("pix " << pos.x << " " << pos.y << " " << pix.x << " " << pix.y << " " << pos2.x << " " << pos2.y);
    //dmess("quadKey " << quadKey);

    ImGui::Begin("Bing Tile System", &showBingTileSystemPanel);

    ImGui::Text(("    pos: " + toStr(vec2(pos))).c_str());
    ImGui::Text(("    pix: " + toStr(pix)).c_str());
    ImGui::Text(("   tile: " + toStr(tile)).c_str());
    ImGui::Text(("quadKey: " + quadKey).c_str());

    ImGui::End();
}

