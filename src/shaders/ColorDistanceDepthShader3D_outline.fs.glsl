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

#version 430 core

in vec4 vertexColorNear;
in vec4 vertexColorFar;
in vec4 posViewSpace;
in vec4 glPos;

layout (location = 0) out vec4 outColor;

uniform sampler2D depthTex;

bool canDiscard()
{
	vec2 size = textureSize(depthTex, 0);

	float posX = gl_FragCoord.x / size.x;
	float posY = gl_FragCoord.y / size.y;

	float deltaX = 1.0f / size.x;
	float deltaY = 1.0f / size.y;

	for(float x = -1.0f; x < 2.0f; x += 1.0f) 
	for(float y = -1.0f; y < 2.0f; y += 1.0f)
	{
		vec4 t = textureLod(depthTex, vec2(posX + x * deltaX, posY + y * deltaY), 0);

		float v = abs(t.w - glPos.w);

		if(v <= 0.0001) { return false ;}
	}

	return true;
}

void main()
{
	if(canDiscard()) { discard ;}

	float minDist = 0.0;
	float maxDist = 5.0;

	// computes the distance between the fragment position 
	// and the origin (4th coordinate should always be 1 
	// for points). The origin in view space is actually 
	// the camera position.
	float dist = max(0.0, distance(posViewSpace, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);

	dist = min(maxDist, dist) / maxDist;

	outColor = vertexColorNear * (1.0f - dist) + vertexColorFar * dist;
}