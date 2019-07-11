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

#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/shaders/NormalToRGB_Shader.h>
#include <webAsmPlay/shaders/DepthToRGB_Shader.h>
#include <webAsmPlay/GUI/GUI.h>

namespace
{
	void bindFB(FrameBuffer*& fb, const ImVec2 & sceneWindowSize)
	{
		if (!fb)
		{
			fb = new FrameBuffer(	__(sceneWindowSize),
									{FB_Component(GL_COLOR_ATTACHMENT0, GL_RGB32F,				
										{	TexParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST),
											TexParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST)})});
		}

		fb->setBufferSize(__(sceneWindowSize));

		fb->bind();
	}
}

extern GLuint quad_vao;

void GUI::frameBufferDepthDebugPanel()
{
	if (!s_showFrameBufferDepthDebugPanel) { return; }

	ImGui::Begin("FrameBuffer Depth", &s_showFrameBufferDepthDebugPanel);
	{
		const ImVec2 pos = ImGui::GetCursorScreenPos();

		const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		if (s_canvas->getG_FrameBuffer())
		{
			static FrameBuffer* fb = NULL;

			bindFB(fb, sceneWindowSize);

			DepthToRGB_Shader::bind(s_canvas->getG_FrameBuffer()->getTextureID(1));

			glDisable(GL_DEPTH_TEST);

			glBindVertexArray(quad_vao);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			fb->unbind();

			ImGui::GetWindowDrawList()->AddImage(	
													//(void*)(size_t)s_canvas->getG_FrameBuffer()->getTextureID(1),
													(void*)(size_t)fb->getTextureID(),
													pos,
													ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
													ImVec2(0, 1),
													ImVec2(1, 0));
		}
	}
	ImGui::End();
}

void GUI::normalFrameBufferDebugPanel()
{
	if (!s_showNormalFrameBufferDebugPanel) { return; }

	ImGui::Begin("FrameBuffer Normals", &s_showNormalFrameBufferDebugPanel);
	{
		const ImVec2 pos = ImGui::GetCursorScreenPos();

		const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		if (s_canvas->getG_FrameBuffer())
		{
			static FrameBuffer* fb = NULL;

			bindFB(fb, sceneWindowSize);

			//glViewport(0, 0, (GLsizei)sceneWindowSize.x, (GLsizei)sceneWindowSize.y);

			NormalToRGB_Shader::bind(s_canvas->getG_FrameBuffer()->getTextureID(1));

			glDisable(GL_DEPTH_TEST);

			glBindVertexArray(quad_vao);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			fb->unbind();

			ImGui::GetWindowDrawList()->AddImage(	
													//(void*)(size_t)s_canvas->getG_FrameBuffer()->getTextureID(1),
													(void*)(size_t)fb->getTextureID(),
													pos,
													ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
													ImVec2(0, 1),
													ImVec2(1, 0));
		}

		ImGui::End();
	}


}