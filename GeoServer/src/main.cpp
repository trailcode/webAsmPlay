
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

  \\author Matthew Tang
  \\email trailcode@gmail.com
  \\copyright 2018
*/

#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

using namespace std;

void (*debugLoggerFunc)(const std::string & file, const size_t line, const std::string & message) = NULL;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
}

void showHelp()
{
const char* helpMessage = R"help(
                          __                    __
          __       __     \_\  __          __   \_\  __   __       __
          \_\     /_/        \/_/         /_/      \/_/   \_\     /_/
        .-.  \.-./  .-.   .-./  .-.   .-./  .-.   .-\   .-.  \.-./  .-.
       //-\\_//-\\_//-\\_//-\\_//-\\_//-\\_// \\_//-\\_//-\\_//-\\_//-\\
     __(   '-'   '-'\  '-'   '-'  /'-'   '-'\__'-'   '-'__/'-'   '-'\__
    /_/))            \__       __/\          \_\       /_/           \_\
 ___\_//              \_\     /_/  \__
/_/  ((                             \_\
      )) __
__   // /_/
\_\_((_/___     ╭━━━╮╱╱╱╱╱╱╱╱╭━━━╮
     ))  \_\    ┃╭━╮┃╱╱╱╱╱╱╱╱┃╭━╮┃
     \\         ┃┃╱╰╯╭━━╮╭━━╮┃╰━━╮╭━━╮╭━╮╭╮╭╮╭━━╮╭━╮
      )) _      ┃┃╭━╮┃┃━┫┃╭╮┃╰━━╮┃┃┃━┫┃╭╯┃╰╯┃┃┃━┫┃╭╯
__   // /_/     ┃╰┻━┃┃┃━┫┃╰╯┃┃╰━╯┃┃┃━┫┃┃╱╰╮╭╯┃┃━┫┃┃
\_\_((          ╰━━━╯╰━━╯╰━━╯╰━━━╯╰━━╯╰╯╱╱╰╯╱╰━━╯╰╯
     \\         
      )) __     Usage:
__   // /_/     
\_\_((_/___             geoServer <path to vector file>
     ))  \_\        
     \\             Supported file types:
      )) _              
__   // /_/             .shp
\_\_((_/                .osm (OpenStreetMap xml file format)
     \\                 .geo (GeoServer file format)
      )) __
__   // /_/         Create .geo: 
\_\_((_/___     
     ))  \_\            geoServer --convert <vector file> <.geo file>
     \\         
      )) _          Help (This message):    
__   // /_/         
\_\_((_/___             geoServer --help
     ))  \_\                __                    __
     \\     __       __     \_\  __          __   \_\  __   __       __
  __  ))    \_\     /_/        \/_/         /_/      \/_/   \_\     /_/
  \_\_((   .-.  \.-./  .-.   .-./  .-.   .-./  .-.   .-\   .-.  \.-./  .-.
       \\_//-\\_//-\\_//-\\_//-\\_//-\\_//-\\_// \\_//-\\_//-\\_//-\\_//-\\
        'dc\__'-'   '-'\  '-'   '-'  /'-'   '-'\__'-'   '-'__/'-'   '-'\__
            \_\         \__       __/\          \_\       /_/           \_\
                         \_\     /_/  \__
                                       \_\
)help";

    cout << helpMessage << endl;

    exit(0);
}

int main(const int argc, char ** argv)
{
    if(argc == 1) { showHelp() ;}

    if(argc > 1 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) { showHelp() ;}

    debugLoggerFunc = &dmessCallback;

    GeoServer s;
    
    s.addGeoFile(argv[1]);

    s.start();

    return 0;
}