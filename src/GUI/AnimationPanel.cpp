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
\copyright 2019
*/

#include <webAsmPlay/Util.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/canvas/AnimationCanvas.h>
#include <webAsmPlay/GUI/GUI.h>

void GUI::animationPanel()
{
	s_animationCanvas->setEnabled(s_showAnimationPanel);

	if (!s_showAnimationPanel) { return; }

	ImGui::Begin("Animation", &s_showAnimationPanel);
	{
		const ImVec2 pos = ImGui::GetCursorScreenPos();		

		const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		s_animationCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));

		s_animationCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

		ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)s_animationCanvas->render(),
												pos,
												ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
												ImVec2(0, 1),
												ImVec2(1, 0));

		dmess("ImGui::IsItemActive() " << ImGui::IsItemActive());
	}
	ImGui::End();
}