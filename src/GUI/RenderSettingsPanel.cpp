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
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/renderables/Renderable.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

void GUI::renderSettingsPanel()
{
    if(!showRenderSettingsPanel) { return ;}

    ImGui::Begin("Render Settings", &showRenderSettingsPanel);

        if(ImGui::Checkbox("Fill Meshes", &renderSettingsFillMeshes))
        {
            for(Renderable * r : canvas->getMeshesRef()) { r->setRenderFill(renderSettingsFillMeshes) ;}
        }

        if(ImGui::Checkbox("Mesh Outlines", &renderSettingsRenderMeshOutlines))
        {
            for(Renderable * r : canvas->getMeshesRef()) { r->setRenderOutline(renderSettingsRenderMeshOutlines) ;}
        }

        if(ImGui::Checkbox("Fill Polygons", &renderSettingsFillPolygons))
        {
            for(Renderable * r : canvas->getPolygonsRef()) { r->setRenderFill(renderSettingsFillPolygons) ;}

            for(Renderable * r : geosTestCanvas->getPolygonsRef()) { r->setRenderFill(renderSettingsFillPolygons) ;}
        }

        if(ImGui::Checkbox("Polygon Outlines", &renderSettingsRenderPolygonOutlines))
        {
            for(Renderable * r : canvas->getPolygonsRef()) { r->setRenderOutline(renderSettingsRenderPolygonOutlines) ;}

            for(Renderable * r : geosTestCanvas->getPolygonsRef()) { r->setRenderOutline(renderSettingsRenderPolygonOutlines) ;}
        }

        if(ImGui::Checkbox("Linear Features", &renderSettingsRenderLinearFeatures))
        {
            for(Renderable * r : canvas->getLineStringsRef())
            {
                r->setRenderFill    (renderSettingsRenderLinearFeatures);
                r->setRenderOutline (renderSettingsRenderLinearFeatures);
            }
        }

        if(ImGui::Checkbox("SkyBox", &renderSettingsRenderSkyBox))
        {   
            if(renderSettingsRenderSkyBox) { canvas->setSkyBox(skyBox) ;} // TODO create check render functor
            else                           { canvas->setSkyBox(NULL)   ;}
        }
        
        if(ImGui::Checkbox("BingMaps", &renderSettingsRenderBingMaps))
        {
            for(Renderable * r : canvas->getRastersRef())
            {
                r->setRenderFill(renderSettingsRenderBingMaps);
            }
        }

        //ImGui::Spacing();

        float heightMultiplier = ColorDistanceShader3D::getDefaultInstance()->getHeightMultiplier();

        if(ImGui::SliderFloat("", &heightMultiplier, 0.0f, 5.0f, "Height mult: %.3f"))
        {
            ColorDistanceShader3D     ::getDefaultInstance()->setHeightMultiplier(heightMultiplier);
            ColorDistanceDepthShader3D::getDefaultInstance()->setHeightMultiplier(heightMultiplier);
        }

        const char * items[] = { "TrackBall", "First Person", "Track Entity" };
        
        ImGui::Combo("Camera", &cameraMode, items, IM_ARRAYSIZE(items));

        ImGui::Text("Linestring Shader");

        const char * shaders[] = { "ColorDistanceShader", "ColorDistanceShader3D", "ColorDistanceDepthShader3D", "ColorShader", "ColorVertexShader" };

        static int lineStringShader = 0;

        ImGui::Combo("Linestring", &lineStringShader, shaders, IM_ARRAYSIZE(shaders)); 
        
        static int meshShader = 0;

        if(ImGui::Combo("Mesh", &meshShader, shaders, IM_ARRAYSIZE(shaders)))
        {
            dmess("meshShader " << meshShader);

            vector<Shader *> shaderMap({ColorDistanceShader       ::getDefaultInstance(),
                                        ColorDistanceShader3D     ::getDefaultInstance(),
                                        ColorDistanceDepthShader3D::getDefaultInstance(),
                                        ColorShader               ::getDefaultInstance(),
                                        ColorVertexShader         ::getDefaultInstance()
                                        });

            for(auto i : canvas->getMeshesRef()) { i->setShader(shaderMap[meshShader]) ;}
        }

        //ImGui::SameLine(); ShowHelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");

    ImGui::End();
}

//float GUI::getHeightMultiplier() { return heightMultiplier ;}