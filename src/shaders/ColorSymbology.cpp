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
#include <webAsmPlay/shaders/ColorSymbology.h>

using namespace std;
using namespace glm;

namespace
{
    //ColorSymbology * instance = NULL;

    glm::vec4 initalColors[32];

    unordered_map<string, ColorSymbology *> instances;
}

/*
void ColorSymbology::ensureInstance()
{
    if(!instance) { instance = new ColorSymbology() ;}
}
*/

//ColorSymbology * ColorSymbology::getDefaultInstance() { return instance ;}

ColorSymbology * ColorSymbology::getInstance(const string & name)
{
    unordered_map<string, ColorSymbology *>::const_iterator i = instances.find(name);

    if(i != instances.end()) { return i->second ;}

    return instances[name] = new ColorSymbology(name);
}

ColorSymbology::ColorSymbology(const string & name) : name(name)
{
    colors[0] = vec4(1,0,0,1);
    colors[1] = vec4(1,1,0,1);
    colors[2] = vec4(1,0,1,1);
    colors[3] = vec4(0,1,0,1);
    colors[4] = vec4(0,1,1,1);
    colors[5] = vec4(0,0,1,1);
    colors[6] = vec4(1,0,0,1);
    colors[7] = vec4(1,0,0,1);

    for(size_t i = 8; i < 32; ++i) { colors[i] = vec4(1,1,0,1) ;}

    colorTexture = Textures::create(colors, 32);
}

ColorSymbology::~ColorSymbology()
{

}

GLuint ColorSymbology::getTextureID()
{
    if(!colorTextureDirty) { return colorTexture ;}
    
    Textures::set1D(colorTexture, colors, 32);

    colorTextureDirty = false;

    return colorTexture;
}

vec4 ColorSymbology::setColor(const size_t index, const vec4 & color)
{
    colorTextureDirty = true;

    return colors[index] = color;
}

vec4 ColorSymbology::getColor(const size_t index) { return colors[index] ;}

vec4 & ColorSymbology::getColorRef(const size_t index) { return colors[index] ;}

void ColorSymbology::loadState(const JSONObject & dataStore)
{
    dmess("loadState " << name);

    auto setVec4 = [&dataStore](const wstring & key, vec4 & color)->void
    {
        JSONObject::const_iterator i = dataStore.find(key);

        if(i != dataStore.end()) { color = i->second->AsVec4() ;}
    };

    char buf[1024];

    for(size_t i = 0; i < 32; ++i)  
    {
        sprintf(buf, "ColorSymbology_%s_attributeColor_%i", name.c_str(), (int)i);

        setVec4(stringToWstring(buf), colors[i]);
    }

    colorTextureDirty = true;
}

void ColorSymbology::saveState(JSONObject & dataStore)
{
    dmess("saveState " << name);

    char buf[1024];

    for(size_t i = 0; i < 32; ++i)
    {
        sprintf(buf, "ColorSymbology_%s_attributeColor_%i", name.c_str(), (int)i);

        dataStore[stringToWstring(buf)] = new JSONValue(colors[i]);
    }
}