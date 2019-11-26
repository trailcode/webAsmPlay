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
#include <webAsmPlay/canvas/BubbleFaceTestCanvas.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

void GUI::bubbleFaceTestPanel()
{
	s_bubbleFaceTestCanvas->setEnabled(s_showBubbleFaceTestPanel);

	if(!s_showBubbleFaceTestPanel) { return ;}

	ImGui::Begin("Bubble Face Test", &s_showBubbleFaceTestPanel);

        const ImVec2 pos = ImGui::GetCursorScreenPos();

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		s_bubbleFaceTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));

        s_bubbleFaceTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

        ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)s_bubbleFaceTestCanvas->render(),
                                                pos,
                                                ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                ImVec2(0, 1),
                                                ImVec2(1, 0));

		//ImGui::GetWindowDrawList()->AddText( ImVec2(pos.x + 60, pos.y + 60), ImColor(1.0f,1.0f,1.0f,1.0f), "Text in Background Layer" );
	
	/*
	auto dl = ImGui::GetWindowDrawList();

	dl->AddText(ImVec2(100,100),  ImGui::ColorConvertFloat4ToU32(ImVec4(1.f,1.f,1.f,1)), "Hello World");
	*/
	
    ImGui::End();
	/*
	ImGui::SetNextWindowPos( ImVec2(0,0) );
	ImGui::Begin("BCKGND", NULL, ImGui::GetIO().DisplaySize, 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus );
	ImGui::GetWindowDrawList()->AddText( ImVec2(400,400), ImColor(1.0f,1.0f,1.0f,1.0f), "Text in Background Layer" );
	ImGui::End();
	*/
}