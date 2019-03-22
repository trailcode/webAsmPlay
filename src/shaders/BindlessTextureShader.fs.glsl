﻿/**
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

#version 440 core

#extension GL_ARB_bindless_texture : require

out vec4 outColor;
in vec2 UV;
//uniform sampler2D tex;
//in uint texID;
//uniform uvec2 texID;
//uniform sampler2D  texID;
uniform int texID;
// Texture block
/*
layout (binding = 6, std140) uniform TEXTURE_BLOCK
{
sampler2D      texa[1024];
};
*/

layout (binding = 6, std140) uniform TEXTURE_BLOCK
{
	sampler2D      texa[2048];
};

/*
layout(binding = 0) uniform material
{
uvec2 Diffuse;

} Material;
*/

void main()
{
	//outColor = texture( tex, UV );
	outColor = texture( texa[texID], UV );
	//outColor = texture(sampler2D(Material.Diffuse), UV);
	//outColor = texture(sampler2D(texID), UV);
	//outColor = texture(texID, UV);

	//outColor.r = 0.5;

	outColor.a = 1.0;
}
