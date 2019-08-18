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
using namespace nlohmann;

/*
KeyFrame KeyFrame::create()
{
	
}
*/

namespace
{
	size_t currID = 0;
}

KeyFrame::KeyFrame(	const float   timeIndex,
					const vec3	& cameraCenter,
					const vec3	& cameraEye,
					const vec3	& cameraUp) :	m_timeIndex		(timeIndex),
												m_cameraCenter	(cameraCenter),
												m_cameraEye		(cameraEye),
												m_cameraUp		(cameraUp),
												m_ID			(++currID)
{

}

/*
json KeyFrame::save()
{
	json keyFrames;

	for(const auto & i : s_keyFrames)
	{
		json keyFrame;

		keyFrames["frames"].push_back
	}

	return keyFrames;
}

void KeyFrame::load(const json & keyFrames)
{
	s_keyFrames.clear();
}
*/