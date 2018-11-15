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

#ifndef __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_3D_H__
#define __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_3D_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <JSON.h>
#include <webAsmPlay/shaders/Shader.h>

class ColorDistanceShader3D : public Shader
{
public:

    static ColorDistanceShader3D * getDefaultInstance();

    ColorDistanceShader3D();
    ~ColorDistanceShader3D();

    static void ensureShader();

    void bind(Canvas     * canvas,
              const bool   isOutline);

    glm::vec4 setColor(const size_t index, const glm::vec4 & color);
    glm::vec4 getColor(const size_t index);
    glm::vec4 & getColorRef(const size_t index);

    void loadState(const JSONObject & dataStore);

    void saveState(JSONObject & dataStore);

    float setHeightMultiplier(const float multiplier);
    float getHeightMultiplier() const;

private:

    glm::vec4 colors[32];

    bool colorTextureDirty;

    float heightMultiplier = 1.0f;
};

#endif // __WEB_ASM_PLAY_COLOR_DISTANCE_SHADER_3D_H__