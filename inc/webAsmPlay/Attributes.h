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

#ifndef __WEB_ASM_PLAY_ATTRIBUTES_H__
#define __WEB_ASM_PLAY_ATTRIBUTES_H__

#include <string>
#include <ostream>
#include <unordered_map>

class Attributes
{
public:

    Attributes();
    Attributes(const char *& attrs);

    void write(std::ostream & out) const;

    std::string toString() const;

    bool hasStringKey(const std::string & key) const;

    bool hasStringKeyValue(const std::string & key, const std::string & value) const;

    typedef std::unordered_map<std::string, int32_t>        Ints32;
    typedef std::unordered_map<std::string, int64_t>        Ints64;
    typedef std::unordered_map<std::string, double >        Doubles;
    typedef std::unordered_map<std::string, std::string>    Strings;

    Ints32  ints32;
    Ints64  ints64;
    Doubles doubles;
    Strings strings;
};

#endif // __WEB_ASM_PLAY_ATTRIBUTES_H__