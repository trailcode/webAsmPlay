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

#ifndef __EMSCRIPTEN__
#include <boost/python.hpp>
#include <ctpl/ctpl.h>
#endif

#include <SDL_image.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/Texture.h>

using namespace std;
using namespace std::filesystem;
using namespace curlUtil;

#ifndef __EMSCRIPTEN__
using namespace ctpl;

namespace p		= boost::python;
namespace np	= boost::python::numpy;
#endif

atomic_size_t Texture::s_desiredMaxNumTextures = { 4000 };

GLuint Texture::s_NO_DATA = numeric_limits<GLuint>::max();

unordered_map<string, Texture *> Texture::s_textures; // TODO Use an array! No newsing and deleting all the time.

bool Texture::s_useCache			= true;
bool Texture::s_useBindlessTextures	= true;

namespace
{
	vector<GLuint> a_texturesToFree;

#ifndef __EMSCRIPTEN__

	thread_pool a_loaderPool(8);
	thread_pool a_writerPool(8);

#endif

	atomic<size_t> a_numLoading;
	atomic<size_t> a_numDownloading;
	atomic<size_t> a_numUploading;
	atomic<size_t> a_numWriting;
	atomic<size_t> a_numCacheHits;
	atomic<size_t> a_numCacheMisses;

	atomic<size_t> a_frameNumber = {0};

	class MyCleanup
	{
	public:

		~MyCleanup()
		{
#ifndef __EMSCRIPTEN__

			a_loaderPool.stop();
			a_writerPool.stop();

#endif
		}
	};

	static MyCleanup s_cleanup;
}

Texture::Texture(const string & ID) : m_ID(ID)
{
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

	if(m_loading || m_textureID) { return ;}

	m_loading = true;

	++a_numLoading;

#ifndef __EMSCRIPTEN__

	a_loaderPool.push([this](int ID) { readyTexture(ID) ;});

#else

	readyTexture(0);

#endif
}

void Texture::readyTexture(const int ID)
{
	if (!m_stillNeeded)
	{
		m_loading = false;

		--a_numLoading;

		return;
	}

	const string tileCachePath = "./tiles/" + m_ID + ".jpg";

	if(s_useCache && fileExists(tileCachePath))
	{
		++a_numCacheHits;

		--a_numLoading;

		if(!file_size(tileCachePath.c_str())) { return markNoData() ;}

		auto img = IMG_Load(tileCachePath.c_str());

		if (!img) { goto doDownload ;}

		++a_numUploading;

#ifndef __EMSCRIPTEN__
		Textures::s_queue.push([this, img, tileCachePath](int ID)
		{
			if (!m_stillNeeded)
			{
				m_loading = false;

				SDL_FreeSurface(img);

				--a_numUploading;

				return;
			}

			OpenGL::ensureSharedContext();

			m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(s_useBindlessTextures)
			{
				m_handle = glGetTextureHandleARB(m_textureID);
			}

			m_loading = false;

			--a_numUploading;
		});

#else
		
		if (!m_stillNeeded)
		{
			m_loading = false;

			SDL_FreeSurface(img);

			--a_numUploading;

			return;
		}

		OpenGL::ensureSharedContext();

		m_textureID = Textures::load(img);

		SDL_FreeSurface(img);

		m_loading = false;

		--a_numUploading;

#endif

		return;
	}
	
	doDownload:

	++a_numDownloading;

	++a_numCacheMisses;

	download(	getDownloadURL(), 
				[this]() -> bool
				{
					--a_numLoading;

					if(!m_stillNeeded)
					{
						--a_numDownloading;

						m_loading = false;
					}

					return m_stillNeeded;
				},
				[this, tileCachePath](BufferStruct * tileBuffer)
	{
		--a_numDownloading;

		if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
		{
			markNoData();

			if (!s_useCache) { return ;}

			FILE * fp = fopen(tileCachePath.c_str(), "wb");

			if(fp) { fclose(fp) ;}

			else { dmess("Warn could not write file: " << tileCachePath) ;}

			return;
		}

		++a_numUploading;

#ifndef __EMSCRIPTEN__

		Textures::s_queue.push([this, tileBuffer, tileCachePath](int ID)
		{
			if (!m_stillNeeded)
			{
				m_loading = false;
		
				--a_numUploading;
		
				return;
			}

			auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

			if(!img)
			{
				--a_numUploading;

				return markNoData();
			}

			const auto bytesPerPixel = img->format->BytesPerPixel;

			if(bytesPerPixel < 3)
			{
				SDL_FreeSurface(img);

				--a_numUploading;

				// Must be the no data png image, mark as no data.
				return markNoData();
			}

			OpenGL::ensureSharedContext();

			m_textureID = Textures::load(img);

			SDL_FreeSurface(img);

			if(s_useBindlessTextures)
			{
				m_handle = glGetTextureHandleARB(m_textureID);
			}

			m_loading = false;

			--a_numUploading;

			if (!s_useCache) { return ;}

			++a_numWriting;

			a_writerPool.push([tileCachePath, tileBuffer](int ID)
			{
				FILE * fp = fopen(tileCachePath.c_str(), "wb");

				if(fp)
				{
					fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

					fclose(fp);
				}
				else { dmess("Warn could not write file: " << tileCachePath) ;}

				--a_numWriting;
			});
		});

#else

		dmessError("Implement me!");

#endif

	});
}

void Texture::markNoData()
{
	m_loading = false;

	m_textureID = s_NO_DATA;
}

size_t Texture::pruneTextures()
{
	vector<Texture*> tiles;

	for (const auto [ID, texture] : s_textures)
	{
		if(!texture->m_loading) { tiles.push_back(texture) ;}
	}

	sort(tiles.begin(), tiles.end(), [](const Texture* A, const Texture* B) { return A->m_lastAccessTime < B->m_lastAccessTime ;});

	const size_t cacheSize = s_desiredMaxNumTextures;

	if (tiles.size() < cacheSize) { return 0; }

	size_t numFreed = 0;

	for (size_t i = 0; i < tiles.size() - cacheSize; ++i)
	{
		delete tiles[i];

		++numFreed;
	}

	if(a_texturesToFree.size()) { glDeleteTextures((GLsizei)a_texturesToFree.size(), &a_texturesToFree[0]) ;}

	a_texturesToFree.clear();

	//dmess("numFreed " << numFreed << " " << tiles.size() - cacheSize << " s_textures " << s_textures.size() << " cacheSize " << cacheSize);

	return numFreed;
}

size_t Texture::getNumTextures()	{ return s_textures.size()	;}
size_t Texture::getNumLoading()		{ return a_numLoading		;}
size_t Texture::getNumDownloading() { return a_numDownloading	;}
size_t Texture::getNumUploading()	{ return a_numUploading		;}
size_t Texture::getNumWriting()		{ return a_numWriting		;}
size_t Texture::getNumCacheHits()	{ return a_numCacheHits		;}
size_t Texture::getNumCacheMisses()	{ return a_numCacheMisses	;}

void Texture::incrementFrameNumber() { ++a_frameNumber ;}

size_t Texture::getFrameNumber() { return a_frameNumber ;}

#ifndef __EMSCRIPTEN__
np::ndarray Texture::textureToNdArray(const GLuint texID)
{
	int w, h;

	glBindTexture(GL_TEXTURE_2D, texID);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,	&w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,	&h);

	dmess("w " << w << " h " << h);

	p::tuple shape = p::make_tuple(h, w, 3);
	np::dtype dtype = np::dtype::get_builtin<float>();
	np::ndarray a = np::zeros(shape, dtype);

	glBindTexture(GL_TEXTURE_2D, texID);

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, (GLvoid *)a.get_data());

	return a;
}
#endif

