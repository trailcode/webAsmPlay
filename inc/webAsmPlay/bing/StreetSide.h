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
#pragma once

#include <vector>
#include <glm/mat4x4.hpp>

class Bubble;
class Renderable;

class StreetSide
{
public:

	static bool ensureBubbleCollectionTile(const glm::dmat4 & trans, const glm::dvec2 & pos, const size_t zoomLevel = 16);

	static void queryClosestBubbles(const glm::dvec2 & pos, const size_t num);

	static std::vector<std::pair<Bubble *, Renderable *>> s_closestBubbles;
	
	static Bubble * closestBubble();

	static Renderable * closestBubbleRenderable();

	static void preFetchBubbleTiles();

private:

	static void query(const glm::dmat4 & trans, const std::string & quadKey, const glm::dvec2 & tMin, const glm::dvec2 & tMax);

	static void indexBubbles(const glm::dmat4 & trans, const std::vector<Bubble *> & bubbles);

	StreetSide() {}
	~StreetSide() {}
};