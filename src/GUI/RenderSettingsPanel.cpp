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
#include <webAsmPlay/canvas/GeosTestCanvas.h>
#include <webAsmPlay/renderables/Renderable.h>
#include <webAsmPlay/shaders/ColorDistanceShader.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/shaders/SsaoShader.h>
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
            for(auto r : s_canvas->getMeshesRef()) { r->setRenderFill(s_renderSettingsFillMeshes) ;}
        }

        if(ImGui::Checkbox("Mesh Outlines", &s_renderSettingsRenderMeshOutlines))
        {
            for(auto r : s_canvas->getMeshesRef()) { r->setRenderOutline(s_renderSettingsRenderMeshOutlines) ;}
        }

        if(ImGui::Checkbox("Fill Polygons", &s_renderSettingsFillPolygons))
        {
            for(auto r : s_canvas->getPolygonsRef()) { r->setRenderFill(s_renderSettingsFillPolygons) ;}

            for(auto r : s_geosTestCanvas->getPolygonsRef()) { r->setRenderFill(s_renderSettingsFillPolygons) ;}
        }

        if(ImGui::Checkbox("Polygon Outlines", &s_renderSettingsRenderPolygonOutlines))
        {
            for(auto r : s_canvas->getPolygonsRef()) { r->setRenderOutline(s_renderSettingsRenderPolygonOutlines) ;}

            for(auto r : s_geosTestCanvas->getPolygonsRef()) { r->setRenderOutline(s_renderSettingsRenderPolygonOutlines) ;}
        }

        if(ImGui::Checkbox("Linear Features", &s_renderSettingsRenderLinearFeatures))
        {
            for(auto r : s_canvas->getLineStringsRef())
            {
                r->setRenderFill    (s_renderSettingsRenderLinearFeatures);
                r->setRenderOutline (s_renderSettingsRenderLinearFeatures);
            }
        }

        if (ImGui::Checkbox("Models", &s_renderSettingsRenderModels))
        {
            for (auto r : s_canvas->getModelsRef())
            {
                r->setRenderFill(s_renderSettingsRenderModels);
            }
        }

        if(ImGui::Checkbox("SkyBox", &s_renderSettingsRenderSkyBox))
        {   
            if(s_renderSettingsRenderSkyBox)	{ s_canvas->setSkyBox(s_skyBox) ;} // TODO create check render functor
            else								{ s_canvas->setSkyBox(nullptr)  ;}
        }
        
        if(ImGui::Checkbox("BingMaps", &s_renderSettingsRenderBingMaps))
        {
            for(auto r : s_canvas->getRastersRef()) { r->setRenderFill(s_renderSettingsRenderBingMaps) ;}
        }

        float heightMultiplier = ColorDistanceDepthShader3D::getDefaultInstance()->getHeightMultiplier();

        if(ImGui::SliderFloat("", &heightMultiplier, 0.0f, 1.0f, "Height mult: %.3f"))
        {
            ColorDistanceDepthShader3D::getDefaultInstance()->setHeightMultiplier(heightMultiplier);
        }

        const char * items[] = { "TrackBall", "First Person", "Track Entity" };
        
        ImGui::Combo("Camera", &s_cameraMode, items, IM_ARRAYSIZE(items));

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
			float perspectiveFOV = float(s_canvas->getPerspectiveFOV());

			// 43.994f Seems to be the smallest value before things get strange.
			// 46.7f Seems to be the max.
			//if(ImGui::SliderFloat("FOV", &perspectiveFOV, 43.994f, 46.7f, "%.3f"))
			if(ImGui::SliderFloat("FOV", &perspectiveFOV, 43.994f, 90.0f, "%.3f"))
			{
				s_canvas->setPerspectiveFOV(perspectiveFOV);
			}
		}

		if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float ssaoRadius = SsaoShader::getDefaultInstance()->getSSAO_Radius() * 4000.0f;
			int   numPoints  = SsaoShader::getDefaultInstance()->getNumPoints();
			float minDepth   = SsaoShader::getDefaultInstance()->getMinDepth() * 1000.0f;
			float mixPercent = SsaoShader::getDefaultInstance()->getMixPercent() * 100.0f;

			if (ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.0f, 50.0f, "SSAO Radius: %.3f"))
			{
				SsaoShader::getDefaultInstance()->setSSAO_Radius(ssaoRadius / 4000.0f);
			}

			if (ImGui::SliderInt("Num Points:", &numPoints, 1, 256))
			{
				SsaoShader::getDefaultInstance()->setNumPoints(numPoints);
			}

			if (ImGui::SliderFloat("Min Depth", &minDepth, 0.0f, 50.0f, "Min Depth: %.3f"))
			{
				SsaoShader::getDefaultInstance()->setMinDepth(minDepth / 1000.0f);
			}

			if (ImGui::SliderFloat("Mix Percent", &mixPercent, 0.0f, 100.0f, "Mix Percent: %.3f"))
			{
				SsaoShader::getDefaultInstance()->setMixPercent(mixPercent / 100.0f);
			}
		}

    ImGui::End();
}

void GUI::initRenderSettingsPanel()
{
	getMainCanvas()->addKeyListener([](const int key, const int scancode, const int action, const int mods)
	{
		//a_clickToViewBubble ^= action && key == GLFW_KEY_B;
		if(!action || key != GLFW_KEY_C) { return ;}

		s_cameraMode = (s_cameraMode + 1) % 3;
	});
}