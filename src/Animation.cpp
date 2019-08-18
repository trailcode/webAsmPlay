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

#include <fstream>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/canvas/TrackBallInteractor.h>
#include <webAsmPlay/Animation.h>

using namespace std;
using namespace glm;
using namespace nlohmann;

list<KeyFrame>		Animation::s_keyFrames;
vector<KeyFrame *>	Animation::s_keyFramesVec;

void Animation::createKeyFrame()
{
	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex >= GUI::s_currAnimationTime) { break ;}
	}

	s_keyFrames.insert(i, move(KeyFrame(GUI::s_currAnimationTime,
										GUI::getMainCamera()->getCenterConstRef(),
										GUI::getMainCamera()->getEyeConstRef(),
										GUI::getMainCamera()->getUpConstRef())));

	updateKeyFramesVec();
}

void Animation::updateKeyFramesVec()
{
	s_keyFramesVec.clear();

	for(auto & i : s_keyFrames) { s_keyFramesVec.push_back(&i) ;}
}

void Animation::printFrames()
{
	dmess("--------------------------------------------------------");

	for(const auto & frame : s_keyFrames)
	{
		dmess("Time: " << frame.m_timeIndex << " " << frame.m_ID << " center: " << frame.m_cameraCenter << " eye: " << frame.m_cameraEye);
	}
}

void Animation::setClosest()
{
	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex > GUI::s_currAnimationTime) { break ;}
	}

	--i;

	dmess(" " << i->m_timeIndex);

	GUI::getMainCamera()->setCenter	(i->m_cameraCenter);
	GUI::getMainCamera()->setEye	(i->m_cameraEye);
	GUI::getMainCamera()->setUp		(i->m_cameraUp);
	GUI::getMainCamera()->update();
	
	update(GUI::s_currAnimationTime);
}

namespace
{
	// From: https://blog.demofox.org/2015/08/08/cubic-hermite-interpolation/
	// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
	// when t is 0, this will return B.  When t is 1, this will return C.
	inline double cubicHermite (const double A, const double B, const double C, const double D, const double t)
	{
		float a = -A/2.0f + (3.0f*B)/2.0f - (3.0f*C)/2.0f + D/2.0f;
		float b = A - (5.0f*B)/2.0f + 2.0f*C - D / 2.0f;
		float c = -A/2.0f + C/2.0f;
		float d = B;
 
		return a*t*t*t + b*t*t + c*t + d;
	}

	inline vec3 cubicHermite(const vec3 A, const vec3 B, const vec3 C, const vec3 D, const double t)
	{
		return vec3(cubicHermite(A.x, B.x, C.x, D.x, t),
					cubicHermite(A.y, B.y, C.y, D.y, t),
					cubicHermite(A.z, B.z, C.z, D.z, t));
	}
}

void Animation::update(const float timeIndex)
{
	int i = 0;
	
	for(; i < s_keyFramesVec.size(); ++i)
	{
		if(s_keyFramesVec[i]->m_timeIndex > GUI::s_currAnimationTime) { break ;} 
	}

	const auto next1 = s_keyFramesVec[(i + 0) % s_keyFramesVec.size()];
	const auto next2 = s_keyFramesVec[(i + 1) % s_keyFramesVec.size()];
	const auto prev1 = s_keyFramesVec[(i - 1) % s_keyFramesVec.size()];
	const auto prev2 = s_keyFramesVec[(i - 2) % s_keyFramesVec.size()];

	const double t = (timeIndex - prev1->m_timeIndex) / (next1->m_timeIndex - prev1->m_timeIndex);

	/*
	dmess(	" prev2 " << prev2->m_timeIndex << " " << prev2->m_ID <<
			" prev1 " << prev1->m_timeIndex << " " << prev1->m_ID <<
			" next1 " << next1->m_timeIndex << " " << next1->m_ID <<
			" next2 " << next2->m_timeIndex << " " << next2->m_ID << " t " << t);
			*/

	const auto cameraCenter = cubicHermite(prev2->m_cameraCenter,	prev1->m_cameraCenter,	next1->m_cameraCenter,	next2->m_cameraCenter,	t);
	const auto cameraEye	= cubicHermite(prev2->m_cameraEye,		prev1->m_cameraEye,		next1->m_cameraEye,		next2->m_cameraEye,		t);
	const auto cameraUp		= cubicHermite(prev2->m_cameraUp,		prev1->m_cameraUp,		next1->m_cameraUp,		next2->m_cameraUp,		t);

	GUI::getMainCamera()->setCenter	(cameraCenter);
	GUI::getMainCamera()->setEye	(cameraEye);
	//GUI::getMainCamera()->setEye	(cameraCenter + vec3(0,0,1));
	GUI::getMainCamera()->setUp		(cameraUp);

	GUI::getMainCamera()->update();
}

void Animation::load(const json & animation)
{
	try
	{
		for(const auto & keyFrame : animation["keyFrames"])
		{
			s_keyFrames.push_back(KeyFrame(		keyFrame["timeIndex"],
												toVec3( keyFrame["cameraCenter"]),
												toVec3( keyFrame["cameraEye"]),
												toVec3( keyFrame["cameraUp"])));
		}
	}
	catch(const exception & e)
	{
		dmess("Error loading: " << e.what());

		s_keyFrames.clear();
	}

	updateKeyFramesVec();
}

void Animation::loadFile(const string & jsonFile)
{
	ifstream in(jsonFile);

	if(!in.is_open())
	{
		dmess("Warn: could not open animation file: " << jsonFile);

		return;
	}

	json animation;

	animation << in;

	in.close();

	load(animation);
}

const json Animation::save()
{
	json animation;

	for(const auto & i : s_keyFrames)
	{
		json keyFrame{	{"timeIndex",		i.m_timeIndex				},
						{"cameraEye",		toTuple(i.m_cameraEye)		},
						{"cameraCenter",	toTuple(i.m_cameraCenter)	},
						{"cameraUp",		toTuple(i.m_cameraUp)		}};

		animation["keyFrames"].push_back(keyFrame);
	}

	return animation;
}

size_t Animation::numKeys() { return s_keyFrames.size() ;}

namespace
{
	void setKey(const KeyFrame & key)
	{
		GUI::getMainCamera()->setCenter	(key.m_cameraCenter);
		GUI::getMainCamera()->setEye	(key.m_cameraEye);
		GUI::getMainCamera()->setUp		(key.m_cameraUp);

		GUI::getTrackBallInteractor()->updateCameraEyeUp(true, true);

		GUI::getMainCamera()->update();

		GUI::s_currAnimationTime = key.m_timeIndex;
	}
}

void Animation::prev()
{
	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex >= GUI::s_currAnimationTime) { break ;}
	}

	setKey(*--i);
}

void Animation::next()
{
	list<KeyFrame>::iterator i;

	for(i = s_keyFrames.begin(); i != s_keyFrames.end(); ++i)
	{
		if(i->m_timeIndex > GUI::s_currAnimationTime) { break ;}
	}

	setKey(*i);
}