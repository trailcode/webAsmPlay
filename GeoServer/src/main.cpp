
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

#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

using namespace std;

void (*debugLoggerFunc)(const std::string & file, const size_t line, const std::string & message) = NULL;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
}

int main(const int argc, char ** argv)
{
    debugLoggerFunc = &dmessCallback;

    //GeoServer s("/Users/trailcode/Downloads/cb_2017_08_tract_500k/cb_2017_08_tract_500k.shp");
    //GeoServer s("/Users/trailcode/osmPolygons.shp");
    GeoServer s;
    
    //s.addGeoFile("/Users/trailcode/osmPolygon2.shp");
    //s.addGeoFile("/Users/trailcode/osmPolygons.shp");
    //s.addGeoFile("/Users/trailcode/osmPolylines2.shp");
    //s.addGeoFile("/Users/trailcode/osmPolys.geojson");
    //s.addGeoFile("/Users/trailcode/osm1.osm");

    s.start();

    return 0;
}