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
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/GUI/GUI.h>

#include <webAsmPlay/FrameBuffer.h>

using namespace std;
using namespace glm;
using namespace ctpl;
using namespace boostGeom;

namespace
{
	Bubble		* a_bubble		= nullptr;
	Renderable	* a_renderable	= nullptr;

	vector<GLuint> a_textIDs(6);

	thread_pool a_tileLoader(1);

	bool a_clickToViewBubble = false;
}

void GUI::streetSidePanel()
{
	if(!s_showStreetSidePanel) { return ;}

	ImGui::Begin("Bing StreetSide", &s_showStreetSidePanel);

	ImGui::Checkbox("Click to view Bubble", &a_clickToViewBubble);

	if(a_bubble)
	{
		ImGui::Text(("        ID: " + toStr(a_bubble->m_ID)).c_str());
		ImGui::Text(("       Pos: " + toStr(a_bubble->m_pos)).c_str());
		ImGui::Text(("Roll/Pitch: " + toStr(a_bubble->m_rollPitch)).c_str());
		ImGui::Text(("  Altitude: " + toStr(a_bubble->m_altitude)).c_str());

		static FrameBuffer ** fb = nullptr;

		const size_t bubbleFaceSize = 256 * 4;

		if(!fb)
		{
			glfwMakeContextCurrent(getMainWindow()); // TODO Try to make this more clean.

			// TODO Memory leak!
			fb = new FrameBuffer*[6];

			for(size_t i = 1; i < 6; ++i)
			{
				fb[i] = new FrameBuffer({bubbleFaceSize, bubbleFaceSize},
										{ FB_Component(GL_COLOR_ATTACHMENT0, GL_RGBA32F,
											{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
												TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)})});
			}
		}

		for(size_t i = 1; i < 6; ++i)
		{
			ImGui::Image((ImTextureID)BubbleFaceRender::renderBubbleFace(fb[i], a_bubble, i), ImVec2(bubbleFaceSize, bubbleFaceSize));
		}

		/*
		for(size_t i = 1; i < 6; ++i)
		{
			const auto tex = a_bubble->getCachedCubeFaceTexture(i);

			ImGui::Image((ImTextureID)tex, ImVec2(256, 256));
		}
		*/

	}
	ImGui::End();
}

void GUI::initBingStreetSidePanel(const dmat4 & trans)
{
	const auto requestBubbleFaces = [](const dvec3 & posWC)
	{
		const auto pos = getClient()->getInverseTrans() * dvec4(posWC, 1);

		tie(a_bubble, a_renderable) = StreetSide::closestBubble(pos);

		if(!a_bubble) { return ;}

		static unordered_set<Bubble *> loadingBubbles;

		if(loadingBubbles.find(a_bubble) != loadingBubbles.end()) { return ;}

		loadingBubbles.insert(a_bubble);

		auto bubble = a_bubble;

		for(size_t i = 0; i < 6; ++i) { bubble->requestCubeFaceTexture(i) ;}
	};

	getMainCanvas()->addLeftClickListener([requestBubbleFaces](const dvec3 & posWC)
	{
		switch(getMode())
		{
			case PICK_STREET_SIDE_BUBBLE:
				
				if(!a_clickToViewBubble || !s_showStreetSidePanel) {  break ;}

				requestBubbleFaces(posWC);

			break;

			case PICK_BING_TILE:

				for(auto bubble : StreetSide::query(getClient()->getInverseTrans() * dvec4(posWC, 1)))
				{
					const auto trans = getClient()->getTrans();

					auto b = buffer({bubble->m_pos.y, bubble->m_pos.x}, 0.00001);

					auto r = Renderable::create(b, trans);

					getMainCanvas()->addRenderable(r);

					StreetSide::indexBubble(bubble, r);
				}

				break;
		}
	});

	getMainCanvas()->addMouseMoveListener([requestBubbleFaces](const dvec3 & posWC)
	{
		if(!s_showStreetSidePanel || a_clickToViewBubble) { return ;}

		requestBubbleFaces(posWC);
	});
}