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

#include <webAsmPlay/Util.h>
#include <webAsmPlay/Types.h>
#include <webAsmPlay/Canvas.h>
#include <webAsmPlay/shaders/ColorSymbology.h>
#include <webAsmPlay/shaders/ShaderProgram.h>
#include <webAsmPlay/shaders/ColorDistanceDepthShader3D.h>

using namespace std;
using namespace glm;

// TODO A lot of code duplication! 

namespace
{
    ShaderProgram              * shaderProgramDepth    = NULL;
    ShaderProgram              * shaderProgramFill     = NULL;
    ShaderProgram              * shaderProgramOutline  = NULL;
    ColorDistanceDepthShader3D * defaultInstance       = NULL;

    GLint vertInAttrDepth;
    GLint heightMultiplierDepth;
    GLint modelDepth;
    GLint viewDepth;
    GLint projectionDepth;

    GLint vertInAttrFill;
    GLint normalInAttrFill;
    GLint vertColorInAttrFill;

    GLint colorLookupOffsetFill;
    GLint heightMultiplierFill;
    GLint modelFill;
    GLint viewFill;
    GLint projectionFill;
    GLint texUniformFill;
    GLint depthTexUniformFill;
    GLint lightPosUniformFill;
    GLint widthUniformFill;
    GLint heightUniformFill;
    
    GLint vertInAttrOutline;
    GLint vertColorInAttrOutline;
    GLint colorLookupOffsetOutline;
    GLint MV_Outline;
    GLint MVP_Outline;
    GLint texUniformOutline;
    GLint depthTexUniformOutline;
    GLint heightMultiplierOutline;
    GLint widthUniformOutline;
    GLint heightUniformOutline;
}

void ColorDistanceDepthShader3D::ensureShader()
{
    const GLchar* vertexSourceDepth = R"glsl(#version 150 core

        in vec3  vertIn;
        
        uniform mat4   model;
        uniform mat4   view;
        uniform mat4   projection;
        uniform float  heightMultiplier;

        out vec4 glPos;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

            gl_Position = projection * view * model * vert;

            glPos = gl_Position;
        }
    )glsl";

    const GLchar* fragmentSourceDepth = R"glsl(#version 150 core
        in  vec4 glPos;
        out vec4 outColor;

        void main()
        {
            //outColor = vec4(1,1,1,1);
            outColor = vec4(glPos.w, glPos.w, glPos.w, 1);
        }
    )glsl";

    shaderProgramDepth = ShaderProgram::create( vertexSourceDepth,
                                                fragmentSourceDepth,
                                                Variables({{"vertIn",               vertInAttrDepth       }}),
                                                Variables({{"model",                modelDepth            },
                                                           {"view",                 viewDepth             },
                                                           {"projection",           projectionDepth       },
                                                           {"heightMultiplier",     heightMultiplierDepth }}));

    // Shader sources
    const GLchar* vertexSourceFill = R"glsl(#version 150 core

        in vec3  vertIn;
        in vec3  normalIn;
        in float vertColorIn;
        
        uniform mat4      model;
        uniform mat4      view;
        uniform mat4      projection;
        uniform float     colorLookupOffset;
        uniform float     heightMultiplier;
        uniform sampler2D tex;
        
        out vec4 vertexColorNear;
        out vec4 vertexColorFar;
        out vec4 position_in_view_space;
        out vec3 normal;
        out vec3 fragPos;
        out vec4 glPos;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

            fragPos = vec3(model * vert);

            mat4 MV = view * model;

            position_in_view_space = MV * vert;

            gl_Position = projection * MV * vert;

            glPos = gl_Position; // TODO just use gl_Position?

            vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
            vertexColorFar  = texture(tex, vec2(vertColorIn + (2.0 + colorLookupOffset) / 32.0, 0.5));

            normal = mat3(transpose(inverse(model))) * normalIn;
        }
    )glsl";

    const GLchar* fragmentSourceFill = R"glsl(#version 150 core
        uniform sampler2D depthTex;
        in vec4 vertexColorNear;
        in vec4 vertexColorFar;
        in vec4 position_in_view_space;
        in vec3 normal; 
        in vec3 fragPos;
        in vec4 glPos;

        uniform vec3      lightPos;
        uniform float     width;
        uniform float     height;
        
        
        out vec4 outColor;

        void main()
        {
            vec4 t = texture(depthTex, vec2(gl_FragCoord.x / width, gl_FragCoord.y / height));

            float v = abs(t.x - glPos.w);

            if(v > 0.0001)
            {
                discard;
            }

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
                                                           {"depthTex",             depthTexUniformFill     },
                                                           {"model",                modelFill               },
                                                           {"view",                 viewFill                },
                                                           {"projection",           projectionFill          },
                                                           {"colorLookupOffset",    colorLookupOffsetFill   },
                                                           {"heightMultiplier",     heightMultiplierFill    },
                                                           {"lightPos",             lightPosUniformFill     },
                                                           {"width",                widthUniformFill        },
                                                           {"height",               heightUniformFill       }}));

    const GLchar* vertexSourceOutline = R"glsl(#version 150 core

        in vec3  vertIn;
        in float vertColorIn;
        
        uniform mat4      MVP;
        uniform mat4      MV;
        uniform float     colorLookupOffset;
        uniform float     heightMultiplier;
        uniform sampler2D tex; // TODO Rename, and the one above

        out vec4 vertexColorNear;
        out vec4 vertexColorFar;
        out vec4 position_in_view_space;
        out vec4 glPos;

        void main()
        {
            vec4 vert = vec4(vertIn.xy, vertIn.z * heightMultiplier, 1);

            position_in_view_space = MV * vert;

            gl_Position = MVP * vert;

            glPos = gl_Position;

            vertexColorNear = texture(tex, vec2(vertColorIn + colorLookupOffset / 32.0, 0.5));
            vertexColorFar = texture(tex, vec2(vertColorIn + (1.0 + colorLookupOffset) / 32.0, 0.5));
        }
    )glsl";

    const GLchar* fragmentSourceOutline = R"glsl(#version 150 core
        in vec4 vertexColorNear;
        in vec4 vertexColorFar;
        in vec4 position_in_view_space;
        in vec4 glPos;

        out vec4 outColor;

        uniform float     width;
        uniform float     height;
        uniform sampler2D depthTex;

        bool canDiscard()
        {
            float posX = gl_FragCoord.x / width;
            float posY = gl_FragCoord.y / height;
            float deltaX = 1.0f / width;
            float deltaY = 1.0f / height;
            for(float x = -1.0f; x < 2.0f; x += 1.0f) 
            for(float y = -1.0f; y < 2.0f; y += 1.0f)
            {
                vec4 t = texture(depthTex, vec2(posX + x * deltaX, posY + y * deltaY));

                float v = abs(t.x - glPos.w);

                if(v <= 0.0001) { return false ;}
            }

            return true;
        }

        void main()
        {
            if(canDiscard()) { discard ;}

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

    shaderProgramOutline = ShaderProgram::create(   vertexSourceOutline,
                                                    fragmentSourceOutline,
                                                    Variables({{"vertIn",            vertInAttrOutline          },
                                                               {"vertColorIn",       vertColorInAttrOutline     }}),
                                                    Variables({{"MV",                MV_Outline                 },
                                                               {"MVP",               MVP_Outline                },
                                                               {"tex",               texUniformOutline          },
                                                               {"depthTex",          depthTexUniformOutline     },
                                                               {"colorLookupOffset", colorLookupOffsetOutline   },
                                                               {"heightMultiplier",  heightMultiplierOutline    },
                                                               {"width",             widthUniformOutline        },
                                                               {"height",            heightUniformOutline       }
                                                               }));

    defaultInstance = new ColorDistanceDepthShader3D();
}

ColorDistanceDepthShader3D::ColorDistanceDepthShader3D() : Shader("ColorDistanceDepthShader3D")
{
}

ColorDistanceDepthShader3D::~ColorDistanceDepthShader3D()
{

}

ColorDistanceDepthShader3D * ColorDistanceDepthShader3D::getDefaultInstance() { return defaultInstance ;}

void ColorDistanceDepthShader3D::bind(Canvas     * canvas,
                                      const bool   isOutline,
                                      const size_t renderingStage)
{
    switch(renderingStage)
    {
        case 0: return bindStage0(canvas, isOutline);
        case 1: return bindStage1(canvas, isOutline);
        default: 
            dmess("Error!");
            abort();
    }
}

void ColorDistanceDepthShader3D::bindStage1(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage1");

    shaderProgramDepth->bind();

    GL_CHECK(glEnable(GL_DEPTH_TEST));

    ShaderProgram::enableVertexAttribArray( vertInAttrDepth,
                                            sizeVertex,
                                            GL_FLOAT,
                                            GL_FALSE,
                                            strideVertex,
                                            pointerVertex);

    shaderProgramDepth->setUniformf(heightMultiplierDepth,    heightMultiplier);
    shaderProgramDepth->setUniform (modelDepth,               canvas->getModelRef());
    shaderProgramDepth->setUniform (viewDepth,                canvas->getViewRef());
    shaderProgramDepth->setUniform (projectionDepth,          canvas->getProjectionRef());
}

#include <webAsmPlay/FrameBuffer.h>

void ColorDistanceDepthShader3D::bindStage0(Canvas * canvas, const bool isOutline)
{
    //dmess("ColorDistanceDepthShader3D::bindStage0");

    GL_CHECK(glActiveTexture(GL_TEXTURE0));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, colorSymbology->getTextureID()));

    GL_CHECK(glActiveTexture(GL_TEXTURE1));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, canvas->auxFrameBuffer->getTextureID()));

    GL_CHECK(glEnable(GL_BLEND));

    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    GL_CHECK(glDisable(GL_DEPTH_TEST));

    if(!isOutline)
    {
        shaderProgramFill->bind();

        ShaderProgram::enableVertexAttribArray( vertInAttrFill,
                                                sizeVertex,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                strideVertex,
                                                pointerVertex);

        ShaderProgram::enableVertexAttribArray( normalInAttrFill,
                                                sizeNormal,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                strideNormal,
                                                pointerNormal);

        ShaderProgram::enableVertexAttribArray( vertColorInAttrFill,
                                                sizeColor,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                strideColor,
                                                pointerColor);

        shaderProgramFill->setUniformi(texUniformFill,          0);
        shaderProgramFill->setUniformi(depthTexUniformFill,          1);
        shaderProgramFill->setUniformf(widthUniformFill,          canvas->frameBufferSize.x);
        shaderProgramFill->setUniformf(heightUniformFill,          canvas->frameBufferSize.y);
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
                                                sizeVertex,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                strideVertex,
                                                pointerVertex);

        ShaderProgram::enableVertexAttribArray( vertColorInAttrOutline,
                                                sizeColor,
                                                GL_FLOAT,
                                                GL_FALSE,
                                                strideColor,
                                                pointerColor);

        //shaderProgramOutline->colorLookupOffsetOutline;
        shaderProgramOutline->setUniformf(heightMultiplierOutline,    heightMultiplier);
        shaderProgramOutline->setUniformi(depthTexUniformOutline,          1);
        shaderProgramOutline->setUniformf(widthUniformOutline,          canvas->frameBufferSize.x);
        shaderProgramOutline->setUniformf(heightUniformOutline,          canvas->frameBufferSize.y);
        shaderProgramOutline->setUniform(MV_Outline, canvas->getMV_Ref());
        shaderProgramOutline->setUniform(MVP_Outline, canvas->getMVP_Ref());
        shaderProgramOutline->setUniformi(texUniformOutline, 0);

        shaderProgramOutline->setUniformf(colorLookupOffsetOutline, 1.0f);
    }

    //if(isOutline) { shaderProgramFill->setUniformf(colorLookupOffsetFill, 1.0f) ;}
    //else          { shaderProgramFill->setUniformf(colorLookupOffsetFill, 0.0f) ;}
}

bool ColorDistanceDepthShader3D::shouldRender(const bool isOutline, const size_t renderingStage) const
{
    if(renderingStage == 0) { return true ;}

    if(renderingStage == 1 && !isOutline) { return true ;}

    return false;
}

float ColorDistanceDepthShader3D::setHeightMultiplier(const float multiplier) { return heightMultiplier = multiplier ;}

float ColorDistanceDepthShader3D::getHeightMultiplier() const { return heightMultiplier ;}

vec3 ColorDistanceDepthShader3D::setLightPos(const vec3 & pos) { return lightPos = pos ;}

vec3 ColorDistanceDepthShader3D::getLightPos() const { return lightPos ;}

size_t ColorDistanceDepthShader3D::getNumRenderingStages() const { return 2 ;}

ColorSymbology * ColorDistanceDepthShader3D::setColorSymbology(ColorSymbology * colorSymbology) { return this->colorSymbology = colorSymbology ;}
ColorSymbology * ColorDistanceDepthShader3D::getColorSymbology() const { return colorSymbology ;}
