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
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/canvas/GeosTestCanvas.h>
#include <webAsmPlay/FrameBuffer.h>

void GUI::geosTestPanel()
{
    s_geosTestCanvas->setEnabled(s_showGeosTestPanel);

    if(!s_showGeosTestPanel) { return ;}
    
    ImGui::Begin("Geos Tests", &s_showGeosTestPanel);

		// Yellow is content region min/max
		
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();

			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;
			vMax.x += ImGui::GetWindowPos().x;
			vMax.y += ImGui::GetWindowPos().y;

			ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 255, 0, 255 ) );
		

        const ImVec2 pos = ImGui::GetCursorScreenPos();

		//

		const auto startPos = vMin;

		//dmess("Posa " << startPos.x << "," << startPos.y);

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		//s_geosTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));
		s_geosTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(startPos) - __(pos));

        s_geosTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

        ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)s_geosTestCanvas->render(),
                                                pos,
                                                ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                ImVec2(0, 1),
                                                ImVec2(1, 0));
        
        static float buffer1 = 0.5f;
        static float buffer2 = 0.25f;
        static float buffer3 = 2.0f;
		static float buffer4 = 0.0f;
		static float buffer5 = 0.0f;

        ImGui::SliderFloat("buffer1", &buffer1, 0.01f, 1.5f, "buffer1 = %.3f");
        ImGui::SliderFloat("buffer2", &buffer2, 0.01f, 1.5f, "buffer2 = %.3f");
        ImGui::SliderFloat("buffer3", &buffer3, 0.01f, 3.0f, "buffer3 = %.3f");
		ImGui::SliderFloat("buffer4", &buffer4, -4.0f, 4.0f, "buffer4 = %.3f");
		ImGui::SliderFloat("buffer5", &buffer5, -4.0f, 4.0f, "buffer5 = %.3f");

        s_geosTestCanvas->setGeomParameters(buffer1, buffer2, buffer3, buffer4, buffer5);

		if (ImGui::Button("Export GeoJSON")) { s_geosTestCanvas->exportGeoJson(); }
		
    ImGui::End();
}

