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

    GeoServer(const std::string & geomFile);
    ~GeoServer();

    std::string addGeoFile(const std::string & geomFile);

    void start();

    size_t getNumGeoms() const;

    const std::string & getGeom(const size_t index) const;

private:
    
    typedef websocketpp::server<websocketpp::config::asio> Server;

    typedef Server::message_ptr message_ptr;

    Server serverEndPoint;

    static void on_message(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg);

    std::vector<std::string> serializedGeoms;

    double boundsMinX;
    double boundsMinY;
    double boundsMaxX;
    double boundsMaxY;
};

#endif // __WEB_ASM_PLAY_GEO_SERVER_H__