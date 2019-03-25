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

#include <thread>
#include <unordered_set>
#include <tbb/concurrent_queue.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/OpenGL_Util.h>

using namespace std;
using namespace tbb;

namespace
{
	const size_t numOpenGL_Contexts = 10;
	
	//vector<GLFWwindow *> contextWindows;

	concurrent_queue<GLFWwindow *> contextWindows;

	unordered_set<thread::id> setContextes;
}

void OpenGL::initSharedContexts()
{
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	for (size_t i = 0; i < numOpenGL_Contexts; ++i)
	{
		contextWindows.push(glfwCreateWindow(1, 1, "Thread Window", NULL, GUI::getMainWindow()));
	}
}

void OpenGL::ensureSharedContext()
{
	const thread::id threadID = this_thread::get_id();

	if(setContextes.find(threadID) != setContextes.end()) { return ;}

	GLFWwindow * contextWindow = NULL;

	if(!contextWindows.try_pop(contextWindow)) { dmessError("Unable to create opengl shared context! Increase the number available.") }

	glfwMakeContextCurrent(contextWindow);

	setContextes.insert(threadID);
}