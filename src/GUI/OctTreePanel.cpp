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

#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/canvas/TrackBallInteractor.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace glm;

namespace
{
	void doInit(Canvas * canvas, SkyBox * skyBox)
	{
		static bool didInit = false;

		if(didInit)
			return;

		didInit = true;

		canvas->getTrackBallInteractor()->setSpeed(5.0);

		canvas->getTrackBallInteractor()->setZoomScale(0.1 / 5.0);
		canvas->getTrackBallInteractor()->setPanScale(0.005 / 5.0);

		canvas->setSkyBox(skyBox);
	}
}

void GUI::octTreePanel()
{
	if(!s_showOctTreePanel)
		return;

	doInit(s_octTreeCanvas, s_skyBox);

	ImGui::Begin("Oct Tree Demo", &s_showOctTreePanel);

		const ImVec2 pos = ImGui::GetCursorScreenPos();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();

		const auto startPos = ImVec2(vMin.x + pos.x, vMin.y + pos.y);

		//dmess("Posa " << startPos.x << "," << startPos.y);

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		s_octTreeCanvas->setFrameBufferSize(__(sceneWindowSize), __(startPos) - __(pos));

        s_octTreeCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

		static const auto trans = scale(dmat4(1.0), {0.001, 0.001, 0.001});

		static DeferredRenderable * r3 = nullptr; 
		
		if(!r3)
		{
			r3 = DeferredRenderable::createFromQueued(DEFER_FEATURES, trans);

			s_CSG_HSR_Canvas->addRenderable(r3);
		}
		
		r3->setFromQueued(DEFER_FEATURES, trans);

		//a_geoms.push_back(unique_ptr<Renderable>(r3));

		ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)s_octTreeCanvas->render(),
                                                pos,
                                                ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                ImVec2(0, 1),
                                                ImVec2(1, 0));

	ImGui::End();
}

