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

#include <unordered_map>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/ColorSymbology.h>

using namespace std;
using namespace glm;

namespace
{
    glm::vec4 initalColors[32];

    unordered_map<string, ColorSymbology *> instances;
}

ColorSymbology * ColorSymbology::getInstance(const string & name)
{
    unordered_map<string, ColorSymbology *>::const_iterator i = instances.find(name);

    if(i != instances.end()) { return i->second ;}

    return instances[name] = new ColorSymbology(name);
}

ColorSymbology::ColorSymbology(const string & name) : m_name(name)
{
    m_colors[0] = vec4(1,0,0,1);
    m_colors[1] = vec4(1,1,0,1);
    m_colors[2] = vec4(1,0,1,1);
    m_colors[3] = vec4(0,1,0,1);
    m_colors[4] = vec4(0,1,1,1);
    m_colors[5] = vec4(0,0,1,1);
    m_colors[6] = vec4(1,0,0,1);
    m_colors[7] = vec4(1,0,0,1);

    for(size_t i = 8; i < 32; ++i) { m_colors[i] = vec4(1,1,0,1) ;}

    m_colorTexture = Textures::create(m_colors, 32);
}

ColorSymbology::~ColorSymbology()
{

}

GLuint ColorSymbology::getTextureID()
{
    if(!m_colorTextureDirty) { return m_colorTexture ;}
    
    Textures::set1D(m_colorTexture, m_colors, 32);

    m_colorTextureDirty = false;

    return m_colorTexture;
}

vec4 ColorSymbology::setColor(const size_t index, const vec4 & color)
{
    m_colorTextureDirty = true;

    return m_colors[index] = color;
}

vec4 ColorSymbology::getColor(const size_t index) { return m_colors[index] ;}

vec4 & ColorSymbology::getColorRef(const size_t index) { return m_colors[index] ;}

void ColorSymbology::loadState(const JSONObject & dataStore)
{
    auto setVec4 = [&dataStore](const wstring & key, vec4 & color)->void
    {
        JSONObject::const_iterator i = dataStore.find(key);

        if(i != dataStore.end()) { color = i->second->AsVec4() ;}
    };

    char buf[1024];

    for(size_t i = 0; i < 32; ++i)  
    {
        sprintf(buf, "ColorSymbology_%s_attributeColor_%i", m_name.c_str(), (int)i);

        setVec4(stringToWstring(buf), m_colors[i]);
    }

    m_colorTextureDirty = true;
}

void ColorSymbology::saveState(JSONObject & dataStore)
{
    char buf[1024];

    for(size_t i = 0; i < 32; ++i)
    {
        sprintf(buf, "ColorSymbology_%s_attributeColor_%i", m_name.c_str(), (int)i);

        dataStore[stringToWstring(buf)] = new JSONValue(m_colors[i]);
    }
}