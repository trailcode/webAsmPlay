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
#pragma once

#ifndef __EMSCRIPTEN__
#include <boost/python/numpy.hpp>
#endif
#include <atomic>
#include <string>
#include <unordered_map>
#include <webAsmPlay/OpenGL_Util.h>

class Texture
{
public:

	Texture(const std::string & ID);

	virtual ~Texture();

	bool textureReady() const;

	virtual std::string getDownloadURL() const = 0;

	void readyTexture();

	static size_t pruneTextures();

	static size_t getNumTextures();
	static size_t getNumLoading();
	static size_t getNumDownloading();
	static size_t getNumUploading();
	static size_t getNumWriting();
	static size_t getNumCacheHits();
	static size_t getNumCacheMisses();

	static void incrementFrameNumber();

	static size_t getFrameNumber();

#ifndef __EMSCRIPTEN__

	static boost::python::numpy::ndarray textureToNdArray(const GLuint texID);

#endif

	static GLuint s_NO_DATA;

	static std::atomic_size_t s_desiredMaxNumTextures;

	static bool s_useCache;

	static bool s_useBindlessTextures;

	std::atomic<GLuint> m_textureID = { 0 };

	std::atomic_bool m_loading = { false };

	std::atomic_bool m_stillNeeded = { true };

	size_t m_lastAccessTime = 0;

	GLuint64 m_handle = 0;

	bool m_textureResident = false;

	const std::string m_ID;

protected:

	static std::unordered_map<std::string, Texture *> s_textures;

private:

	void readyTexture(const int ID);

	void markNoData();
};
