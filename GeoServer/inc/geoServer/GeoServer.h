#ifndef __WEB_ASM_PLAY_GEO_SERVER_H__
#define __WEB_ASM_PLAY_GEO_SERVER_H__

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class GeoServer
{
public:

    GeoServer();
    ~GeoServer();

    void start();

private:
    
    typedef websocketpp::server<websocketpp::config::asio> Server;

    typedef Server::message_ptr message_ptr;

    Server serverEndPoint;

    static void on_message(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg);
};

#endif // __WEB_ASM_PLAY_GEO_SERVER_H__