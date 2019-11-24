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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
	Bubble		* a_bubble		= nullptr;
	Renderable	* a_renderable	= nullptr;

	vector<GLuint> a_textIDs(6);
}

void GUI::streetSidePanel()
{
	if(!s_showStreetSidePanel) { return ;}

	ImGui::Begin("Bing StreetSide", &s_showStreetSidePanel);

	if(a_bubble)
	{
		ImGui::Text(("        ID: " + toStr(a_bubble->m_ID)).c_str());
		ImGui::Text(("       Pos: " + toStr(a_bubble->m_pos)).c_str());
		ImGui::Text(("Roll/Pitch: " + toStr(a_bubble->m_rollPitch)).c_str());
		ImGui::Text(("  Altitude: " + toStr(a_bubble->m_altitude)).c_str());

		for(size_t i = 0; i < 6; ++i)
		{
			ImGui::Image((ImTextureID)a_textIDs[i], ImVec2(256, 256));
		}

	}
	ImGui::End();
}

void GUI::initBingStreetSidePanel(const dmat4 & trans)
{
	getMainCanvas()->addLeftClickListener([](const dvec3 & posWC)
	{
		if(!s_showStreetSidePanel || getMode() != PICK_STREET_SIDE_BUBBLE) { return ;}

		const auto pos = getClient()->getInverseTrans() * dvec4(posWC, 1);

		//dmess("pos " << pos);

		tie(a_bubble, a_renderable) = StreetSide::query(pos);

		if(!a_bubble) { return ;}

		for(size_t i = 0; i < 6; ++i)
		{
			const auto tex = a_bubble->getCubeFaceTexture(i);

			dmess("tex " << tex);

			a_textIDs[i] = tex;
		}
	});
}