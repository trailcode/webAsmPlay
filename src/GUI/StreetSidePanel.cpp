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

#ifndef __EMSCRIPTEN__
#include <boost/python.hpp>
#endif

#include <ctpl/ctpl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GeoClient.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleTile.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/geom/BoostGeomUtil.h>
#include <webAsmPlay/renderables/RenderablePoint.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/OpenSteerGlue.h>
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

float g_featureConfidence = 0.5;

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
		ImGui::Text(("                ID: " + toStr(bubble->m_ID)).c_str());
		ImGui::Text(("               Pos: " + toStr(bubble->m_pos)).c_str());
		ImGui::Text(("Roll/Pitch/Heading: " + toStr(bubble->m_rollPitchHeading)).c_str());
		ImGui::Text(("          Altitude: " + toStr(bubble->m_altitude)).c_str());
	}

	if(ImGui::CollapsingHeader("Face Panels"))
	{
		for(size_t i = 0; i < 6; ++i) { ImGui::Checkbox(Bubble::s_faceNames[i].c_str(), &s_showBubbleFacePanel[i]) ;}
	}

	if(ImGui::Button("Free Tiles")) { BubbleTile::freeAllTiles() ;}

	static float bubbleCameraHeight = 100.0f;

	const auto bubble = StreetSide::closestBubble();

	static dmat4 lastLookDir(1.0);

	auto cameraToBubble = [bubble](const dmat4 & lookDir)
	{
		lastLookDir = lookDir;

		if(!getClient() || !bubble) { return ;}

		const auto pos = getClient()->getTrans() * dvec4(bubble->m_pos.y, bubble->m_pos.x, -bubbleCameraHeight * 0.0000001, 1);

		const auto rot = rotate(lookDir, radians(bubble->m_rollPitchHeading.z), dvec3(0, 0, 1));

		const auto forward = rot * dvec4(-0.0001, 0, 0, 0);
		
		getMainCamera()->setCenter(pos);
		getMainCamera()->setUp({ 0, 0, -1 });
		getMainCamera()->setEye(pos + forward);

		getMainCamera()->update();
	};

	ImGui::SliderInt("Bubble Level", &BubbleFaceRender::m_level, 1, 4);

	if(ImGui::SliderFloat("Camera Height", &bubbleCameraHeight, 0.0f, 1000.0f)) { cameraToBubble(lastLookDir) ;}

	if (ImGui::Button("Camera to Bubble Front"))	{ cameraToBubble(dmat4(1.0)) ;}
	if (ImGui::Button("Camera to Bubble Back"))		{ cameraToBubble(rotate(dmat4(1.0), radians(180.0), dvec3(0, 0, 1))) ;}
	if (ImGui::Button("Camera to Bubble Right"))	{ cameraToBubble(rotate(dmat4(1.0), radians(90.0), dvec3(0, 0, 1))) ;}
	if (ImGui::Button("Camera to Bubble Left"))		{ cameraToBubble(rotate(dmat4(1.0), radians(-90.0), dvec3(0, 0, 1))) ;}
	if (ImGui::Button("Camera to Bubble Top"))		{ }

	ImGui::SliderFloat("Feature Confidence", &g_featureConfidence, 0.0f, 1.0f);

	ImGui::End();
}

void GUI::initBingStreetSidePanel(const dmat4 & trans)
{
	getMainCanvas()->addLeftClickListener([](const dvec3 & posWC)
	{
		if(s_cameraMode == CAMERA_FOLLOW_ENTITY) { return ;}

		if(!a_clickToViewBubble || !s_showStreetSidePanel) { return ;}

		StreetSide::ensureBubbleCollectionTile(getClient()->getTrans(), getClient()->getInverseTrans() * dvec4(posWC, 1));

		StreetSide::queryClosestBubbles(getClient()->getInverseTrans() * dvec4(posWC, 1), 10);

		getMainCanvas()->removeRenderable(a_closestBubble);

		getMainCanvas()->addRenderable(a_closestBubble = StreetSide::closestBubbleRenderable());
	});

	getMainCanvas()->addMouseMoveListener([](const dvec3 & posWC)
	{
		if(s_cameraMode == CAMERA_FOLLOW_ENTITY) { return ;}

		if(!s_showStreetSidePanel || a_clickToViewBubble) { return ;}

		StreetSide::ensureBubbleCollectionTile(getClient()->getTrans(), getClient()->getInverseTrans() * dvec4(posWC, 1));

		StreetSide::queryClosestBubbles(getClient()->getInverseTrans() * dvec4(posWC, 1), 10);

		getMainCanvas()->removeRenderable(a_closestBubble);

		getMainCanvas()->addRenderable(a_closestBubble = StreetSide::closestBubbleRenderable());
	});

	getMainCanvas()->addPreRenderFunctor([]()
	{
		if(!s_showStreetSidePanel || s_cameraMode != CAMERA_FOLLOW_ENTITY) { return ;}

		StreetSide::ensureBubbleCollectionTile(getClient()->getTrans(), getClient()->getInverseTrans() * OpenSteerGlue::s_cameraTarget);

		StreetSide::queryClosestBubbles(getClient()->getInverseTrans() * OpenSteerGlue::s_cameraTarget, 10);

		getMainCanvas()->removeRenderable(a_closestBubble);

		getMainCanvas()->addRenderable(a_closestBubble = StreetSide::closestBubbleRenderable());
	});

	getMainCanvas()->addKeyListener([](const int key, const int scancode, const int action, const int mods)
	{
		a_clickToViewBubble ^= action && key == GLFW_KEY_B;
	});

	/*
	GUI::addUpdatable([]()
    {

	});
	*/
}