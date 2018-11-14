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

#include <iomanip>
#include <sstream>
#include <OpenSteer/OpenSteerDemo.h>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include <webAsmPlay/Network.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/OpenSteerGlue.h>
#include <webAsmPlay/ZombiePlugin.h>

using namespace OpenSteer;

namespace
{
    int const gPedestrianStartCount = 1000;
    bool gUseDirectedPathFollowing = true;  // TODO dup!
    bool gWanderSwitch = true; // TODO dup!

    Network * network = NULL;

    ZombiePlugin gZombiePlugin;
}

const char * ZombiePlugin::name() { return "Zombies" ;}

float ZombiePlugin::selectionOrderSortKey() { return 0.1f ;}

ZombiePlugin::~ZombiePlugin()
{

}

void ZombiePlugin::open()
{
    dmess("ZombiePlugin::open");

    // make the database used to accelerate proximity queries
    cyclePD = -1;
    nextPD ();

    // create the specified number of Pedestrians
    population = 0;
    for (int i = 0; i < gPedestrianStartCount; i++) addPedestrianToCrowd ();

    // initialize camera and selectedVehicle
    Zombie& firstPedestrian = **crowd.begin();

    float distance = 10 * 1.0;
    float elevation = 8 * 1.0;

    OpenSteerDemo::init3dCamera (firstPedestrian, distance, elevation);

    OpenSteerDemo::camera.mode = Camera::cmFixedDistanceOffset;
    OpenSteerDemo::camera.fixedTarget.set (15, 0, 30);
    OpenSteerDemo::camera.fixedPosition.set (15, 70, -70);
    //OpenSteerDemo::camera.fixedPosition.set (15, 70, -60);
    //OpenSteerDemo::camera.mouseAdjustOffset(Vec3(15, 70, -60));
    
}

void ZombiePlugin::update(const float currentTime, const float elapsedTime)
{
    Vec3 p = crowd[0]->position();
    //dmess("crowd " << crowd.size() << p.x << " " << p.z);

    // update each Pedestrian
    for (iterator i = crowd.begin(); i != crowd.end(); i++)
    {
        (**i).update (currentTime, elapsedTime);
    }
}

void ZombiePlugin::redraw(const float currentTime, const float elapsedTime)
{
    // selected Pedestrian (user can mouse click to select another)

    AbstractVehicle& selected = *OpenSteerDemo::selectedVehicle;

    // Pedestrian nearest mouse (to be highlighted)
    AbstractVehicle& nearMouse = *OpenSteerDemo::vehicleNearestToMouse ();

    //OpenSteerDemo::camera.mouseAdjustOffset(Vec3(0.001,0.001,0.001));

    // update camera
    OpenSteerDemo::updateCamera (currentTime, elapsedTime, selected);

    // draw "ground plane"
    if (OpenSteerDemo::selectedVehicle) gridCenter = selected.position();
    //OpenSteerDemo::gridUtility (gridCenter);

    // draw and annotate each Pedestrian
    for (iterator i = crowd.begin(); i != crowd.end(); i++) (**i).draw (); 

    // draw the path they follow and obstacles they avoid
    drawPathAndObstacles ();

    // highlight Pedestrian nearest mouse
    //OpenSteerDemo::highlightVehicleUtility (nearMouse); // Crashing in release mode!

    // textual annotation (at the vehicle's screen position)
    //serialNumberAnnotationUtility (selected, nearMouse); // Crashing in release mode!

    // textual annotation for selected Pedestrian
    if (OpenSteerDemo::selectedVehicle && OpenSteer::annotationIsOn())
    {
        const Color color (0.8f, 0.8f, 1.0f);
        const Vec3 textOffset (0, 0.25f, 0);
        const Vec3 textPosition = selected.position() + textOffset;
        const Vec3 camPosition = OpenSteerDemo::camera.position();
        const float camDistance = Vec3::distance (selected.position(),
                                                    camPosition);
        const char* spacer = "      ";
        std::ostringstream annote;
        annote << std::setprecision (2);
        annote << std::setiosflags (std::ios::fixed);
        annote << spacer << "1: speed: " << selected.speed() << std::endl;
        annote << std::setprecision (1);
        annote << spacer << "2: cam dist: " << camDistance << std::endl;
        annote << spacer << "3: no third thing" << std::ends;
        draw2dTextAt3dLocation (annote, textPosition, color, drawGetWindowWidth(), drawGetWindowHeight());
    }

    // display status in the upper left corner of the window
    std::ostringstream status;
    status << "[F1/F2] Crowd size: " << population;
    status << "\n[F3] PD type: ";
    switch (cyclePD)
    {
    case 0: status << "LQ bin lattice"; break;
    case 1: status << "brute force";    break;
    }
    status << "\n[F4] ";
    if (gUseDirectedPathFollowing)
        status << "Directed path following.";
    else
        status << "Stay on the path.";
    status << "\n[F5] Wander: ";
    if (gWanderSwitch) status << "yes"; else status << "no";
    status << std::endl;
    const float h = drawGetWindowHeight ();
    const Vec3 screenLocation (10, h-50, 0);
    //draw2dTextAt2dLocation (status, screenLocation, gGray80, drawGetWindowWidth(), drawGetWindowHeight());
}

void ZombiePlugin::serialNumberAnnotationUtility(const AbstractVehicle & selected,
                                                  const AbstractVehicle & nearMouse)
{
    // display a Pedestrian's serial number as a text label near its
    // screen position when it is near the selected vehicle or mouse.
    if (&selected && &nearMouse && OpenSteer::annotationIsOn())
    {
        for (iterator i = crowd.begin(); i != crowd.end(); i++)
        {
            AbstractVehicle* vehicle = *i;
            const float nearDistance = 6;
            const Vec3& vp = vehicle->position();
            const Vec3& np = nearMouse.position();
            if ((Vec3::distance (vp, selected.position()) < nearDistance)
                ||
                (&nearMouse && (Vec3::distance (vp, np) < nearDistance)))
            {
                std::ostringstream sn;
                sn << "#"
                    << ((Zombie*)vehicle)->serialNumber
                    << std::ends;
                const Color textColor (0.8f, 1, 0.8f);
                const Vec3 textOffset (0, 0.25f, 0);
                const Vec3 textPos = vehicle->position() + textOffset;
                //draw2dTextAt3dLocation (sn, textPos, textColor, drawGetWindowWidth(), drawGetWindowHeight());
            }
        }
    }
}

void ZombiePlugin::drawPathAndObstacles()
{
    typedef PolylineSegmentedPathwaySingleRadius::size_type size_type;

    /*            
    for (iterator i = crowd.begin(); i != crowd.end(); i++)
    {
        const PolylineSegmentedPathwaySingleRadius& path = *(*i)->path;

        for (size_type i = 1; i < path.pointCount(); ++i ) {
            drawLine (path.point( i ), path.point( i-1) , gGreen);
        }
    }
    */
    
    /*
    // draw obstacles
    drawXZCircle (gObstacle1.radius, gObstacle1.center, gWhite, 40);
    drawXZCircle (gObstacle2.radius, gObstacle2.center, gWhite, 40);
// ------------------------------------ xxxcwr11-1-04 fixing steerToAvoid
    {
        float w = gObstacle3.width * 0.5f;
        Vec3 p = gObstacle3.position ();
        Vec3 s = gObstacle3.side ();
        drawLine (p + (s * w), p + (s * -w), gWhite);

        Vec3 v1 = gObstacle3.globalizePosition (Vec3 (w, w, 0));
        Vec3 v2 = gObstacle3.globalizePosition (Vec3 (-w, w, 0));
        Vec3 v3 = gObstacle3.globalizePosition (Vec3 (-w, -w, 0));
        Vec3 v4 = gObstacle3.globalizePosition (Vec3 (w, -w, 0));

        drawLine (v1, v2, gWhite);
        drawLine (v2, v3, gWhite);
        drawLine (v3, v4, gWhite);
        drawLine (v4, v1, gWhite);
    }
    */
}

void ZombiePlugin::close()
{
    // delete all Pedestrians
    while (population > 0) removePedestrianFromCrowd ();
}

void ZombiePlugin::reset()
{
    // reset each Pedestrian
    for (iterator i = crowd.begin(); i != crowd.end(); i++) (**i).reset ();

    // reset camera position
    OpenSteerDemo::position2dCamera (*OpenSteerDemo::selectedVehicle);

    // make camera jump immediately to new position
    OpenSteerDemo::camera.doNotSmoothNextMove ();
}

void ZombiePlugin::handleFunctionKeys(int keyNumber)
{
    switch (keyNumber)
    {
    case 1:  addPedestrianToCrowd ();                               break;
    case 2:  removePedestrianFromCrowd ();                          break;
    case 3:  nextPD ();                                             break;
    case 4: gUseDirectedPathFollowing = !gUseDirectedPathFollowing; break;
    case 5: gWanderSwitch = !gWanderSwitch;                         break;
    }
}

void ZombiePlugin::printMiniHelpForFunctionKeys()
{

}

void ZombiePlugin::addPedestrianToCrowd()
{
    population++;
    Zombie * pedestrian = new Zombie(*pd, getNetwork());
    crowd.push_back (pedestrian);
    if (population == 1) OpenSteerDemo::selectedVehicle = pedestrian;
}

void ZombiePlugin::removePedestrianFromCrowd()
{
    if (population > 0)
    {
        // save pointer to last pedestrian, then remove it from the crowd
        const Zombie * pedestrian = crowd.back();
        crowd.pop_back();
        population--;

        // if it is OpenSteerDemo's selected vehicle, unselect it
        if (pedestrian == OpenSteerDemo::selectedVehicle)
            OpenSteerDemo::selectedVehicle = NULL;

        // delete the Pedestrian
        delete pedestrian;
    }
}

void ZombiePlugin::nextPD()
{
    // save pointer to old PD
    ProximityDatabase* oldPD = pd;

    // allocate new PD
    const int totalPD = 2;
    switch (cyclePD = (cyclePD + 1) % totalPD)
    {
    case 0:
        {
            const Vec3 center;
            const float div = 20.0f;
            const Vec3 divisions (div, 1.0f, div);
            const float diameter = 80.0f; //XXX need better way to get this
            const Vec3 dimensions (diameter, diameter, diameter);
            typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
            pd = new LQPDAV (center, dimensions, divisions);
            break;
        }
    case 1:
        {
            pd = new BruteForceProximityDatabase<AbstractVehicle*> ();
            break;
        }
    }

    // switch each boid to new PD
    for (iterator i=crowd.begin(); i!=crowd.end(); i++) (**i).newPD(*pd);

    // delete old PD (if any)
    delete oldPD;
}

const AVGroup & ZombiePlugin::allVehicles() { return (const AVGroup&) crowd ;}

Network * ZombiePlugin::setNetwork(Network * _network) { return network = _network ;}

Network * ZombiePlugin::getNetwork() { return network ;}