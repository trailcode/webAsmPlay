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

#include <boost/python.hpp>
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleFaceRender.h>
#include <webAsmPlay/bing/StreetSide.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/VertexArrayObject.h>
#include <webAsmPlay/Python.h>
#include <webAsmPlay/ImageFeatures.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;
using namespace glm;

namespace
{
	array<FrameBuffer *, 6> a_frameBuffers;

	const size_t a_bubbleFaceSize = 256 * 4;

	bool a_autoDetectFeratures[6] = {false, false, false, false, false, false};
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

extern float g_featureConfidence;

void GUI::bubbleFacePanels()
{
	char buf[1024];

	for(size_t i = 0; i < 6; ++i)
	{
		if(!s_showBubbleFacePanel[i] || !StreetSide::closestBubble()) { continue ;}

		const string title = "Bubble face: " + Bubble::s_faceNames[i];

		ImGui::Begin(title.c_str(), &s_showBubbleFacePanel[i]);
		{
			const ImVec2 pos = ImGui::GetCursorScreenPos();

			const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

			a_frameBuffers[i]->setBufferSize(__(sceneWindowSize));

			const auto texID = BubbleFaceRender::renderBubbleFace(a_frameBuffers[i], StreetSide::closestBubble(), i);

			const auto imageID = "bing_" + StreetSide::closestBubble()->getQuadKey() + "_" + toStr(i);

			const ImageFeatures * features = nullptr;

			if(features = ImageFeatures::getFeatures(imageID))
			{
				for(size_t i = 0; i < features->m_scores.size(); ++i)
				{
					//if(features->m_scores[i] < 0.5) { break ;}
					if(features->m_scores[i] < g_featureConfidence) { continue ;} // Are they sorted?

					const auto data = (const float *)features->m_bounds[i].get_data();

					const auto min = vec3(data[1] * 2.0, (1.0 - data[0]) * 2.0, 0);
					const auto max = vec3(data[3] * 2.0, (1.0 - data[2]) * 2.0, 0);

					DeferredRenderable::addLine({min.x, max.y}, {max.x, max.y}, {1, 1, 1, 1}, DEFER_FEATURES);
					DeferredRenderable::addLine({max.x, max.y}, {max.x, min.y}, {1, 1, 1, 1}, DEFER_FEATURES);
					DeferredRenderable::addLine({max.x, min.y}, {min.x, min.y}, {1, 1, 1, 1}, DEFER_FEATURES);
					DeferredRenderable::addLine({min.x, min.y}, {min.x, max.y}, {1, 1, 1, 1}, DEFER_FEATURES);
				}

				a_frameBuffers[i]->bind(false);

				static DeferredRenderable * r = nullptr;
				
				const auto transa = glm::translate(dmat4(1.0), dvec3(-1,-1,0));

				if(!r)	{ r = DeferredRenderable::createFromQueued(DEFER_FEATURES, transa) ;}
				else	{ r->setFromQueued(DEFER_FEATURES, transa);}

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);

				const auto projection = glm::ortho(-1.f, 1.f, 1.f, -1.f, -10.0f, 10.0f);
				const auto view = lookAt(vec3(0,0,1),vec3(0,0,0),vec3(0,1,0));
				const auto model = mat4(1.0);

				r->render(model, view, projection, POST_G_BUFFER);

				a_frameBuffers[i]->unbind();
			}

			ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)texID,
													pos,
													ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y));

			ImGui::Checkbox("Auto Detect", &a_autoDetectFeratures[i]);														

			if(!features)
			{
				if(ImGui::Button("Copy") || a_autoDetectFeratures[i])
				{
					dmess("texID " << texID << " " << StreetSide::closestBubble()->getQuadKey() << " " << i);

					dmess(Python::execute("detectObjects(" + toStr((size_t)texID) + ", '" + imageID + "')"));
				}
			}
		}
		ImGui::End();
	}
}