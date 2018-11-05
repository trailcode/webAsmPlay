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

#ifndef __WEB_ASM_PLAY_UTIL_H__
#define __WEB_ASM_PLAY_UTIL_H__

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <OpenSteer/Vec3.h>
#include <OpenSteer/Color.h>
#include <imgui.h>

std::string mat4ToStr(const glm::mat4 & m);

static glm::ivec2 __(const ImVec2 & v) { return glm::ivec2(v.x, v.y) ;}

static ImVec4 __(const glm::vec4 & v) { return ImVec4(v.x, v.y, v.z, v.q) ;}

static glm::vec4 __(const ImVec4 & v) { return glm::vec4(v.x, v.y, v.z, v.w) ;}

static glm::vec3 __(const OpenSteer::Vec3 & v) { return glm::vec3(v.x, v.y, v.z) ;}

static glm::vec4 __(const OpenSteer::Color & v) { return glm::vec4(v.r(), v.g(), v.b(), v.a()) ;}

std::wstring stringToWstring(const std::string& t_str);

#endif // __WEB_ASM_PLAY_UTIL_H__