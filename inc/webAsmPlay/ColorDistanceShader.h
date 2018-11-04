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

#ifndef __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_H__
#define __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <webAsmPlay/Shader.h>

class ColorDistanceShader : public Shader
{
public:

    static void ensureShader();

    void bind(const glm::mat4 & MVP, const glm::mat4 & MV, const bool isOutline);

    static glm::vec4 setMinColor(const glm::vec4 & minColor);
    static glm::vec4 setMaxColor(const glm::vec4 & maxColor);

    static glm::vec4 getMinColor();
    static glm::vec4 getMaxColor();

    static float setMinDist(const float & value);
    static float setMaxDist(const float & value);

    static float getMinDist();
    static float getMaxDist();

private:

    ColorDistanceShader();
    ~ColorDistanceShader() {}
};

#endif // __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_H__
