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

#include <webAsmPlay/Types.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceShader3D.h>

using namespace std;
using namespace glm;

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram         * shaderProgramFill    = NULL;
    ShaderProgram         * shaderProgramOutline = NULL;
    ColorDistanceShader3D * defaultInstance      = NULL;

    GLint vertInAttrFill;
    GLint normalInAttrFill;
    GLint vertColorInAttrFill;

    GLint colorLookupOffsetFill;
    GLint heightMultiplierFill;
    GLint modelFill;
    GLint viewFill;
    GLint projectionFill;
    GLint texUniformFill;
    GLint lightPosUniformFill;

    GLint vertInAttrOutline;
    GLint vertColorInAttrOutline;
    GLint colorLookupOffsetOutline;
    GLint MV_Outline;
    GLint MVP_Outline;
    GLint texUniformOutline;
    GLint heightMultiplierOutline;
}

void ColorDistanceShader3D::ensureShader()
{
    // Shader sources
    const GLchar* vertexSourceFill = R"glsl(#version 150 core

        uniform sampler2D tex; // TODO why does the ordering matter here? Something must not be correct.

        in vec3  vertIn;
        in vec3  normalIn;
        in float vertColorIn;
        
        uniform mat4   model;
        uniform mat4   view;
        uniform mat4   projection;
        uniform float  colorLookupOffset;
        uniform float  heightMultiplier;
        
        out vec4 vertexColorNear;
        out vec4 vertexColorFar;
        out vec4 position_in_view_space;
        out vec3 normal;
        out vec3 fragPos;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

            fragPos = vec3(model * vert);

            mat4 MV = view * model;

            position_in_view_space = MV * vert;

            gl_Position = projection * MV * vert;

            vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
            vertexColorFar  = texture(tex, vec2(vertColorIn + (2.0 + colorLookupOffset) / 32.0, 0.5));

            normal = mat3(transpose(inverse(model))) * normalIn;
        }
    )glsl";

    const GLchar* fragmentSourceFill = R"glsl(#version 150 core
        
        in vec4 vertexColorNear;
        in vec4 vertexColorFar;
        in vec4 position_in_view_space;
        in vec3 normal; 
        in vec3 fragPos;
        uniform vec3 lightPos;
        
        out vec4 outColor;

        void main()
        {
            float minDist = 0.0;
            float maxDist = 5.0;
            vec3 lightColor = vec3(1,1,1);
            //vec3 objectColor = vec3(1,1,0);
            vec3 viewPos = vec3(0,0,0);

            // computes the distance between the fragment position 
            // and the origin (4th coordinate should always be 1 
            // for points). The origin in view space is actually 
            // the camera position.
            float dist = max(0.0, distance(position_in_view_space, vec4(0.0, 0.0, 0.0, 1.0)) + minDist);
            
            dist = min(maxDist, dist) / maxDist;

            vec4 objectColor = vertexColorNear * (1.0f - dist) + vertexColorFar * dist;

            vec3 lightDir = normalize(lightPos - fragPos);

            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            vec3 result = diffuse * vec3(objectColor);
            outColor = vec4(result, objectColor.w);
        }
    )glsl";

    shaderProgramFill = ShaderProgram::create(  vertexSourceFill,
                                                fragmentSourceFill,
                                                Variables({{"vertIn",               vertInAttrFill          },
                                                           {"vertColorIn",          vertColorInAttrFill     },
                                                           {"normalIn",             normalInAttrFill        }}),
                                                Variables({{"tex",                  texUniformFill          },
                                                           {"model",                modelFill               },
                                                           {"view",                 viewFill                },
                                                           {"projection",           projectionFill          },
                                                           {"colorLookupOffset",    colorLookupOffsetFill   },
                                                           {"heightMultiplier",     heightMultiplierFill    },
                                                           {"lightPos",             lightPosUniformFill     }}));

    const GLchar* vertexSourceOutline = R"glsl(#version 150 core
        uniform sampler2D tex;

        in vec3  vertIn;
        in float vertColorIn;
        
        uniform mat4  MVP;
        uniform mat4  MV;
        uniform float colorLookupOffset;
        uniform float heightMultiplier;
        
        out vec4 vertexColorNear;
        out vec4 vertexColorFar;
        out vec4 position_in_view_space;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

            position_in_view_space = MV * vert;

            gl_Position = MVP * vert;

            vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
            vertexColorFar = texture(tex, vec2(vertColorIn + (1.0 + colorLookupOffset) / 32.0, 0.5));
        }
    )glsl";

    const GLchar* fragmentSourceOutline = R"glsl(#version 150 core
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
            //outColor = vec4(1,1,1,1);
        }
    )glsl";

    shaderProgramOutline = ShaderProgram::create(   vertexSourceOutline,
                                                    fragmentSourceOutline,
                                                    Variables({{"vertIn",            vertInAttrOutline          },
                                                               {"vertColorIn",       vertColorInAttrOutline     }}),
                                                    Variables({{"MV",                MV_Outline                 },
                                                               {"MVP",               MVP_Outline                },
                                                               {"tex",               texUniformOutline          },
                                                               {"colorLookupOffset", colorLookupOffsetOutline   },
                                                               {"heightMultiplier",  heightMultiplierOutline    }}));

    defaultInstance = new ColorDistanceShader3D();
}

ColorDistanceShader3D::ColorDistanceShader3D() : Shader("ColorDistanceShader3D")
{
}

ColorDistanceShader3D::~ColorDistanceShader3D()
{

}

ColorDistanceShader3D * ColorDistanceShader3D::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceShader3D::bind(Canvas     * canvas,
                                 const bool   isOutline,
                                 const size_t renderingStage)
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, colorSymbology->getTextureID()));

    if(!isOutline)
    {
        shaderProgramFill->bind();

        ShaderProgram::enableVertexAttribArray( vertInAttrFill,
                                                vertexFormat.size,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                vertexFormat.stride,
                                                vertexFormat.pointer);

        ShaderProgram::enableVertexAttribArray( normalInAttrFill,
                                                normalFormat.size,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                normalFormat.stride,
                                                normalFormat.pointer);

        ShaderProgram::enableVertexAttribArray( vertColorInAttrFill,
                                                colorFormat.size,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                colorFormat.stride,
                                                colorFormat.pointer);

        shaderProgramFill->setUniformi(texUniformFill,          0);
        shaderProgramFill->setUniformf(heightMultiplierFill,    heightMultiplier);
        shaderProgramFill->setUniform (modelFill,               canvas->getModelRef());
        shaderProgramFill->setUniform (viewFill,                canvas->getViewRef());
        shaderProgramFill->setUniform (projectionFill,          canvas->getProjectionRef());
        shaderProgramFill->setUniform (lightPosUniformFill,     lightPos);

        shaderProgramFill->setUniformf(colorLookupOffsetFill, 0.0f);
    }
    else
    {
        shaderProgramOutline->bind();

        ShaderProgram::enableVertexAttribArray( vertInAttrOutline,
                                                vertexFormat.size,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                vertexFormat.stride,
                                                vertexFormat.pointer);

        ShaderProgram::enableVertexAttribArray( vertColorInAttrOutline,
                                                colorFormat.size,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                colorFormat.stride,
                                                colorFormat.pointer);

        shaderProgramOutline->setUniformf(heightMultiplierOutline,  heightMultiplier);
        shaderProgramOutline->setUniform(MV_Outline,                canvas->getMV_Ref());
        shaderProgramOutline->setUniform(MVP_Outline,               canvas->getMVP_Ref());
        shaderProgramOutline->setUniformi(texUniformOutline,        0);

        shaderProgramOutline->setUniformf(colorLookupOffsetOutline, 1.0f);
    }
}

float ColorDistanceShader3D::setHeightMultiplier(const float multiplier) { return heightMultiplier = multiplier ;}

float ColorDistanceShader3D::getHeightMultiplier() const { return heightMultiplier ;}

vec3 ColorDistanceShader3D::setLightPos(const vec3 & pos) { return lightPos = pos ;}

vec3 ColorDistanceShader3D::getLightPos() const { return lightPos ;}

ColorSymbology * ColorDistanceShader3D::setColorSymbology(ColorSymbology * colorSymbology) { return this->colorSymbology = colorSymbology ;}
ColorSymbology * ColorDistanceShader3D::getColorSymbology() const { return colorSymbology ;}