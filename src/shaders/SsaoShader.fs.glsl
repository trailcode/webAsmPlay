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

// Various uniforms controlling SSAO effect
/*
uniform float ssao_level = 0.5;
uniform float object_level = 0;
uniform float ssao_radius = 0.005 * 3500.0 / 1000.0;
uniform uint point_count = 64;
uniform bool randomize_points = true;
*/

uniform float ssao_level = 0.7;
uniform float object_level = 1.0;
//uniform float ssao_radius = 0.005 * 3500.0 / 1000.0;
//uniform float ssao_radius = 0.05;
uniform float ssaoRadius = 0.000001;
uniform uint point_count = 64;
uniform bool randomize_points = false;

// Samplers for pre-rendered color, normal and depth
layout (binding = 0) uniform sampler2D sColor;
layout (binding = 1) uniform sampler2D sNormalDepth;

// Final output
layout (location = 0) out vec4 color;

// Uniform block containing up to 256 random directions (x,y,z,0)
// and 256 more completely random vectors
layout (binding = 0, std140) uniform SAMPLE_POINTS
{
	vec4 pos[256];
	vec4 random_vectors[256];
} points;

void main()
{
	/*
	vec2 P = gl_FragCoord.xy / textureSize(sColor, 0);

	vec4 object_color =  textureLod(sColor, P, 0);

	color = object_color;
	*/

	// Get texture position from gl_FragCoord
	vec2 P = gl_FragCoord.xy / textureSize(sNormalDepth, 0);
	// ND = normal and depth
	vec4 ND = textureLod(sNormalDepth, P, 0);
	// Extract normal and depth
	vec3 N = ND.xyz;
	float my_depth = ND.w;

	// Local temporary variables
	int i;
	int j;
	int n;

	float occ = 0.0;
	float total = 0.0;

	// n is a pseudo-random number generated from fragment coordinate
	// and depth
	n = (int(gl_FragCoord.x * 7123.2315 + 125.232) *
		 int(gl_FragCoord.y * 3137.1519 + 234.8)) ^
		 int(my_depth);
	// Pull one of the random vectors
	vec4 v = points.random_vectors[n & 255];

	// r is our 'radius randomizer'
	float r = (v.r + 3.0) * 0.1;
	if (!randomize_points)
		r = 0.5;

	r = ssaoRadius;
	//r = 0;

	// For each random point (or direction)...
	for (i = 0; i < point_count; i++)
	{
		// Get direction
		vec3 dir = points.pos[i].xyz;

		//dir = vec3(1, 0, 0);

		// Put it into the correct hemisphere
		if (dot(N, dir) < 0.0)
			dir = -dir;

		// f is the distance we've stepped in this direction 
		// z is the interpolated depth
		float f = 0.0;
		float z = my_depth;

		// We're going to take 4 steps - we could make this
		// configurable
		//total += 4.0;

		int numIter = 2;

		total += numIter;

		//for (j = 0; j < 4; j++)
		for (j = 0; j < numIter; j++)
		{
			// Step in the right direction
			f += r;
			// Step _towards_ viewer reduces z
			z -= dir.z * f;
			//z -= 0.001;

			// Read depth from current fragment
			//float their_depth = textureLod(sNormalDepth, (P + dir.xy * f * ssao_radius), 0).w;
			float their_depth = textureLod(sNormalDepth, (P + dir.xy * f), 0).w;
			//float their_depth = textureLod(sNormalDepth, (P + dir.xy * 0 * ssao_radius), 0).w;
			//float their_depth = textureLod(sNormalDepth, P, 0).w;

			// Calculate a weighting (d) for this fragment's
			// contribution to occlusion
			float d = abs(their_depth - my_depth) * 1.0;
			//d *= d;

			// If we're obscured, accumulate occlusion
			//if ((z - their_depth) > 0.0)
			if ((z - their_depth) > 0.0 && d > 0.001)
			//if ((z - their_depth) > 0.01)
			{
				//occ += 4.0 / (1.0 + d);
				//occ += 4.0;
				//occ += d;
				//occ += float(numIter);
				occ += float(numIter) / (1.0 + d);
			}
			//occ += d;
			//occ = my_depth;
		}
	}

	// Calculate occlusion amount
	//float ao_amount = (1.0 - occ / total);
	float ao_amount = min(1.0, (1.0 - occ / total));
	//float ao_amount = occ / total;
	//float ao_amount = (occ / total);

	// Get object color from color texture
	vec4 object_color =  textureLod(sColor, P, 0);

	// Mix in ambient color scaled by SSAO level
	//color = object_level * object_color + mix(vec4(0.2), vec4(ao_amount), ssao_level);
	//color = (object_level * object_color) * vec4(vec3(ao_amount * 0.5), 1.0);
	color = (object_level * object_color) * vec4(vec3(ao_amount), 1.0);
	//color = object_level * object_color;
	//color = vec4(vec3(ao_amount), 1.0);
}
