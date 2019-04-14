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
#pragma once

#include <glm/mat4x4.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <webAsmPlay/Types.h>
#include <geoServer/GeoServerBase.h>

class GeoServer : public GeoServerBase
{
public:

    GeoServer();
    ~GeoServer();

    std::string addGeoFile(const std::string & geomFile);

    void start();

    std::string saveGeoFile(const std::string & fileName);

    void createNavigationPaths(const std::vector<AttributedLineString> & lineStrings);

private:
    
    typedef websocketpp::server<websocketpp::config::asio> Server;

    typedef Server::message_ptr message_ptr;

    Server m_serverEndPoint;

    static void onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg);

#ifdef __USE_GDAL__
    std::string addGdalSupportedFile(const std::string & gdalFile);
#endif

    std::string addOsmFile(const std::string & osmFile);

    std::string _addGeoFile(const std::string & geoFile);

    std::string addLasFile(const std::string & lasFile);

    std::vector<std::string> m_serializedPolygons;
    std::vector<std::string> m_serializedLineStrings;
    std::vector<std::string> m_serializedPoints;
    std::vector<std::string> m_serializedRelations;
    std::vector<std::string> m_serializedPaths;

    double m_boundsMinX;
    double m_boundsMinY;
    double m_boundsMaxX;
    double m_boundsMaxY;

    glm::dmat4 m_trans;
};