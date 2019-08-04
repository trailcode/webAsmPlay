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
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/KeyFrame.h>

using namespace std;
using namespace glm;

list<KeyFrame> KeyFrame::s_keyFrames;

void KeyFrame::create()
{
	const auto camera = GUI::getMainCamera();

	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex >= GUI::s_currAnimationTime) { break ;}
	}

	s_keyFrames.insert(i, std::move(KeyFrame(	GUI::s_currAnimationTime,
												camera->getCenterConstRef(),
												camera->getEyeConstRef(),
												camera->getUpConstRef())));
}

KeyFrame::KeyFrame(	const float timeIndex,
					const vec3	cameraCenter,
					const vec3	cameraEye,
					const vec3	cameraUp) :	m_timeIndex		(timeIndex),
											m_cameraCenter	(cameraCenter),
											m_cameraEye		(cameraEye),
											m_cameraUp		(cameraUp)
{

}

void KeyFrame::printFrames()
{
	dmess("--------------------------------------------------------");

	for(const auto & frame : s_keyFrames)
	{
		dmess("Time: " << frame.m_timeIndex << " center: " << frame.m_cameraCenter << " eye: " << frame.m_cameraEye);
	}
}

void KeyFrame::setClosest()
{
	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex > GUI::s_currAnimationTime) { break ;}
	}

	--i;

	dmess(" " << i->m_timeIndex);

	const auto camera = GUI::getMainCamera();

	camera->setCenter(i->m_cameraCenter);
	camera->setEye(i->m_cameraEye);
	camera->setUp(i->m_cameraUp);
	camera->update();
}

void KeyFrame::update(const float timeIndex)
{
	dmess("update: " << timeIndex);


}