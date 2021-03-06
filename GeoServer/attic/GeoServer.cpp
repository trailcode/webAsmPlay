using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void GeoServer::start()
{
    dmess("GeoServer::start");

    dmess("   serializedPolygons: " << m_serializedPolygons.size());
    dmess("serializedLineStrings: " << m_serializedLineStrings.size());
    dmess("     serializedPoints: " << m_serializedPoints.size());
    dmess("  serializedRelations: " << m_serializedRelations.size());

    dmess("boundsMinX " << m_boundsMinX);
    dmess("boundsMinY " << m_boundsMinY);
    dmess("boundsMaxX " << m_boundsMaxX);
    dmess("boundsMaxY " << m_boundsMaxY);

    cout << "  .-----------------------------------------------------------------." << endl;
    cout << " /  .-.                                                         .-.  \\" << endl;
    cout << "|  /   \\   .oOo.oOo.oOo.  G E O  S E R V E R   .oOo.oOo.oOo.   /   \\  |" << endl;
    cout << "| |\\_.  |                                                     |    /| |" << endl;
    cout << "|\\|  | /|      .oOo.oOo.    S T A R T E D    .oOo.oOo.        |\\  | |/|" << endl;
    cout << "| `---' |                                                     | `---' |" << endl;
    cout << "|       |-----------------------------------------------------|       |" << endl;
    cout << "\\       |                                                     |       /" << endl;
    cout << " \\     /                                                       \\     /" << endl;
    cout << "  `---'                                                         `---'" << endl;

    try
    {
        // Set logging settings
        //serverEndPoint.set_access_channels(websocketpp::log::alevel::all);
        //serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // this will turn off console output for frame header and payload
        m_serverEndPoint.clear_access_channels(websocketpp::log::alevel::frame_header | websocketpp::log::alevel::frame_payload); 
        
        // this will turn off everything in console output
        m_serverEndPoint.clear_access_channels(websocketpp::log::alevel::all); 

        // Initialize ASIO
        m_serverEndPoint.init_asio();

        // Register our message handler
        m_serverEndPoint.set_message_handler(::bind(&onMessage, this, ::_1, ::_2));

        // Listen on port 9002
        m_serverEndPoint.listen(9002);

        // Start the server accept loop
        m_serverEndPoint.start_accept();

        // Start the ASIO io_service run loop
        m_serverEndPoint.run();

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

#ifdef __USE_GDAL__
string GeoServer::addGdalSupportedFile(const string & gdalFile)
{
	using geos::geom::Polygon;

    cout << "Loading: " << gdalFile << endl;

    GDALAllRegister();

    GDALDataset * poDS = (GDALDataset *)GDALOpenEx(gdalFile.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);

    if(!poDS) { dmess("Error opening: " << gdalFile) ;}

    OGRLayer * poLayer = poDS->GetLayer(0);

    if(!poLayer) { dmess("Error!") ;}

    poLayer->ResetReading();

    const GEOSContextHandle_t gctx = OGRGeometry::createGEOSContext();

    vector<AttributedPoligonalArea> polygons;
    vector<AttributedLineString>    lineStrings;

    for(const auto & poFeature : *poLayer)
    {
        Attributes * attrs = new Attributes();

        for(const auto & oField: *poFeature)
        {
            if(oField.IsNull()) { continue ;}

            const string key = oField.GetName();

            switch(oField.GetType())
            {
                case OFTInteger   : attrs->ints32 [key] = oField.GetInteger();   break;
                case OFTInteger64 : attrs->ints64 [key] = oField.GetInteger64(); break;
                case OFTReal      : attrs->doubles[key] = oField.GetDouble();    break;
                case OFTString    : attrs->strings[key] = oField.GetString();    break;
                default           : attrs->strings[key] = oField.GetAsString();  break;
            }
        }

        Geometry * geom = (Geometry *)poFeature->GetGeometryRef()->exportToGEOS(gctx);
        
        switch(geom->getGeometryTypeId())
        {
            case GEOS_POLYGON:    polygons   .push_back(AttributedPoligonalArea(attrs, dynamic_cast<Polygon    *>(geom), geom->getArea())); break;
            case GEOS_LINESTRING: lineStrings.push_back(AttributedLineString   (attrs, dynamic_cast<LineString *>(geom))); break;
            default:
                dmess("Implement for " << geom->getGeometryType());
        }
    }
    
	polygons = discoverTopologicalRelations(polygons);

    breakLineStrings(lineStrings);

    for(const AttributedPoligonalArea & g : polygons)    { serializedPolygons.push_back   (GeometryConverter::convert(g)) ;}
    for(const AttributedLineString    & l : lineStrings) { serializedLineStrings.push_back(GeometryConverter::convert(l)) ;}

    OGREnvelope extent;

    if(poLayer->GetExtent(&extent) != CPLE_None)
    {
       dmess("Error getting extent!");

       abort();
    }

    if(boundsMinX > extent.MinX) { boundsMinX = extent.MinX ;}
    if(boundsMinY > extent.MinY) { boundsMinY = extent.MinY ;}
    if(boundsMaxX < extent.MaxX) { boundsMaxX = extent.MaxX ;}
    if(boundsMaxY < extent.MaxY) { boundsMaxY = extent.MaxY ;}

    GDALClose(poDS);

    return gdalFile;
}
#endif

// Define a callback to handle incoming messages
void GeoServer::onMessage(GeoServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
    hdl.lock().get();

    try {

        Server * s = &server->m_serverEndPoint;

        const char * data = (char *)msg->get_payload().data();

        const char * dataPtr = &data[1];

        const uint32_t requestID = getUint32(dataPtr);

        switch(data[0])
        {
            case GET_NUM_POLYGONS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POLYGONS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedPolygons.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_NUM_POLYLINES_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POLYLINES_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedLineStrings.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_NUM_POINTS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) * 2);

                    data[0] = GET_NUM_POINTS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = (uint32_t)server->m_serializedPoints.size();

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_POLYGONS_REQUEST:
            {
                const uint32_t startIndex = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                const uint32_t numGeoms = *(const uint32_t *)dataPtr; dataPtr += sizeof(uint32_t);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {
                    const vector<string> & serializedPolygons = server->m_serializedPolygons;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(size_t i = 0; i < numGeoms; ++i) { bufferSize += uint32_t(serializedPolygons[startIndex + i].length() + sizeof(uint32_t)) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYGONS_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedPolygons[startIndex + i];

                        *(uint32_t *)ptr = (uint32_t)geom.length(); ptr += sizeof(uint32_t);

                        memcpy(ptr, geom.data(), geom.length()); 

                        ptr += geom.length();
                    }

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;
            }
            case GET_POLYLINES_REQUEST:
            {
                const uint32_t startIndex = getUint32(dataPtr);
                const uint32_t numGeoms   = getUint32(dataPtr);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {
                    const vector<string> & serializedLineStrings = server->m_serializedLineStrings;

                    uint32_t bufferSize = sizeof(char) + sizeof(uint32_t) * 2;

                    for(uint i = 0; i < numGeoms; ++i) { bufferSize += uint32_t(serializedLineStrings[startIndex + i].length() + sizeof(uint32_t)) ;}

                    vector<char> data(bufferSize);

                    char * ptr = &data[0];
                    
                    *ptr = GET_POLYLINES_RESPONCE; ptr += sizeof(char);

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *(uint32_t *)ptr = numGeoms; ptr += sizeof(uint32_t);

                    for(size_t i = 0; i < numGeoms; ++i)
                    {
                        const string & geom = serializedLineStrings[startIndex + i];

                        *(uint32_t *)ptr = (uint32_t)geom.length(); ptr += sizeof(uint32_t);

                        memcpy(ptr, geom.data(), geom.length()); 

                        ptr += geom.length();
                    }

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;
            }
            case GET_POINTS_REQUEST:
            {
                const uint32_t startIndex = getUint32(dataPtr);
                const uint32_t numGeoms   = getUint32(dataPtr);

                pool.push([hdl, s, server, requestID, startIndex, numGeoms](int ID)
                {

                });

                break;
            }
            case GET_LAYER_BOUNDS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    vector<char> data(sizeof(char) + sizeof(uint32_t) + sizeof(AABB2D)); // TODO make a AABB2D class

                    data[0] = GET_LAYER_BOUNDS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *((AABB2D *)ptr) = AABB2D(  server->m_boundsMinX,
                                                server->m_boundsMinY,
                                                server->m_boundsMaxX,
                                                server->m_boundsMaxY);

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                });

                break;

            case GET_NAVIGATION_PATHS_REQUEST:

                pool.push([hdl, s, server, requestID](int ID)
                {
                    /*
                    vector<char> data(sizeof(char) + sizeof(uint32_t) + sizeof(AABB2D)); // TODO make a AABB2D class

                    data[0] = GET_LAYER_BOUNDS_RESPONCE;

                    char * ptr = &data[1];

                    *(uint32_t *)ptr = requestID; ptr += sizeof(uint32_t);

                    *((AABB2D *)ptr) = AABB2D(  server->boundsMinX,
                                                server->boundsMinY,
                                                server->boundsMaxX,
                                                server->boundsMaxY);

                    s->send(hdl, &data[0], data.size(), websocketpp::frame::opcode::BINARY);
                    */
                });

                break;

            default:

                dmess("Error!");
        };
    }
    catch (const websocketpp::lib::error_code & e)
    {
        dmess("Failed because: " << e << "(" << e.message() << ")");
    }
}
