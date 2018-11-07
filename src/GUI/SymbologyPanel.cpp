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
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceShader2.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

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
        ColorDistanceShader2 * shader = ColorDistanceShader2::getDefaultInstance();

        const ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoInputs;

        //ImGui::ColorPicker4((name + " fill neara").c_str(), value_ptr(colors[0]), flags);

        /*
        ImVec4 c(colors[0].x, colors[0].y, colors[0].z, colors[0].w);

        if(ImGui::ColorButton((name + " fill neara").c_str(), c, flags, ImVec2(20,20)))
        {
            ImGui::ColorPicker4((name + " fill neara").c_str(), value_ptr(colors[0]), flags);
        }
        */

        if(ImGui::ColorEdit4((name + " fill near").c_str(), value_ptr(colors[0]), flags))
        {
            shader->setColor(colorIndex * 4 + 0, colors[0]);
        }

        if(ImGui::ColorEdit4((name + " fill far").c_str(), value_ptr(colors[1]), flags))
        {
            shader->setColor(colorIndex * 4 + 1, colors[1]);
        }

        if(ImGui::ColorEdit4((name + " outline near").c_str(), value_ptr(colors[2]), flags))
        {
            shader->setColor(colorIndex * 4 + 2, colors[2]);
        }

        if(ImGui::ColorEdit4((name + " outline far").c_str(), value_ptr(colors[3]), flags))
        {
            shader->setColor(colorIndex * 4 + 3, colors[3]);
        }

        ImGui::Separator();
    }
}

void GUI::symbologyPanel()
{
    if(!showSymbologyPanel) { return ;}

    ImGui::Begin("Symbology", &showSymbologyPanel);

    if (ImGui::CollapsingHeader("Linear Features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ColorDistanceShader * shader = ColorDistanceShader::getDefaultInstance();

        static float minDist = shader->getMinDist();
        static float maxDist = shader->getMaxDist();
        
        static ImVec4 minColor = __(shader->getMinColor());
        static ImVec4 maxColor = __(shader->getMaxColor());

        if(ImGui::ColorEdit4("Min", (float*)&minColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            shader->setMinColor(__(minColor));
        }

        if(ImGui::ColorEdit4("Max", (float*)&maxColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            shader->setMaxColor(__(maxColor));
        }
    }

    if (ImGui::CollapsingHeader("Polygonal features", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ColorDistanceShader2 * shader = ColorDistanceShader2::getDefaultInstance();

        addSymbologyColorControls(0, "Default",  &shader->getColorRef(0 * 4 + 0));
        addSymbologyColorControls(1, "House",    &shader->getColorRef(1 * 4 + 0));
        addSymbologyColorControls(2, "Building", &shader->getColorRef(2 * 4 + 0));
        addSymbologyColorControls(3, "Grass",    &shader->getColorRef(3 * 4 + 0));
        addSymbologyColorControls(3, "water",    &shader->getColorRef(4 * 4 + 0));
    }

    ImGui::End();
}