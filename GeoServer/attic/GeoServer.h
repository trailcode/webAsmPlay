#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

class GeoServer : public GeoServerBase
{
public:

    typedef websocketpp::server<websocketpp::config::asio> Server;

    typedef Server::message_ptr message_ptr;

    Server m_serverEndPoint;

    static void onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg);
};

