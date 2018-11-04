
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
#include <webAsmPlay/Camera.h>
#include <webAsmPlay/ShaderProgram.h>
#include <webAsmPlay/ColorDistanceShader.h>

using namespace glm;

namespace
{
    ShaderProgram * programInstance = NULL;

    ColorDistanceShader * defaultInstance = NULL;

    GLint MV_Uniform            = -1;
    GLint minVertexColorUniform = -1;
    GLint maxVertexColorUniform = -1;
    GLint minDistUniform        = -1;
    GLint maxDistUniform        = -1;
}

void ColorDistanceShader::ensureShader()
{
    dmess("ColorDistanceShader::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        
        in vec2 vertIn;
        
        uniform mat4  MVP;
        uniform mat4  MV;
        uniform vec4  minVertexColorIn;
        uniform vec4  maxVertexColorIn;
        uniform float minDistIn;
        uniform float maxDistIn;

        out vec4  minVertexColor;
        out vec4  maxVertexColor;
        out float minDist;
        out float maxDist;
        out vec4  position_in_view_space;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, 0, 1);

            position_in_view_space = MV * vert;

            gl_Position = MVP * vert;

            minVertexColor = minVertexColorIn;
            maxVertexColor = maxVertexColorIn;

            minDist = minDistIn;
            maxDist = maxDistIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        
        in vec4  minVertexColor;
        in vec4  maxVertexColor;
        in float minDist;
        in float maxDist;
        in vec4  position_in_view_space;

        out vec4 outColor;

        void main()
        {
            // computes the distance between the fragment position 
            // and the origin (4th coordinate should always be 1 
            // for points). The origin in view space is actually 
            // the camera position.
            float dist = max(0.0, distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);
            
            dist = min(maxDist, dist) / maxDist;

            outColor = minVertexColor * (1.0f - dist) + maxVertexColor * dist;
        }
    )glsl";

    programInstance = ShaderProgram::create(vertexSource,
                                            fragmentSource,
                                            StrVec({"MV",
                                                    "minVertexColorIn",
                                                    "maxVertexColorIn",
                                                    "minDistIn",
                                                    "maxDistIn"}));

    MV_Uniform            = programInstance->getUniformLoc("MV");
    minVertexColorUniform = programInstance->getUniformLoc("minVertexColorIn");
    maxVertexColorUniform = programInstance->getUniformLoc("maxVertexColorIn");
    minDistUniform        = programInstance->getUniformLoc("minDistIn");
    maxDistUniform        = programInstance->getUniformLoc("maxDistIn");

    defaultInstance = new ColorDistanceShader();
}

ColorDistanceShader * ColorDistanceShader::getDefaultInstance() { return defaultInstance ;}

ColorDistanceShader::ColorDistanceShader() : Shader  (programInstance),
                                             minColor(1,0,0,1),
                                             maxColor(0,1,0,1),
                                             minDist (0),
                                             maxDist (30)
{
}

void ColorDistanceShader::bind(const mat4 & MVP, const mat4 & MV, const bool isOutline)
{
    programInstance->ShaderProgram::bind(MVP, MV);

    programInstance->setUniform(MV_Uniform,            MV);
    programInstance->setUniform(minVertexColorUniform, minColor);
    programInstance->setUniform(maxVertexColorUniform, maxColor);
    programInstance->setUniform(minDistUniform,        minDist);
    programInstance->setUniform(maxDistUniform,        maxDist);

    glEnable   (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

vec4 ColorDistanceShader::setMinColor(const vec4 & _minColor) { return minColor = _minColor ;}
vec4 ColorDistanceShader::setMaxColor(const vec4 & _maxColor) { return maxColor = _maxColor ;}

vec4 ColorDistanceShader::getMinColor() { return minColor ;}
vec4 ColorDistanceShader::getMaxColor() { return maxColor ;}

float ColorDistanceShader::setMinDist(const float & value) { return minDist = value ;}
float ColorDistanceShader::setMaxDist(const float & value) { return maxDist = value ;}

float ColorDistanceShader::getMinDist() { return minDist ;}
float ColorDistanceShader::getMaxDist() { return maxDist ;}
