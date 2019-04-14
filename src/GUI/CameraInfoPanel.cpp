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
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace glm;

void GUI::cameraInfoPanel()
{
    if(!s_showCameraInfoPanel) { return ;}

    ImGui::Begin("Camera", &s_showCameraInfoPanel);

    if(ImGui::CollapsingHeader("Orientation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(("   Eye: " + toStr(s_canvas->getCamera()->getEyeConstRef()))   .c_str());
        ImGui::Text(("Center: " + toStr(s_canvas->getCamera()->getCenterConstRef())).c_str());
        ImGui::Text(("    Up: " + toStr(s_canvas->getCamera()->getUpConstRef()))    .c_str());
    }

    if(ImGui::CollapsingHeader("View Matrix", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(toStr(s_canvas->getViewRef()).c_str());
    }

    if(ImGui::CollapsingHeader("MVP Matrix", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text(toStr(s_canvas->getMVP_Ref()).c_str());
    }

	if(ImGui::CollapsingHeader("Cursor Position", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text(("Local pos: " + toStr(vec3(s_canvas->getCursorPosWC()))).c_str());
	}

    ImGui::End();
}