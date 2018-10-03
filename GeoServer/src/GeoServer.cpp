#include <ctpl.h>
#include "ogrsf_frmts.h"
#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace
{
    ctpl::thread_pool pool(1);
}

GeoServer::GeoServer(const string & geomFile) : geomFile(geomFile)
{
    GDALAllRegister();

    GDALDataset * poDS = (GDALDataset *)GDALOpenEx(geomFile.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

    if(!poDS) { dmess("Error opening: " << geomFile) ;}

    OGRLayer * poLayer = poDS->GetLayer(0);

    if(!poLayer) { dmess("Error!") ;}

    poLayer->ResetReading();

    for(OGRFeature * poFeature; (poFeature = poLayer->GetNextFeature()) != NULL ;)
    {
        OGRGeometry * poGeometry = poFeature->GetGeometryRef();

        //dmess("poGeometry " << poGeometry);

        unsigned char * data = new unsigned char[poGeometry->WkbSize()];

        poGeometry->exportToWkb(
                                //wkbXDR,
                                wkbNDR, // little-endian (least significant byte first)
                                data);

        dmess("poGeometry->getGeometryName() " << poGeometry->getGeometryName() << " " << poGeometry->WkbSize());

        geoms.push_back(WkbGeom(data, poGeometry->WkbSize()));

        OGRFeature::DestroyFeature( poFeature );
    }
    
    GDALClose( poDS );

    dmess("geoms " << geoms.size());
}

GeoServer::~GeoServer()
{
}

// Define a callback to handle incoming messages
void GeoServer::on_message(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    hdl.lock().get();

    dmess("on_message");

    try {

        string message = msg->get_payload();

        const char * data = (char *)msg->get_payload().data();

        Server * s = &server->serverEndPoint;

        switch(data[0])
        {
            case GET_NUM_GEOMETRIES_REQUEST:

                dmess("GET_NUM_GEOMETRIES_REQUEST");

                pool.push([hdl, s, server](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t));

                    data[0] = GET_NUM_GEOMETRIES_RESPONCE;

                    *(uint32_t *)&data[1] = server->getNumGeoms();

                    dmess("*(uint32_t *)&data[1] " << *(uint32_t *)&data[1]);

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

            break;

            case GET_GEOMETRY_REQUEST:
                {
                    const uint32_t geomID = *(uint32_t *)&data[1];

                    dmess("GET_GEOMETRY_REQUEST " << geomID);

                    pool.push([hdl, s, server, geomID](int ID)
                    {
                        const WkbGeom & geom = server->getGeom(geomID);

                        //vector<char> data(sizeof(char) + sizeof(uint32_t) + geom.second);
                        vector<char> data(sizeof(char) + geom.second);

                        data[0] = GET_GEOMETRY_RESPONCE;

                        //*(uint32_t *)&data[1] = geom.second;

                        dmess("geom.second " << geom.second);

                        memcpy(&data[1], geom.first, geom.second);

                        s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                    });

                    break;
                }

            default:

                dmess("Error!");
        };
    }
    catch (const websocketpp::lib::error_code &e)
    {
        dmess("Failed because: " << e << "(" << e.message() << ")");
    }
}

void GeoServer::start()
{
    dmess("GeoServer::start");

    try
    {
        // Set logging settings
        serverEndPoint.set_access_channels(websocketpp::log::alevel::all);
        serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

        Server::connection_type; // Does nothing?

        // Initialize ASIO
        serverEndPoint.init_asio();

        // Register our message handler
        serverEndPoint.set_message_handler(::bind(&on_message, this, ::_1, ::_2));

        // Listen on port 9002
        serverEndPoint.listen(9002);

        // Start the server accept loop
        serverEndPoint.start_accept();

        // Start the ASIO io_service run loop
        serverEndPoint.run();

    }
    catch (const std::exception &e)
    {
        dmess(e.what());
    }
    catch (websocketpp::lib::error_code e)
    {
        dmess(e.message());
    }
    catch (...)
    {
        dmess("other exception");
    }

    dmess("Exit");
}

size_t GeoServer::getNumGeoms() const { return geoms.size() ;}

GeoServer::WkbGeom GeoServer::getGeom(const size_t index) const { return geoms[index] ;}