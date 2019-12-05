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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/Texture.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace std;

void GUI::textureSystemPanel()
{
	if(!s_showTextureSystemPanel) { return ;}

	ImGui::Begin("Texture System", &s_showTextureSystemPanel);
	{
		ImGui::Text(("   Textures: " + to_string(Texture::getNumTextures())).c_str());
		ImGui::Text(("    Loading: " + to_string(Texture::getNumLoading())).c_str());
		ImGui::Text(("Downloading: " + to_string(Texture::getNumDownloading())).c_str());
		ImGui::Text(("  Uploading: " + to_string(Texture::getNumUploading())).c_str());
		ImGui::Text(("    Writing: " + to_string(Texture::getNumWriting())).c_str());
		ImGui::Text((" Cache Hits: " + to_string(Texture::getNumCacheHits())).c_str());
		ImGui::Text(("Cache Mises: " + to_string(Texture::getNumCacheMises())).c_str());

		int maxDesiredNumTextures = (int)Texture::s_desiredMaxNumTextures;

		if (ImGui::SliderInt("Max Tile Cache Size", &maxDesiredNumTextures, 150, 6000))
		{
			dmess("maxDesiredNumTiles " << maxDesiredNumTextures);

			Texture::s_desiredMaxNumTextures = (size_t)maxDesiredNumTextures; 
		}
	}
	ImGui::End();
}