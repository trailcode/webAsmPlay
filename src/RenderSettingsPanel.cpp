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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/ImguiInclude.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/Renderable.h>
#include <webAsmPlay/ColorDistanceShader.h>
#include <webAsmPlay/GUI.h>

using namespace std;
using namespace glm;

void GUI::renderSettingsPanel()
{
    if(!showRenderSettingsPanel) { return ;}

    ImGui::Begin("Render Settings", &showRenderSettingsPanel);

        static bool fillPolygons            = true;
        static bool renderPolygonOutlines   = true;
        static bool renderSkyBox            = true;

        static bool _fillPolygons           = true;
        static bool _renderPolygonOutlines  = true;
        static bool _renderSkyBox           = true;

        ImGui::Checkbox("Fill Polygons",    &_fillPolygons);
        ImGui::Checkbox("Polygon Outlines", &_renderPolygonOutlines);
        ImGui::Checkbox("SkyBox",           &_renderSkyBox);

        if(fillPolygons != _fillPolygons)
        {
            fillPolygons = _fillPolygons;

            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            Renderable::setDefaultRenderFill(fillPolygons);
        }

        if(renderPolygonOutlines != _renderPolygonOutlines)
        {
            renderPolygonOutlines = _renderPolygonOutlines;

            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            Renderable::setDefaultRenderOutline(renderPolygonOutlines);
        }

        if(renderSkyBox != _renderSkyBox)
        {
            renderSkyBox = _renderSkyBox;

            if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
            else             { canvas->setSkyBox(NULL)   ;}
        }

        static vec4 fillColor(Renderable::getDefaultFillColor());
        static vec4 outlineColor(Renderable::getDefaultOutlineColor());
        
        /*
        ImGui::ColorPicker4("##picker",
                            (float*)&fillColor,
                            //ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                            0);
                            */

        ImGui::Spacing();

        if(ImGui::CollapsingHeader("Line strings"))
        {
            static float minDist = ColorDistanceShader::getMinDist();
            static float maxDist = ColorDistanceShader::getMaxDist();
            
            if (ImGui::SliderFloat("Min", &minDist, -10.0f, 30.0f,  "%.0f")) { ColorDistanceShader::setMinDist(minDist) ;}
            if (ImGui::SliderFloat("Max", &maxDist, 0.0f,   100.0f, "%.0f")) { ColorDistanceShader::setMaxDist(maxDist) ;}
            
            static ImGuiColorEditFlags alpha_flags = 0;

            ImGui::RadioButton("Opaque", &alpha_flags, 0);                                  ImGui::SameLine();
            ImGui::RadioButton("Alpha",  &alpha_flags, ImGuiColorEditFlags_AlphaPreview);   ImGui::SameLine();
            ImGui::RadioButton("Both",   &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf);

            static ImVec4 minColor = __(ColorDistanceShader::getMinColor());
            static ImVec4 maxColor(0,0.5,0.5,0.5);

            if(ImGui::ColorEdit4("Min", (float*)&minColor, ImGuiColorEditFlags_AlphaBar | alpha_flags))
            {
                ColorDistanceShader::setMinColor(__(minColor));
            }

            if(ImGui::ColorEdit4("Max", (float*)&maxColor, ImGuiColorEditFlags_AlphaBar | alpha_flags))
            {
                ColorDistanceShader::setMaxColor(__(maxColor));
            }
        }

    ImGui::End();
}