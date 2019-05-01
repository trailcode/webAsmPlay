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
in vec4 position_in_view_space;
noperspective in vec4 fragCoord2D;

// Input from vertex shader
in VS_OUT
{
	vec3 N;
	//vec3 L;
	vec3 V;
} fs_in;

uniform sampler2D topDownTexture;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 normalDepth;

void main()
{
	float minDist = 0.0;
	float maxDist = 5.0;

	// computes the distance between the fragment position 
	// and the origin (4th coordinate should always be 1 
	// for points). The origin in view space is actually 
	// the camera position.
	float dist = max(0.0, distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);

	dist = min(maxDist, dist) / maxDist;

	outColor = vertexColorNear * (1.0f - dist) + vertexColorFar * dist;

	vec4 texColor = vec4(texture(topDownTexture, fragCoord2D.xy).xyz, 0);

	outColor += texColor;

	vec3 N = normalize(fs_in.N);

	normalDepth = vec4(N, fs_in.V.z);
}