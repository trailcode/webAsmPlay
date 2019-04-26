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

uniform sampler2D tex; // TODO why does the ordering matter here? Something must not be correct.

layout(location = 0) in vec3  vertIn;
layout(location = 1) in float vertColorIn;
layout(location = 2) in vec3  normalIn;

uniform mat4	model;
uniform mat4	view;
uniform mat4	projection;
uniform float	colorLookupOffset;
uniform float	heightMultiplier;

out vec4 vertexColorNear;
out vec4 vertexColorFar;
out vec4 position_in_view_space;
out vec3 normal;
out vec3 fragPos;

void main()
{
	vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

	fragPos = vec3(model * vert);

	mat4 MV = view * model;

	position_in_view_space = MV * vert;

	gl_Position = projection * MV * vert;

	vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
	vertexColorFar  = texture(tex, vec2(vertColorIn + (2.0 + colorLookupOffset) / 32.0, 0.5));

	normal = mat3(transpose(inverse(model))) * normalIn;
}