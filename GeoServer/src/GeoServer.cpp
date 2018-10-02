#include "ogrsf_frmts.h"
#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

using namespace std;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

GeoServer::GeoServer()
{
    GDALAllRegister();
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

        /*
        switch(data[0])
        {
            case PointCloudServer::GET_POINT_CLOUD_ID_REQUEST:
            {
                const uint32_t pointCloudID = server->ensurePointCloud(++data);

                pool.push([hdl, s, pointCloudID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t));

                    data[0] = GET_POINT_CLOUD_ID_RESPONCE;

                    *(uint32_t *)&data[1] = pointCloudID;

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;
            }

            case PointCloudServer::GET_NUMBER_OF_SECTORS_REQUEST:
            {
                const uint32_t pointCloudID = *(uint32_t *)&data[1];
               
                vector<char> data(sizeof(char) + sizeof(uint32_t));

                data[0] = GET_NUMBER_OF_SECTORS_RESPONCE;

                *(uint32_t *)&data[1] = server->pointClouds[pointCloudID]->getAllGeneralizedSectors().size();

                s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);

                break;
            }
            case PointCloudServer::GET_GENERALIZED_SECTOR_REQUEST:
            {
                const char *dataPtr = &data[1];

                const uint32_t pointCloudID = *(uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                const uint32_t sectorIndex = *(uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                PointCloud *pc = server->pointClouds[pointCloudID];

                GeneralizedPointCloudSectorBase *sector = pc->getGeneralizedSector(sectorIndex);

                const AABB3Dd bounds = sector->getBoundingBox();

                vector<char> data(sizeof(char) + sizeof(AABB3Dd) + sizeof(size_t));

                data[0] = GET_GENERALIZED_SECTOR_RESPONCE;

                char * dataOutPtr = &data[1];

                memcpy(dataOutPtr, &bounds, sizeof(AABB3Dd));
                dataOutPtr += sizeof(AABB3Dd);

                const size_t numLevels = sector->getNumLevels();

                memcpy(dataOutPtr, &numLevels, sizeof(size_t));
                dataOutPtr += sizeof(size_t);

                s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);

                break;
            }
            case GET_GENERALIZED_SECTOR_LEVEL_REQUEST:
            {
                const char *dataPtr = &data[1];

                const uint32_t pointCloudID = *(uint32_t *)dataPtr;
                dataPtr += sizeof(uint32_t);

                const uint32_t sectorID = *(uint32_t *)dataPtr;
                dataPtr += sizeof(uint32_t);

                const uint32_t level = *(uint32_t *)dataPtr;
                dataPtr += sizeof(uint32_t);

                pool.push([hdl, s, pointCloudID, sectorID, level, server](int ID)
                {
                    PointCloud * pointCloud = server->pointClouds[pointCloudID];

                    GeneralizedPointCloudSectorBase * sector = pointCloud->getGeneralizedSector(sectorID - 1); // TODO use another ID function

                    sector->ensureLevel(level);

                    const uint32_t numVerts = sector->getNumVerts(level);

                    vector<char> dataOut(sizeof(char) +
                                        sizeof(uint32_t) +
                                        sizeof(uint32_t) +
                                        sizeof(uint32_t) +
                                        sizeof(uint32_t) +
                                        sizeof(Vec3f) * numVerts * 2);

                    char * dataOutPtr = &dataOut[0];

                    *dataOutPtr = GET_GENERALIZED_SECTOR_LEVEL_RESPONCE; dataOutPtr += sizeof(char);

                    *(uint32_t *)dataOutPtr = pointCloudID; 
                    dataOutPtr += sizeof(uint32_t);

                    *(uint32_t *)dataOutPtr = sectorID;
                    dataOutPtr += sizeof(uint32_t);

                    *(uint32_t *)dataOutPtr = level;
                    dataOutPtr += sizeof(uint32_t);

                    *(uint32_t *)dataOutPtr = numVerts;
                    dataOutPtr += sizeof(uint32_t);

                    memcpy(dataOutPtr, sector->getVertsPtr(level), sizeof(Vec3f) * numVerts);
                    dataOutPtr += sizeof(Vec3f) * numVerts;

                    memcpy(dataOutPtr, sector->getColorsPtr(level), sizeof(Vec3f) * numVerts);
                    dataOutPtr += sizeof(Vec3f) * numVerts;

                    s->send(hdl, &dataOut[0], dataOut.size(), websocketpp::frame::opcode::BINARY);
                    
                });

                break;
            }
            default:

                dmess("Error!");
        }
        */
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