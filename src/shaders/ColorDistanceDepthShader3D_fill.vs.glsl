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

layout(location = 0) in vec3  vertIn;
layout(location = 1) in float vertColorIn;
layout(location = 2) in vec3  normalIn;

uniform vec3	  lightPos;
uniform float     colorLookupOffset;
uniform float     heightMultiplier;
uniform float     width;
uniform float     height;
uniform sampler2D colorLookupTexture;

layout(std140, binding = 0) uniform constants
{
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 modelView;
	mat4 modelViewProj;
};

out vec4 vertexColorNear;
out vec4 vertexColorFar;
out vec4 position_in_view_space;
out vec3 normal;
out vec3 fragPos;
out vec4 glPos;


// Inputs from vertex shader
out VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;


void main()
{
	vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

	fragPos = vec3(model * vert);

	// Calculate view-space coordinate
	position_in_view_space = modelView * vert; 

	//gl_Position = projection * MV * vert;
	gl_Position = modelViewProj * vert;

	glPos = gl_Position; // TODO just use gl_Position?

	vertexColorNear = texture(colorLookupTexture, vec2(vertColorIn +        colorLookupOffset  / 32.0, 0.5));
	vertexColorFar  = texture(colorLookupTexture, vec2(vertColorIn + (2.0 + colorLookupOffset) / 32.0, 0.5));

	normal = mat3(transpose(inverse(model))) * normalIn;

	// Calculate normal in view-space
	//vs_out.N = mat3(MV) * normalIn;
	vs_out.N = normalIn;

	// Calculate light vector
	vs_out.L = lightPos - position_in_view_space.xyz;

	// Calculate view vector
	//vs_out.V = -position_in_view_space.xyz;
	vs_out.V = position_in_view_space.xyz;
}