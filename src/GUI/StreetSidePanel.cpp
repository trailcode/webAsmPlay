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

#include <ctpl/ctpl.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleTile.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;
using namespace ctpl;
using namespace boostGeom;

namespace
{
	bool a_clickToViewBubble = false;

	Renderable * a_closestBubble = nullptr;
}

void GUI::streetSidePanel()
{
	if(!s_showStreetSidePanel) { return ;}

	ImGui::Begin("Bing StreetSide", &s_showStreetSidePanel);

	ImGui::Checkbox("Click to view Bubble", &a_clickToViewBubble);

	ImGui::Text(("    Tiles Loading: " + toStr(BubbleTile::getNumLoading())).c_str());
	ImGui::Text(("Tiles Downloading: " + toStr(BubbleTile::getNumDownloading())).c_str());
	ImGui::Text(("        Num tiles: " + toStr(BubbleTile::getNumTiles())).c_str());

	if(const auto bubble = StreetSide::closestBubble())
	{
		ImGui::Text(("        ID: " + toStr(bubble->m_ID)).c_str());
		ImGui::Text(("       Pos: " + toStr(bubble->m_pos)).c_str());
		ImGui::Text(("Roll/Pitch: " + toStr(bubble->m_rollPitch)).c_str());
		ImGui::Text(("  Altitude: " + toStr(bubble->m_altitude)).c_str());
	}

	if(ImGui::CollapsingHeader("Face Panels"))
	{
		for(size_t i = 0; i < 6; ++i) { ImGui::Checkbox(Bubble::s_faceNames[i].c_str(), &s_showBubbleFacePanel[i]) ;}
	}

	if(ImGui::Button("Free Tiles")) { BubbleTile::freeAllTiles() ;}

	ImGui::End();
}

void GUI::initBingStreetSidePanel(const dmat4 & trans)
{
	getMainCanvas()->addLeftClickListener([](const dvec3 & posWC)
	{
		switch(getMode())
		{
			case PICK_STREET_SIDE_BUBBLE:
				
				if(!a_clickToViewBubble || !s_showStreetSidePanel) {  break ;}

				 StreetSide::queryClosestBubbles(getClient()->getInverseTrans() * dvec4(posWC, 1), 10);

			break;

			case PICK_BING_TILE:

				StreetSide::ensureBubbleCollectionTile(getClient()->getTrans(), getClient()->getInverseTrans() * dvec4(posWC, 1));

				const auto trans = getClient()->getTrans();

				/*
				for(auto bubble : StreetSide::query(getClient()->getInverseTrans() * dvec4(posWC, 1)))
				{
					const auto trans = getClient()->getTrans();

					auto b = buffer({bubble->m_pos.y, bubble->m_pos.x}, 0.00001);

					auto r = Renderable::create(b, trans);

					//getMainCanvas()->addRenderable(r);

					StreetSide::indexBubble(bubble, r);
				}
				*/

				break;
		}
	});

	getMainCanvas()->addMouseMoveListener([](const dvec3 & posWC)
	{
		if(!s_showStreetSidePanel || a_clickToViewBubble) { return ;}

		StreetSide::ensureBubbleCollectionTile(getClient()->getTrans(), getClient()->getInverseTrans() * dvec4(posWC, 1));

		StreetSide::queryClosestBubbles(getClient()->getInverseTrans() * dvec4(posWC, 1), 10);

		getMainCanvas()->removeRenderable(a_closestBubble);

		getMainCanvas()->addRenderable(a_closestBubble = StreetSide::closestBubbleRenderable());
	});

	
}