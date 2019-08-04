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
\copyright 2019
*/

#include <unordered_set>
#include <tbb/concurrent_queue.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/OpenGL_Util.h>

using namespace std;
using namespace tbb;

namespace
{
	const size_t a_numOpenGL_Contexts = 10;
	
	concurrent_queue<GLFWwindow *> a_contextWindows;

	unordered_set<thread::id> a_setContextes;
}

void APIENTRY glDebugOutput(GLenum			  source, 
							GLenum			  type, 
							GLuint			  id, 
							GLenum			  severity, 
							GLsizei			  length, 
							const GLchar	* message, 
							const void		* userParam)
{
	// ignore non-significant error/warning codes
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

	dmess("--------------------------------------------------------------------------");
	dmess("Debug message (" << id << "): " <<  message);

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:				dmess("Source: API");				break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		dmess("Source: Window System");		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:	dmess("Source: Shader Compiler");	break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:		dmess("Source: Third Party");		break;
	case GL_DEBUG_SOURCE_APPLICATION:		dmess("Source: Application");		break;
	case GL_DEBUG_SOURCE_OTHER:				dmess("Source: Other");				break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               dmess("Type: Error");				break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: dmess("Type: Deprecated Behavior"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  dmess("Type: Undefined Behavior");	break; 
	case GL_DEBUG_TYPE_PORTABILITY:         dmess("Type: Portability");			break;
	case GL_DEBUG_TYPE_PERFORMANCE:         dmess("Type: Performance");			break;
	case GL_DEBUG_TYPE_MARKER:              dmess("Type: Marker");				break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          dmess("Type: Push Group");			break;
	case GL_DEBUG_TYPE_POP_GROUP:           dmess("Type: Pop Group");			break;
	case GL_DEBUG_TYPE_OTHER:               dmess("Type: Other");				break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:			dmess("Severity: high");			break;
	case GL_DEBUG_SEVERITY_MEDIUM:			dmess("Severity: medium");			break;
	case GL_DEBUG_SEVERITY_LOW:				dmess("Severity: low");				break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	dmess("Severity: notification");	break;
	}
	dmess("--------------------------------------------------------------------------");
}

GLuint quad_vao = 0;

void OpenGL::init()
{
    //*
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    //*/

	
	//if (!quad_vao)
	{ 
		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(    quad_vao);

	} // TODO put in VertexArrayObject;

	glBindVertexArray(0);

	Shader::ensureShaders();

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	for (size_t i = 0; i < a_numOpenGL_Contexts; ++i)
	{
		a_contextWindows.push(glfwCreateWindow(1, 1, "Thread Window", NULL, GUI::getMainWindow()));
	}
}

void OpenGL::ensureSharedContext()
{
	const thread::id threadID = this_thread::get_id();

	if(a_setContextes.find(threadID) != a_setContextes.end()) { return ;}

	GLFWwindow * contextWindow = NULL;

	if(!a_contextWindows.try_pop(contextWindow)) { dmessError("Unable to create opengl shared context! Increase the number available.") }

	glfwMakeContextCurrent(contextWindow);

	a_setContextes.insert(threadID);
}