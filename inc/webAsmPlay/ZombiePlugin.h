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

#ifndef __WEB_ASM_PLAY_ZOMBIE_PLUGIN_H__
#define __WEB_ASM_PLAY_ZOMBIE_PLUGIN_H__

#include <OpenSteer/PlugIn.h>

namespace OpenSteer
{
    class ProximityDatabase;
}

class ZombiePlugin : public OpenSteer::PlugIn
{
public:

    const char * name();

    float selectionOrderSortKey();

    virtual ~ZombiePlugin();

    void open();

    void update(const float currentTime, const float elapsedTime);

    void redraw(const float currentTime, const float elapsedTime);

    void serialNumberAnnotationUtility(const OpenSteer::AbstractVehicle & selected,
                                       const OpenSteer::AbstractVehicle & nearMouse);

    void drawPathAndObstacles();

    void close();

    void reset();

    void handleFunctionKeys(int keyNumber);

    void printMiniHelpForFunctionKeys();

    void addPedestrianToCrowd();

    void removePedestrianFromCrowd();

    void nextPD();

    const OpenSteer::AVGroup & allVehicles();

    //Pedestrian::groupType crowd;

    //typedef Pedestrian::groupType::const_iterator iterator;

    OpenSteer::Vec3 gridCenter;

    OpenSteer::ProximityDatabase * pd;

    int population;

    int cyclePD;

private:

};

#endif // __WEB_ASM_PLAY_ZOMBIE_PLUGIN_H__
