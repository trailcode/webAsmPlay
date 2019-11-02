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

#include <unordered_set>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include <OpenSteer/OpenSteerDemo.h>
#include <OpenSteer/Draw.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/canvas/Camera.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/ZombiePlugin.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/shaders/Shader.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/OpenSteerGlue.h>

#ifdef max
#undef max
#endif

using namespace std;
using namespace glm;
using namespace OpenSteer;
using namespace geosUtil;

void openSteerDisplayFunc();

thread * openSteerThread = nullptr; // TODO make a thread waiting collection.

extern bool gotoNextZombie; // TODO Un-globalize

namespace
{
	class MyCleanUp
	{
	public:

		~MyCleanUp()
		{
			if(!openSteerThread) { return ;}

			if(openSteerThread->joinable()) { openSteerThread->join() ;}

			delete openSteerThread;
		}
	};

	static MyCleanUp s_myCleanup;

    const double a_scaleValue = 1.0 / (60.0 * 2.0);

    const dmat4 a_geomTrans(scale(dmat4(1.0), dvec3(a_scaleValue, a_scaleValue, a_scaleValue)));

    const dmat4 a_geomInverseTrans(inverse(a_geomTrans));

    unique_ptr<Renderable> a_openSteerGeom;

    mutex a_openSteerMutex;

    vec4 a_lookat;
    vec4 a_pos;   
    vec4 a_up;

    void updateOpenSteerCamera()
    {
        const vec3 scale(0.1, 0.1, 0.1);
            
        const dmat4 rotate = glm::rotate(radians(-90.0), dvec3(1, 0, 0));

        a_lookat = rotate * a_geomTrans * vec4(__(OpenSteerDemo::camera.target)     * scale, 1);
        a_pos    = rotate * a_geomTrans * vec4(__(OpenSteerDemo::camera.position()) * scale, 1);
        a_pos.z *= -1;
        a_up     = rotate * vec4(__(OpenSteer::OpenSteerDemo::camera.up()), 1);
        a_up.z  *= -1;

        a_pos = vec4(vec3(a_lookat) + normalize(vec3(a_pos) - vec3(a_lookat)) * GUI::s_openSteerCameraDist, 1);

        a_pos.z = glm::max(float(GUI::s_openSteerCameraDist * 0.5f), float(a_pos.z)); // When switching camera to next Zombie, keep camera above ground.
    }

    void updateOpenSteer()
    {
        if(gotoNextZombie)
        {
            dmess("gotoNextZombie");

            gotoNextZombie = false;

			lock_guard<mutex> _(a_openSteerMutex);

            OpenSteerDemo::selectNextVehicle();
        }

        OpenSteerDemo::updateSimulation();
    }
}

void OpenSteerGlue::init(Canvas * canvas, Network * network)
{
    dmess("OpenSteerGlue::init");

    ZombiePlugin::setNetwork(network);

#ifndef __EMSCRIPTEN__

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	OpenSteerDemo::initialize();

    openSteerThread = new thread([]()
    {
        for(; !GUI::isShuttingDown() ;) { updateOpenSteer() ;}

        // TODO cleanup threadWin!
    });

#else

    OpenSteerDemo::initialize();

#endif

    dmess("Done OpenSteer::OpenSteerDemo::initialize();");

    GUI::addUpdatable([canvas]()
    {
        lock_guard<mutex> _(a_openSteerMutex);

#ifdef __EMSCRIPTEN__

        updateOpenSteer();

#endif

		updateOpenSteerCamera();

		OpenSteerDemo::redraw();

		if (!a_openSteerGeom) { a_openSteerGeom = unique_ptr<Renderable>(DeferredRenderable::createFromQueued(a_geomTrans)); }
		else
		{
			((DeferredRenderable*)a_openSteerGeom.get())->setFromQueued(a_geomTrans);
		}
		
        if(GUI::getCameraMode() == GUI::CAMERA_FOLLOW_ENTITY)
        {
            // TODO this will be one frame behind!
            canvas->getCamera()->setCenter(a_lookat);
            canvas->getCamera()->setEye   (a_pos);
            canvas->getCamera()->setUp    (a_up);

            canvas->getCamera()->update();
        }

        if(a_openSteerGeom) { a_openSteerGeom->render(canvas, POST_G_BUFFER) ;}
    });
}

const dmat4 & OpenSteerGlue::getGeomTrans()         { return a_geomTrans ;}
const dmat4 & OpenSteerGlue::getGeomInverseTrans()  { return a_geomInverseTrans ;}

PolylineSegmentedPathwaySingleRadius * OpenSteerGlue::getPath(const vector<dvec2> & path)
{
	if (path.size() < 3)
	{
		dmess("Error!!!");
	}

    //const float pathRadius = 2.0;
    const float pathRadius = 1.0;

    vector<Vec3> points;

    points.reserve(path.size());

    unordered_set<dvec3> seen;

    for(const auto & i : path)
    {
        const dvec3 pos(a_geomInverseTrans * dvec4(i, 0, 1));
        
        if(seen.find(pos) != seen.end())
        {
            //dmess("Seen!");

            continue;
        }

        seen.insert(pos);

        // TODO Where is this * 10 coming from?
        points.push_back(Vec3(pos.x * 10.0, 0, pos.y * 10.0));
    }

	if (points.size() < 3)
	{
		dmess("Error!!!");
	}

    return new PolylineSegmentedPathwaySingleRadius(points.size(),
                                                    &points[0],
                                                    pathRadius,
                                                    false);
}