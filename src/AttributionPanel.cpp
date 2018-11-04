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

#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/ColorDistanceShader.h>
#include <webAsmPlay/ColorDistanceShader2.h>
#include <webAsmPlay/ImGuiInclude.h>
#include <webAsmPlay/GUI.h>

using namespace std;
using namespace glm;

namespace
{
    // 0 - default
    // 1 - house
    // 2 - building
    // 3 - grass
    void addSymbologyColorControls(const size_t colorIndex, const string & name, vec4 * colors)
    {
        if(ImGui::ColorEdit4((name + " fill near").c_str(), value_ptr(colors[0]), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            ColorDistanceShader2::setColor(colorIndex * 4 + 0, colors[0]);
        }

        if(ImGui::ColorEdit4((name + " fill far").c_str(), value_ptr(colors[1]), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            ColorDistanceShader2::setColor(colorIndex * 4 + 1, colors[1]);
        }

        if(ImGui::ColorEdit4((name + " outline near").c_str(), value_ptr(colors[2]), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            ColorDistanceShader2::setColor(colorIndex * 4 + 2, colors[2]);
        }

        if(ImGui::ColorEdit4((name + " outline far").c_str(), value_ptr(colors[3]), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            ColorDistanceShader2::setColor(colorIndex * 4 + 3, colors[3]);
        }

        ImGui::Separator();
    }
}

void GUI::attributionPanel()
{
    if(!showAttributionPanel) { return ;}

    ImGui::Begin("GUI Configuration", &showAttributionPanel);

    if (ImGui::CollapsingHeader("Linear Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
    }

    if (ImGui::CollapsingHeader("Polygonal features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static vec4 defaultColors[]  = {ColorDistanceShader2::getColor(0 * 2 + 0), ColorDistanceShader2::getColor(0 * 2 + 1), vec4(0,1,0,1), vec4(0,1,0,1) };
        static vec4 houseColors[]    = {ColorDistanceShader2::getColor(1 * 2 + 0), ColorDistanceShader2::getColor(1 * 2 + 1), vec4(0,1,0,1), vec4(0,1,0,1) };
        static vec4 buildingColors[] = {ColorDistanceShader2::getColor(2 * 2 + 0), ColorDistanceShader2::getColor(2 * 2 + 1), vec4(0,1,0,1), vec4(0,1,0,1) };
        static vec4 grassColors[]    = {ColorDistanceShader2::getColor(3 * 2 + 0), ColorDistanceShader2::getColor(3 * 2 + 1), vec4(0,1,0,1), vec4(0,1,0,1) };

        addSymbologyColorControls(0, "Default",  defaultColors);
        addSymbologyColorControls(1, "House",    houseColors);
        addSymbologyColorControls(2, "Building", buildingColors);
        addSymbologyColorControls(3, "Grass",    grassColors);
    }

    ImGui::End();
}