#ifndef __WEB_ASM_PLAY_UTIL_H__
#define __WEB_ASM_PLAY_UTIL_H__

#include <string>
#include <glm/glm.hpp>
#include <tceGeom/vec2.h>
#include <imgui.h>

std::string mat4ToStr(const glm::mat4 & m);

static tce::geom::Vec2i __(const ImVec2 & v) { return tce::geom::Vec2i(v.x, v.y) ;}

#endif // __WEB_ASM_PLAY_UTIL_H__