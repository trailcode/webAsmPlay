
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
  \copyright 2018
*/

#ifndef __EMSCRIPTEN__
#include <boost/filesystem.hpp>
using namespace boost;
#endif

#include <codecvt>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/Util.h>

using namespace std;
using namespace std::chrono;
using namespace glm;

string toStr(const mat4 & m)
{
    char buf[1024];

    sprintf(buf,
            "[[% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]]\n",
            static_cast<double>(m[0][0]), static_cast<double>(m[0][1]), static_cast<double>(m[0][2]), static_cast<double>(m[0][3]),
            static_cast<double>(m[1][0]), static_cast<double>(m[1][1]), static_cast<double>(m[1][2]), static_cast<double>(m[1][3]),
            static_cast<double>(m[2][0]), static_cast<double>(m[2][1]), static_cast<double>(m[2][2]), static_cast<double>(m[2][3]),
            static_cast<double>(m[3][0]), static_cast<double>(m[3][1]), static_cast<double>(m[3][2]), static_cast<double>(m[3][3]));

    return buf;
}

string toStr(const vec2 & v)
{
    char buf[1024];

    sprintf(buf, "[% 2.9f, % 2.9f]\n", v.x, v.y);

    return buf;
}

string toStr(const ivec2 & v)
{
    char buf[1024];

    sprintf(buf, "[%i, %i]\n", v.x, v.y);

    return buf;
}

string toStr(const vec3 & v)
{
    char buf[1024];

    sprintf(buf, "[% 2.9f, % 2.9f, % 2.9f]\n", v.x, v.y, v.z);

    return buf;
}

string toStr(const vec4 & v)
{
    char buf[1024];

    sprintf(buf, "[% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n", v.x, v.y, v.z, v.w);

    return buf;
}

wstring stringToWstring(const string& t_str)
{
    //setup converter
    typedef codecvt_utf8<wchar_t> convert_type;

    wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes(t_str);
}


void doProgress( const string             & message,
                 const size_t               i,
                 const size_t               num,
                 time_point<system_clock> & startTime,
                 const size_t               sampleRate)
{
    if(i % sampleRate) { return ;}

    const auto now = std::chrono::system_clock::now();

    if((now - startTime).count() > 20000)
    {
        GUI::progress(message, float(i) / float(num));

        startTime = now;

        //GUI::refresh();
    }
}

bool fileExists(const string & fileName)
{
#ifndef __EMSCRIPTEN__

	return filesystem::exists(fileName);

#else

	abort();

#endif
}