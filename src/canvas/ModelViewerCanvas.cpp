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
#include <webAsmPlay/Model.h>
#include <webAsmPlay/shaders/PhongShader.h>
#include <webAsmPlay/FrameBuffer.h>
#include <webAsmPlay/canvas/ModelViewerCanvas.h>

using namespace glm;

namespace
{
	Model * a_model = nullptr;
}

ModelViewerCanvas::ModelViewerCanvas(	const bool   useFrameBuffer,
										const vec4 & clearColor) : Canvas(true, clearColor)
{
	a_model = new Model("C:/build/LearnOpenGL/resources/objects/cartoon_lowpoly_trees_blend.obj");
}

ModelViewerCanvas::~ModelViewerCanvas()
{
}

GLuint ModelViewerCanvas::render()
{
	dmess("ModelViewerCanvas::render");

	preRender();

	PhongShader::getDefaultInstance()->bind(this, false);

	a_model->Draw(PhongShader::getDefaultInstance());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return m_frameBuffer->getTextureID();
}
