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
#include <filesystem>
#include <ctpl/ctpl.h>
#include <SDL_image.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/Texture.h>

using namespace std;
using namespace std::filesystem;
using namespace ctpl;
using namespace curlUtil;

atomic_size_t Texture::s_desiredMaxNumTiles = { 4000 };

GLuint Texture::s_NO_DATA = numeric_limits<GLuint>::max();

unordered_map<string, Texture *> Texture::s_textures;

namespace
{
	vector<GLuint> a_texturesToFree;

	//unordered_set<Texture *> a_currTextures; // TODO Use an array! No newsing and deleting all the time.

	thread_pool a_loaderPool(1);
	thread_pool a_writerPool(1);
}

Texture::Texture(const string & ID) : m_ID(ID)
{
	//a_currTextures.insert(this);
	s_textures[ID] = this;
}

Texture::~Texture()
{
	if(textureReady()) { a_texturesToFree.push_back(m_textureID) ;}

	s_textures.erase(m_ID);
}

bool Texture::textureReady() const
{
	const GLuint textureID = m_textureID;

	return textureID != 0 && textureID != s_NO_DATA;
}

void Texture::readyTexture()
{
	m_stillNeeded = true;

	if(m_loading) { return ;}

	m_loading = true;

	//++s_numLoading;

	a_loaderPool.push([this](int ID)
	{
		//fetchTile(ID, tile);
		readyTexture(ID);
	});
}

bool s_useCache = true;
bool s_useBindlessTextures = false;

void Texture::readyTexture(const int ID)
{
	if (!m_stillNeeded)
	{
		m_loading = false;

		//--s_numLoading;

		return;
	}

	/*
	const string quadKey = tileToQuadKey(latLongToTile(tile->m_center, tile->m_level), tile->m_level);
	
	const string tileCachePath = "./tiles/" + quadKey + ".jpg";
	*/

	const string tileCachePath = "./tiles/" + m_ID + ".jpg";

	if(s_useCache && fileExists(tileCachePath))
	{
		//--s_numLoading;

		if(!file_size(tileCachePath.c_str())) { return markTileNoData() ;}

		auto img = IMG_Load(tileCachePath.c_str());

		if (!img) { goto download ;}

		//++s_numUploading;

		Textures::s_queue.push([this, img, tileCachePath](int ID)
		{
			//--s_numUploading;

			if (!m_stillNeeded)
			{
				m_loading = false;

				SDL_FreeSurface(img);

				return;
			}

			OpenGL::ensureSharedContext();

			m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(s_useBindlessTextures) { m_handle = glGetTextureHandleARB(m_textureID) ;}

			m_loading = false;
		});

		return;
	}
	
	download:

	//const auto url = "http://t1.ssl.ak.dynamic.tiles.virtualearth.net/comp/ch/" + quadKey + "?mkt=en-GB&it=A"; 

	//const auto url = tile->getDownloadURL

	download(getDownloadURL(), 
	//url, 
	[this]() -> bool
	{
		//--s_numLoading;

		if(!m_stillNeeded) { m_loading = false ;}

		return m_stillNeeded;
	},
	[this, tileCachePath
	//, url
	](BufferStruct * tileBuffer)
	{
		if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
		{
			//markTileNoData(tile);
			markTileNoData();

			if (!s_useCache) { return ;}

			FILE * fp = fopen(tileCachePath.c_str(), "wb");

			if(fp) { fclose(fp) ;}

			else { dmess("Warn could not write file: " << tileCachePath) ;}

			return;
		}

		//++s_numUploading;

		Textures::s_queue.push([this, tileBuffer, tileCachePath
		//, url
		](int ID)
		{
			//--s_numUploading;

			if (!m_stillNeeded)
			{
				m_loading = false;
				
				return;
			}

			auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

			if(!img) { return markTileNoData() ;}

			const auto bytesPerPixel = img->format->BytesPerPixel;

			if(bytesPerPixel < 3)
			{
				SDL_FreeSurface(img);

				// Must be the no data png image, mark as no data.
				return markTileNoData();
			}

			OpenGL::ensureSharedContext();

			m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(s_useBindlessTextures) { m_handle = glGetTextureHandleARB(m_textureID) ;}

			m_loading = false;

			if (!s_useCache) { return ;}

			//++s_numWriting;

			a_writerPool.push([tileCachePath, tileBuffer](int ID)
			{
				FILE * fp = fopen(tileCachePath.c_str(), "wb");

				if(fp)
				{
					fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

					fclose(fp);
				}
				else { dmess("Warn could not write file: " << tileCachePath) ;}

				//--s_numWriting;
			});
		});
	});
}

void Texture::markTileNoData()
{

}

size_t Texture::pruneTiles()
{
	vector<Texture*> tiles;

	for (const auto [ID, texture] : s_textures)
	{
		if(!texture->m_loading) { tiles.push_back(texture) ;}
	}

	sort(tiles.begin(), tiles.end(), [](const Texture* A, const Texture* B) { return A->m_lastAccessTime < B->m_lastAccessTime ;});

	const size_t cacheSize = s_desiredMaxNumTiles;

	if (tiles.size() < cacheSize) { return 0; }

	size_t numFreed = 0;

	for (size_t i = 0; i < tiles.size() - cacheSize; ++i)
	{
		delete tiles[i];

		++numFreed;
	}

	if(a_texturesToFree.size()) { glDeleteTextures((GLsizei)a_texturesToFree.size(), &a_texturesToFree[0]) ;}

	a_texturesToFree.clear();

	dmess("numFreed " << numFreed << " " << tiles.size() - cacheSize << " s_textures " << s_textures.size() << " cacheSize " << cacheSize);

	return numFreed;
}

size_t Texture::getNumTiles() { return s_textures.size() ;}