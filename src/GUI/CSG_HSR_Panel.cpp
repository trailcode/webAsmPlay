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
\copyright 2020
*/

// Original code from a course at the GameInstitute. 

#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/GUI/GUI.h>

namespace
{

}

void GUI::CSG_HSR_Panel()
{
	if(!s_showCSG_HSR_Panel) { return ;}

	ImGui::Begin("CSG HSR Demo", &s_showCSG_HSR_Panel);

		const ImVec2 pos = ImGui::GetCursorScreenPos();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();

		const auto startPos = ImVec2(vMin.x + pos.x, vMin.y + pos.y);

		//dmess("Posa " << startPos.x << "," << startPos.y);

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		//s_KD_TreeTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));
		s_CSG_HSR_Canvas->setFrameBufferSize(__(sceneWindowSize), __(startPos) - __(pos));

        s_CSG_HSR_Canvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

	ImGui::End();
}

