#pragma once


#include <glbinding/Boolean8.h>

#include <glbinding/glbinding_api.h>
#include <glbinding/glbinding_features.h>
#include <glbinding/nogl.h>


namespace gl
{


using GLboolean = glbinding::Boolean8;


} // namespace gl

/*
#ifdef __EMSCRIPTEN__
#ifdef GL_FALSE
#undef GL_FALSE
#endif
#ifdef GL_TRUE
#undef GL_TRUE
#endif
#endif
*/

namespace gl
{

#ifndef __EMSCRIPTEN__
// import booleans to namespace
GLBINDING_CONSTEXPR static const GLboolean GL_FALSE = GLboolean(0);
GLBINDING_CONSTEXPR static const GLboolean GL_TRUE = GLboolean(1);
#endif
} // namespace gl
