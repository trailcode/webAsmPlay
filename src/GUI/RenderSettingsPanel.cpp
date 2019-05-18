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
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

void GUI::renderSettingsPanel()
{
    if(!s_showRenderSettingsPanel) { return ;}

    ImGui::Begin("Render Settings", &s_showRenderSettingsPanel);

        if(ImGui::Checkbox("Fill Meshes", &s_renderSettingsFillMeshes))
        {
            for(Renderable * r : s_canvas->getMeshesRef()) { r->setRenderFill(s_renderSettingsFillMeshes) ;}
        }

        if(ImGui::Checkbox("Mesh Outlines", &s_renderSettingsRenderMeshOutlines))
        {
            for(Renderable * r : s_canvas->getMeshesRef()) { r->setRenderOutline(s_renderSettingsRenderMeshOutlines) ;}
        }

        if(ImGui::Checkbox("Fill Polygons", &s_renderSettingsFillPolygons))
        {
            for(Renderable * r : s_canvas->getPolygonsRef()) { r->setRenderFill(s_renderSettingsFillPolygons) ;}

            for(Renderable * r : s_geosTestCanvas->getPolygonsRef()) { r->setRenderFill(s_renderSettingsFillPolygons) ;}
        }

        if(ImGui::Checkbox("Polygon Outlines", &s_renderSettingsRenderPolygonOutlines))
        {
            for(Renderable * r : s_canvas->getPolygonsRef()) { r->setRenderOutline(s_renderSettingsRenderPolygonOutlines) ;}

            for(Renderable * r : s_geosTestCanvas->getPolygonsRef()) { r->setRenderOutline(s_renderSettingsRenderPolygonOutlines) ;}
        }

        if(ImGui::Checkbox("Linear Features", &s_renderSettingsRenderLinearFeatures))
        {
            for(Renderable * r : s_canvas->getLineStringsRef())
            {
                r->setRenderFill    (s_renderSettingsRenderLinearFeatures);
                r->setRenderOutline (s_renderSettingsRenderLinearFeatures);
            }
        }

        if(ImGui::Checkbox("SkyBox", &s_renderSettingsRenderSkyBox))
        {   
            if(s_renderSettingsRenderSkyBox)	{ s_canvas->setSkyBox(s_skyBox) ;} // TODO create check render functor
            else								{ s_canvas->setSkyBox(NULL)     ;}
        }
        
        if(ImGui::Checkbox("BingMaps", &s_renderSettingsRenderBingMaps))
        {
            for(Renderable * r : s_canvas->getRastersRef())
            {
                r->setRenderFill(s_renderSettingsRenderBingMaps);
            }
        }

        //ImGui::Spacing();

        float heightMultiplier = ColorDistanceDepthShader3D::getDefaultInstance()->getHeightMultiplier();

        if(ImGui::SliderFloat("", &heightMultiplier, 0.0f, 1.0f, "Height mult: %.3f"))
        {
            ColorDistanceDepthShader3D::getDefaultInstance()->setHeightMultiplier(heightMultiplier);
        }

        const char * items[] = { "TrackBall", "First Person", "Track Entity" };
        
        ImGui::Combo("Camera", &s_cameraMode, items, IM_ARRAYSIZE(items));

        //ImGui::Text("Linestring Shader");

        const char * shaders[] = { "ColorDistanceShader", "ColorDistanceDepthShader3D", "ColorShader", "ColorVertexShader" };

        static int lineStringShader = 0;

        ImGui::Combo("Linestring", &lineStringShader, shaders, IM_ARRAYSIZE(shaders)); 
        
        static int meshShader = 1;

        if(ImGui::Combo("Mesh", &meshShader, shaders, IM_ARRAYSIZE(shaders)))
        {
            dmess("meshShader " << meshShader);

            vector<Shader *> shaderMap({ColorDistanceShader       ::getDefaultInstance(),
                                        ColorDistanceDepthShader3D::getDefaultInstance(),
                                        ColorShader               ::getDefaultInstance(),
                                        ColorVertexShader         ::getDefaultInstance()
                                        });

            for(auto i : s_canvas->getMeshesRef()) { i->setShader(shaderMap[meshShader]) ;}
        }

		if(ImGui::SliderFloat("LineWidth", &s_lineWidthRender, 0.0f, 5.0f, "Line width: %.3f"))
        {
			dmess("lineWidthRender " << s_lineWidthRender);
		}

		if(s_canvas)
		{
			float perspectiveFOV = s_canvas->getPerspectiveFOV();

			if(ImGui::SliderFloat("FOV", &perspectiveFOV, 30.0f, 50.0f, "%.3f")) // TODO Why does this not work?
			{
				s_canvas->setPerspectiveFOV(perspectiveFOV);
			}
		}
        //ImGui::SameLine(); ShowHelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n");

    ImGui::End();
}

//float GUI::getHeightMultiplier() { return heightMultiplier ;}