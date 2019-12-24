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

#include <array>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/Python.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;

namespace
{
	array<FrameBuffer *, 6> a_frameBuffers;

	const size_t a_bubbleFaceSize = 256 * 4;
}

void GUI::initBubbleFacePanels()
{
	for(size_t i = 0; i < 6; ++i)
	{
		a_frameBuffers[i] = new FrameBuffer({a_bubbleFaceSize, a_bubbleFaceSize},
											{ FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,
												{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
													TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)})});
	}
}

void GUI::bubbleFacePanels()
{
	char buf[1024];

	for(size_t i = 0; i < 6; ++i)
	{
		if(!s_showBubbleFacePanel[i]) { continue ;}

		const string title = "Bubble face: " + Bubble::s_faceNames[i];

		ImGui::Begin(title.c_str(), &s_showBubbleFacePanel[i]);
		{
			const ImVec2 pos = ImGui::GetCursorScreenPos();

			const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

			a_frameBuffers[i]->setBufferSize(__(sceneWindowSize));

			const auto texID = BubbleFaceRender::renderBubbleFace(a_frameBuffers[i], StreetSide::closestBubble(), i);

			ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)texID,
													pos,
													ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y));

			if(ImGui::Button("Copy"))
			{
				dmess("texID " << texID);

				dmess(Python::execute("detectObjects(" + toStr((size_t)texID) + ")"));
			}
		}
		ImGui::End();
	}
}