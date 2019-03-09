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

#include <OpenSteer/OpenSteerDemo.h>
#include <OpenSteer/Annotation.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/GUI/GUI.h>

using namespace OpenSteer;

float GUI::openSteerCameraDist = 0.4f;

bool gotoNextZombie = false; // TODO find a better way!

extern int gPedestrianStartCount;

void GUI::openSteerPanel()
{
    if(!showOpenSteerPanel) { return ;}
    
    ImGui::Begin("OpenSteer Settings", &showOpenSteerPanel);

        static int lines = 1000;

        if(ImGui::SliderInt("Zombies", &lines, 1, 3000))
        {
            gPedestrianStartCount = lines;
        }

        ImGui::SliderFloat("", &openSteerCameraDist, 0.007f, 1.5f, "Camera dist: %.3f");
        
        if(ImGui::Button("Next Zombie"))
        {
            gotoNextZombie = true;

            cameraMode = 2; // TODO create an enum!
        }

        ImGui::SameLine(); ImGui::Checkbox("Annotation", &OpenSteer::enableAnnotation);

    ImGui::End();
}

