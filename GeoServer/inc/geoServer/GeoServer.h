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

#ifndef __WEB_ASM_PLAY_GEO_SERVER_H__
#define __WEB_ASM_PLAY_GEO_SERVER_H__

#include <string>
#include <vector>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <geoServer/GeoServerBase.h>

class GeoServer : public GeoServerBase
{
public:

    GeoServer();
    ~GeoServer();

    std::string addGeoFile(const std::string & geomFile);

    void start();

    //const std::string & getPolygon(const size_t index) const;

private:
    
    typedef websocketpp::server<websocketpp::config::asio> Server;

    typedef Server::message_ptr message_ptr;

    Server serverEndPoint;

    static void onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg);

    //size_t getNumPolygons() const;

    std::vector<std::string> serializedPolygons;
    std::vector<std::string> serializedLineStrings;

    double boundsMinX;
    double boundsMinY;
    double boundsMaxX;
    double boundsMaxY;
};

#endif // __WEB_ASM_PLAY_GEO_SERVER_H__