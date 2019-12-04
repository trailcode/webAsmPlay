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
		ImGui::Text(("      num tiles: " + to_string(Texture::getNumTiles())).c_str());
		ImGui::Text(("    num loading: " + to_string(Texture::getNumLoading())).c_str());
		ImGui::Text(("num downloading: " + to_string(Texture::getNumDownloading())).c_str());
		ImGui::Text(("  num uploading: " + to_string(Texture::getNumUploading())).c_str());
		ImGui::Text(("    num writing: " + to_string(Texture::getNumWriting())).c_str());
		//ImGui::Text(("   num rendered: " + to_string(Texture::getNumRendered())).c_str());

		int maxDesiredNumTiles = (int)Texture::s_desiredMaxNumTiles;

		if (ImGui::SliderInt("Max Tile Cache Size", &maxDesiredNumTiles, 150, 6000))
		{
			dmess("maxDesiredNumTiles " << maxDesiredNumTiles);

			Texture::s_desiredMaxNumTiles = (size_t)maxDesiredNumTiles; 
		}
	}
	ImGui::End();
}