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

#include <algorithm>
#include <unordered_set>
#include <limits>
#include <vector>
#include <webAsmPlay/Texture.h>

using namespace std;

atomic_size_t Texture::s_desiredMaxNumTiles = { 4000 };

GLuint Texture::s_NO_DATA = numeric_limits<GLuint>::max();

namespace
{
	vector<GLuint> a_texturesToFree;

	unordered_set<Texture *> a_currTextures; // TODO Use an array! No newsing and deleting all the time.
}

Texture::Texture()
{
	a_currTextures.insert(this);
}

Texture::~Texture()
{
	if(textureReady()) { a_texturesToFree.push_back(m_textureID) ;}

	a_currTextures.erase(this);
}

bool Texture::textureReady() const
{
	const GLuint textureID = m_textureID;

	return textureID != 0 && textureID != s_NO_DATA;
}

size_t Texture::pruneTiles()
{
	vector<Texture*> tiles;

	for (const auto tile : a_currTextures)
	{
		if(!tile->m_loading) { tiles.push_back(tile) ;}
	}

	sort(tiles.begin(), tiles.end(), [](const Texture* A, const Texture* B) { return A->m_lastAccessTime < B->m_lastAccessTime ;});

	const size_t cacheSize = s_desiredMaxNumTiles;

	if (tiles.size() < cacheSize) { return 0; }

	size_t numFreed = 0;

	for (size_t i = 0; i < tiles.size() - cacheSize; ++i)
	{
		// Deleting the tile removes it from a_currTileSet and adds the texture ID to a_texturesToFree
		delete tiles[i];

		++numFreed;
	}

	if(a_texturesToFree.size()) { glDeleteTextures((GLsizei)a_texturesToFree.size(), &a_texturesToFree[0]) ;}

	a_texturesToFree.clear();

	//dmess("numFreed " << numFreed << " " << tiles.size() - cacheSize << " a_currTileSet " << a_currTileSet.size() << " cacheSize " << cacheSize);

	return numFreed;
}
