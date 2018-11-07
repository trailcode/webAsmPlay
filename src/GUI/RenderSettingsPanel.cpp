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
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GeosTestCanvas.h>
#include <webAsmPlay/Renderable.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

void GUI::renderSettingsPanel()
{
    if(!showRenderSettingsPanel) { return ;}

    ImGui::Begin("Render Settings", &showRenderSettingsPanel);

        static bool fillPolygons           = true;
        static bool renderPolygonOutlines  = true;
        static bool renderLinearFeatures   = true;
        static bool renderSkyBox           = true;

        if(ImGui::Checkbox("Fill Polygons", &fillPolygons))
        {
            dmess("Fix!");
            /*
            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderFill(fillPolygons) ;}

            Renderable::setDefaultRenderFill(fillPolygons);
            */
        }

        if(ImGui::Checkbox("Polygon Outlines", &renderPolygonOutlines))
        {
            dmess("Fix!");

            /*
            for(Renderable * r : canvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            for(Renderable * r : geosTestCanvas->getRenderiablesRef()) { r->setRenderOutline(renderPolygonOutlines) ;}

            Renderable::setDefaultRenderOutline(renderPolygonOutlines);
            */
        }

        if(ImGui::Checkbox("Linear Features", &renderLinearFeatures))
        {
            dmess("Linear Features");
        }

        if(ImGui::Checkbox("SkyBox", &renderSkyBox))
        {
            if(renderSkyBox) { canvas->setSkyBox(skyBox) ;}
            else             { canvas->setSkyBox(NULL)   ;}
        }
        
        ImGui::Spacing();

    ImGui::End();
}