/**
// T R A I L C O D E
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
uniform sampler2D depthTex;
in vec4 vertexColorNear;
in vec4 vertexColorFar;
in vec4 position_in_view_space;
in vec3 normal; 
in vec3 fragPos;
in vec4 glPos;

uniform vec3      lightPos;
uniform float     width;
uniform float     height;


out vec4 outColor;

void main()
{
	vec4 t = texture(depthTex, vec2(gl_FragCoord.x / width, gl_FragCoord.y / height));

	float v = abs(t.x - glPos.w);

	if(v > 0.0001)
	{
		discard;
	}

	float minDist = 0.0;
	float maxDist = 5.0;
	vec3 lightColor = vec3(1,1,1);
	//vec3 objectColor = vec3(1,1,0);
	vec3 viewPos = vec3(0,0,0);

	// computes the distance between the fragment position 
	// and the origin (4th coordinate should always be 1 
	// for points). The origin in view space is actually 
	// the camera position.
	float dist = max(0.0, distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);

	dist = min(maxDist, dist) / maxDist;

	vec4 objectColor = vertexColorNear * (1.0f - dist) + vertexColorFar * dist;

	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 result = diffuse * vec3(objectColor);
	if(distance(result, vec3(0,0,0)) < 0.0001)
		//if(distance(result, vec3(0,0,0)) > 0.0001)
	{
		//discard;
	}
	outColor = vec4(result, objectColor.w);
}