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
#pragma once

#ifndef __EMSCRIPTEN__

#include <webAsmPlay/shaders/Shader.h>

class ColorDistanceDepthShader3D : public Shader
{
public:

    static ColorDistanceDepthShader3D * getDefaultInstance();

    ColorDistanceDepthShader3D(ColorSymbology * colorSymbology = nullptr);
	
    ~ColorDistanceDepthShader3D();

    static void ensureShader();

    void bind(Canvas     * canvas,
              const bool   isOutline,
              const size_t renderingStage = 0) override;

    glm::vec3 setLightPos(const glm::vec3 & pos);
    glm::vec3 getLightPos() const;

    float setHeightMultiplier(const float multiplier);
    float getHeightMultiplier() const;

    size_t getNumRenderingStages() const override;

private:

    void bindStagePostG_Buffer(Canvas * canvas, const bool isOutline);
    void bindStageG_Buffer(Canvas * canvas, const bool isOutline);

    float m_heightMultiplier = 1.0f;

    glm::vec3 m_lightPos;
};

#endif
