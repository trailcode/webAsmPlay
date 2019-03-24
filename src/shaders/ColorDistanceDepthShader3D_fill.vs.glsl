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

#version 330 core

layout(location = 0) in vec3  vertIn;
layout(location = 1) in float vertColorIn;
layout(location = 2) in vec3  normalIn;

uniform mat4      model;
uniform mat4      view;
uniform mat4      projection;
uniform float     colorLookupOffset;
uniform float     heightMultiplier;
uniform sampler2D colorLookupTexture;

out vec4 vertexColorNear;
out vec4 vertexColorFar;
out vec4 position_in_view_space;
out vec3 normal;
out vec3 fragPos;
out vec4 glPos;
//noperspective out vec4 fragCoord2D;
out vec4 fragCoord2D;

void main()
{
	vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

	fragPos = vec3(model * vert);

	mat4 MV = view * model;

	position_in_view_space = MV * vert;

	gl_Position = projection * MV * vert;

	glPos = gl_Position; // TODO just use gl_Position?

	vertexColorNear = texture(colorLookupTexture, vec2(vertColorIn +        colorLookupOffset  / 32.0, 0.5));
	vertexColorFar  = texture(colorLookupTexture, vec2(vertColorIn + (2.0 + colorLookupOffset) / 32.0, 0.5));

	normal = mat3(transpose(inverse(model))) * normalIn;

	
	fragCoord2D  = projection * MV * vec4(vert.xy, 0, 1);

											// Vertex in NDC-space
	fragCoord2D.xyz /= fragCoord2D.w;       // Rescale: [-1,1]^3
	fragCoord2D.w    = 1.0 / fragCoord2D.w; // Invert W

													// Vertex in window-space
	fragCoord2D.xyz *= vec3(0.5) + vec3(0.5); // Rescale: [0,1]^3

	fragCoord2D.xyz += vec3(1);

	fragCoord2D.xyz *= vec3(0.5);
}