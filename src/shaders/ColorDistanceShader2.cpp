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

#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceShader2.h>

using namespace std;
using namespace glm;

namespace
{
    ShaderProgram * shaderProgram = NULL;

    ColorDistanceShader2 * defaultInstance = NULL;

    GLint colorsInLoc = -1;

    GLint colorLookupOffsetLoc = -1;

    GLint MV_Uniform = -1;

    GLuint texUniform = -1;

    GLuint colorTexture = 0;

    glm::vec4 initalColors[32];
}

void ColorDistanceShader2::ensureShader()
{
    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 150 core
        uniform sampler2D tex;

        in vec2 vertIn;
        in float vertColorIn;
        
        uniform mat4 MVP;
        uniform mat4 MV;
        uniform float colorLookupOffset;
        
        out vec4 vertexColorNear;
        out vec4 vertexColorFar;
        out vec4 position_in_view_space;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, 0, 1);

            position_in_view_space = MV * vert;

            gl_Position = MVP * vert;

            vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
            vertexColorFar = texture(tex, vec2(vertColorIn + (1.0 + colorLookupOffset) / 32.0, 0.5));
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 150 core
        in vec4 vertexColorNear;
        in vec4 vertexColorFar;
        in vec4 position_in_view_space;

        out vec4 outColor;

        void main()
        {
            float minDist = 0.0;
            float maxDist = 5.0;

            // computes the distance between the fragment position 
            // and the origin (4th coordinate should always be 1 
            // for points). The origin in view space is actually 
            // the camera position.
            float dist = max(0.0, distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);
            
            dist = min(maxDist, dist) / maxDist;

            outColor = vertexColorNear * (1.0f - dist) + vertexColorFar * dist;
        }
    )glsl";

    colorTexture = Textures::create(initalColors, 32);

    shaderProgram = ShaderProgram::create(  vertexSource,
                                fragmentSource,
                                StrVec({"MV", "colorsIn", "tex", "colorLookupOffset"}));

    colorLookupOffsetLoc = shaderProgram->getUniformLoc("colorLookupOffset");

    MV_Uniform  = shaderProgram->getUniformLoc("MV");

    colorsInLoc = shaderProgram->getUniformLoc("colorsIn");

    texUniform = shaderProgram->getUniformLoc("tex");

    defaultInstance = new ColorDistanceShader2();
}

ColorDistanceShader2::ColorDistanceShader2() : Shader(shaderProgram)
{
    colors[0] = vec4(1,0,0,1);
    colors[1] = vec4(1,1,0,1);
    colors[2] = vec4(1,0,1,1);
    colors[3] = vec4(0,1,0,1);
    colors[4] = vec4(0,1,1,1);
    colors[5] = vec4(0,0,1,1);
    colors[6] = vec4(1,0,0,1);
    colors[7] = vec4(1,0,0,1);
}

ColorDistanceShader2::~ColorDistanceShader2()
{

}

ColorDistanceShader2 * ColorDistanceShader2::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceShader2::bind(const mat4 & MVP, const mat4 & MV, const bool isOutline)
{
    if(colorTextureDirty)
    {
        Textures::set1D(colorTexture, colors, 32);

        colorTextureDirty = false;
    }

    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, colorTexture));

    shaderProgram->bind(MVP, MV);

    shaderProgram->setUniform(MV_Uniform, MV);

    shaderProgram->enableVertexAttribArray(2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    shaderProgram->enableColorAttribArray(1, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(2 * sizeof(GLuint)));

    shaderProgram->setUniformi(texUniform, 0);

    if(isOutline) { shaderProgram->setUniform(colorLookupOffsetLoc, 2.0f) ;}
    else          { shaderProgram->setUniform(colorLookupOffsetLoc, 0.0f) ;}
}

vec4 ColorDistanceShader2::setColor(const size_t index, const vec4 & color)
{
    colorTextureDirty = true;

    return colors[index] = color;
}

vec4 ColorDistanceShader2::getColor(const size_t index) { return colors[index] ;}

vec4 & ColorDistanceShader2::getColorRef(const size_t index) { return colors[index] ;}

void ColorDistanceShader2::loadState(const JSONObject & dataStore)
{
    dmess("ColorDistanceShader2::loadState");

    auto setVec4 = [&dataStore](const wstring & key, vec4 & color)->void
    {
        JSONObject::const_iterator i = dataStore.find(key);

        if(i != dataStore.end()) { color = i->second->AsVec4() ;}
    };

    char buf[1024];

    for(size_t i = 0; i < 32; ++i)
    {
        sprintf(buf, "attributeColor_%i", (int)i);

        setVec4(stringToWstring(buf), colors[i]);
    }

    colorTextureDirty = true;
}

void ColorDistanceShader2::saveState(JSONObject & dataStore)
{
    char buf[1024];

    for(size_t i = 0; i < 32; ++i)
    {
        sprintf(buf, "attributeColor_%i", (int)i);

        dataStore[stringToWstring(buf)] = new JSONValue(colors[i]);
    }
}

