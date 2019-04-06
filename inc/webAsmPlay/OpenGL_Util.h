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

#ifdef __EMSCRIPTEN__

    #define GLEW_STATIC
    #include <GL/glew.h>
#else

	#ifdef USE_GL_ES3
	// OpenGL ES 3
	#include <GLES3/gl3.h>  // Use GL ES 3
	#else
	// Regular OpenGL
	// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
	// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
	// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
	#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	#include <GL/gl3w.h>
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	#include <GL/glew.h>
	#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	#include <glad/glad.h>
	#else
	//#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
	#endif
	#endif

#endif

#ifdef OPENGL_CALL_CHECKING
    #define GL_CHECK(stmt) do { stmt; \
            const GLenum err = glGetError(); \
            if (err != GL_NO_ERROR) { \
            dmess("OpenGL error " << err << " call " << #stmt); \
            abort(); \
            } \
        } while (0)
#else
    #define GL_CHECK(stmt) stmt
#endif

class OpenGL
{
public:

	static void init();

	static void ensureSharedContext();

private:

	OpenGL() {}
	~OpenGL() {}
};

