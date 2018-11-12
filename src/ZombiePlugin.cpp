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

#include <webAsmPlay/ZombiePlugin.h>

using namespace OpenSteer;

const char * ZombiePlugin::name() { return "Zombies" ;}

float ZombiePlugin::selectionOrderSortKey() { return 0.1f ;}

ZombiePlugin::~ZombiePlugin()
{

}

void ZombiePlugin::open()
{

}

void ZombiePlugin::update(const float currentTime, const float elapsedTime)
{

}

void ZombiePlugin::redraw(const float currentTime, const float elapsedTime)
{

}

void ZombiePlugin::serialNumberAnnotationUtility(const AbstractVehicle & selected,
                                    const AbstractVehicle & nearMouse)
{

}

void ZombiePlugin::drawPathAndObstacles()
{

}

void ZombiePlugin::close()
{

}

void ZombiePlugin::reset()
{

}

void ZombiePlugin::handleFunctionKeys(int keyNumber)
{

}

void ZombiePlugin::printMiniHelpForFunctionKeys()
{

}

void ZombiePlugin::addPedestrianToCrowd()
{

}

void ZombiePlugin::removePedestrianFromCrowd()
{

}

void ZombiePlugin::nextPD()
{

}

const AVGroup & ZombiePlugin::allVehicles()
{

}
