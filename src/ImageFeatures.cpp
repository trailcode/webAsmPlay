/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#include <unordered_map>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/ImageFeatures.h>

using namespace std;

namespace
{
	unordered_map<string, ImageFeatures> a_features;
}

ImageFeatures::ImageFeatures()
{
	m_scores.reserve(100);
	m_labels.reserve(100);
	m_bounds.reserve(100);
}

void ImageFeatures::add(const float score, const string & label, const boost::python::numpy::ndarray & bounds)
{
	dmess("add " << score << " " << label);

	m_scores.push_back(score);
	m_labels.push_back(label);
	m_bounds.push_back(bounds);
}

void ImageFeatures::addFeatures(const string & imageID, const ImageFeatures & features)
{
	dmess("addFeatures " << imageID);

	a_features[imageID] = features;
}

const ImageFeatures * ImageFeatures::getFeatures(const string & imageID)
{
	const auto i = a_features.find(imageID);

	if(i == a_features.end()) { return nullptr ;}

	return &i->second;
}

#endif
