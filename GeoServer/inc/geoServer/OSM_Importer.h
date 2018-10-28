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

#ifndef __WEB_ASM_PLAY_OSM_IMPORTER_H__
#define __WEB_ASM_PLAY_OSM_IMPORTER_H__

#include <string>
#include <webAsmPlay/Types.h>

class OSM_Importer
{
public:

    static bool import( const std::string        & fileName,
                        std::vector<AttributedGeometry> & polygons,
                        std::vector<std::string> & serializedLineStrings,
                        std::vector<std::string> & serializedPoints);

private:

    static void startElement(void *userData, const char *name, const char **atts);

    static void endElement(void *userData, const char *name) { }

    static void handleRelation  (const char **atts);
    static void handleMember    (const char **atts);
    static void handleTag       (const char **atts);
    static void handleNode      (const char **atts);
    static void handleWay       (const char **atts);
    static void handleND        (const char **atts);

    OSM_Importer() {}
    ~OSM_Importer() {}
};

#endif // __WEB_ASM_PLAY_OSM_IMPORTER_H__