
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

#ifndef __EMSCRIPTEN__
    #include <curl/curl.h>
    #include <ctpl.h>
#endif

#include <thread>
#include <unordered_map>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/CurlUtil.h>

using namespace std;
using namespace curlUtil;

namespace
{
	mutex loaderMutex;

	unordered_map<int, CURL *> a_curlHandles;

	
}

BufferStruct::~BufferStruct()
{
	free(m_buffer);
}

// This is the function we pass to LC, which writes the output to a BufferStruct
size_t curlUtil::writeMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	const size_t realsize = size * nmemb;

	struct BufferStruct * mem = (struct BufferStruct *) data;

	mem->m_buffer = (char *)realloc(mem->m_buffer, mem->m_size + realsize + 1);

	if (!mem->m_buffer) { return realsize ;}
	
	memcpy(&(mem->m_buffer[mem->m_size]), ptr, realsize);
	mem->m_size += realsize;
	mem->m_buffer[mem->m_size] = 0;

	return realsize;
}

BufferStruct * curlUtil::download(const string & url, const size_t threadID)
{
	return 0;	
}