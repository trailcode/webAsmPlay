void GeoClient::ensureClient()
{
#ifndef __EMSCRIPTEN__
    
    if(m_client) { return ;}

    m_client = new Client;

    // We expect there to be a lot of errors, so suppress them
    m_client->clear_access_channels(websocketpp::log::alevel::all);
    m_client->clear_error_channels (websocketpp::log::elevel::all);
   
    // Initialize ASIO
    m_client->init_asio();

    m_client->set_open_handler   (bind(&on_open,    this, placeholders::_1));
    m_client->set_message_handler(bind(&on_message, this, placeholders::_1, placeholders::_2));

    websocketpp::lib::error_code ec;

    std::string uri = "ws://localhost:9002";

    m_con = m_client->get_connection(uri, ec);

    m_client->connect(m_con);

    cout << "Done connect!" << endl;

    m_clientThread = new thread([this]()
    {
		OpenGL::ensureSharedContext();

        m_client->run();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(150)); // TODO Find a better way

#endif
}

void GeoClient::getNumPolygons(const function<void (const size_t)> & callback)
{
    ensureClient();

    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    a_numGeomsRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POLYGONS_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POLYGONS_REQUEST;

    *(uint32_t *)&data[1] = (uint32_t)request->m_ID;

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getNumPolylines(const function<void (const size_t)> & callback)
{
    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    a_numGeomsRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POLYLINES_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POLYLINES_REQUEST;

    *(uint32_t *)&data[1] = (uint32_t)request->m_ID;

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getNumPoints(const function<void (const size_t)> & callback)
{
    ensureClient();

    GeoRequestGetNumGeoms * request = new GeoRequestGetNumGeoms(callback);

    a_numGeomsRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_NUM_POINTS_REQUEST, request->ID);
                
    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_NUM_POINTS_REQUEST;

    *(uint32_t *)&data[1] = (uint32_t)request->m_ID;

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getLayerBounds(const function<void (const AABB2D &)> & callback)
{
    ensureClient();

    GeoRequestLayerBounds * request = new GeoRequestLayerBounds(callback);

    a_layerBoundsRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(5); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n", GeoServerBase::GET_LAYER_BOUNDS_REQUEST, request->ID);

    emscripten_run_script(buf);

#else

    vector<char> data(5);

    data[0] = GeoServerBase::GET_LAYER_BOUNDS_REQUEST;

    *(uint32_t *)&data[1] = (uint32_t)request->m_ID;

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPolygons(const size_t                                      startIndex,
                            const size_t                                      numPolys,
                            function<void (vector<AttributedGeometry> geoms)> callback)
{
    ensureClient();

    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    a_allGeometriesRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POLYGONS_REQUEST,
                        request->ID,
                        startIndex,
                        numPolys);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    char * ptr = appendChar(data, GeoServerBase::GET_POLYGONS_REQUEST);

    appendUint32s(ptr, request->m_ID, startIndex, numPolys);

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPolylines(const size_t                                     startIndex,
                             const size_t                                     numPolylines,
                             function<void(vector<AttributedGeometry> geoms)> callback)
{
    ensureClient();

    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    a_allGeometriesRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POLYLINES_REQUEST,
                        request->ID,
                        startIndex,
                        numPolylines);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    char * ptr = appendChar(data, GeoServerBase::GET_POLYLINES_REQUEST);

    appendUint32s(ptr, request->m_ID, startIndex, numPolylines);

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::getPoints(const size_t                              startIndex,
                          const size_t                              numPoints,
                          function<void (vector<AttributedGeometry> geoms)> callback)
{
    ensureClient();

    GetRequestGetAllGeometries * request = new GetRequestGetAllGeometries(callback);

    a_allGeometriesRequests[request->m_ID] = request;

#ifdef __EMSCRIPTEN__

    char buf[2048];

    sprintf(buf,    "var buffer = new ArrayBuffer(13); \r\n"
                    "var dv = new DataView(buffer); \r\n"
                    "dv.setUint8(0,%i); \r\n"
                    "dv.setUint32(1, Module.swap32(%i)); \r\n"
                    "dv.setUint32(5, Module.swap32(%i)); \r\n"
                    "dv.setUint32(9, Module.swap32(%i)); \r\n"
                    "Module.connection.send(buffer); \r\n",
                        GeoServerBase::GET_POINTS_REQUEST,
                        request->ID,
                        startIndex,
                        numPoints);
                
    emscripten_run_script(buf);

#else

    vector<char> data(1 + sizeof(uint32_t) * 3);

    char * ptr = appendChar(data, GeoServerBase::GET_POINTS_REQUEST);

    appendUint32s(ptr, request->m_ID, startIndex, numPoints);

    m_client->send(m_con, &data[0], data.size(), websocketpp::frame::opcode::binary);

#endif
}

void GeoClient::onMessage(const string & data)
{
    const char * ptr = (const char *)data.data();

    switch(ptr[0])
    {
        case GeoServerBase::GET_NUM_POLYGONS_RESPONCE:
        case GeoServerBase::GET_NUM_POLYLINES_RESPONCE:
        case GeoServerBase::GET_NUM_POINTS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            const uint32_t numGeoms = getUint32(ptr);

            const auto i = a_numGeomsRequests.find(requestID);

            unique_ptr<GeoRequestGetNumGeoms> request(i->second);

            request->m_callback(numGeoms);

            a_numGeomsRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POLYGONS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            const auto i = a_allGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->m_callback(GeometryConverter::getGeosPolygons(ptr));

            a_allGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POLYLINES_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);;

            const auto i = a_allGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->m_callback(GeometryConverter::getGeosLineStrings(ptr));

            a_allGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_POINTS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);;

            const auto i = a_allGeometriesRequests.find(requestID);

            unique_ptr<GetRequestGetAllGeometries> request(i->second);

            request->m_callback(GeometryConverter::getGeosPoints(ptr));

            a_allGeometriesRequests.erase(i);

            break;
        }

        case GeoServerBase::GET_LAYER_BOUNDS_RESPONCE:
        {
            const uint32_t requestID = getUint32(++ptr);

            const AABB2D & bounds = *(AABB2D *)ptr;

            const auto i = a_layerBoundsRequests.find(requestID);

            unique_ptr<GeoRequestLayerBounds> request(i->second);

            request->m_callback(bounds);

            a_layerBoundsRequests.erase(i);

            break;
        }
    }
}

void GeoClient::loadGeoServerGeometry()
{
    dmess("GeoClient::loadGeoServerGeometry");

    ensureClient();

    for(auto renderiable : m_canvas->getRenderiables()) { delete renderiable ;}

    getLayerBounds([this](const AABB2D & bounds)
    {
        const dmat4 s = scale(dmat4(1.0), dvec3(30.0, 30.0, 30.0));

        m_trans = translate(s,
                            dvec3((get<2>(bounds) + get<0>(bounds)) * -0.5,
                                  (get<3>(bounds) + get<1>(bounds)) * -0.5,
                                  0.0));
        
        m_inverseTrans = inverse(m_trans);

        getNumPoints([this](const size_t numPoints)
        {
            dmess("numPoints " << numPoints);

            if(!numPoints) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPoints);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPoints / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPoints / blockSize;

                getPoints(startIndex, std::min(blockSize, numPoints - startIndex - blockSize),
                             [this, isLast, geoms, startIndex, &startTime, numPoints]
                             (vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(1/6) Loading points:", geoms->size(), numPoints, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createPointRenderiables(*geoms.get()) ;}
                });
            }
        });

        getNumPolylines([this](const size_t numPolylines)
        {
            if(!numPolylines) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPolylines);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPolylines / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPolylines / blockSize;

                getPolylines(startIndex, std::min(blockSize, numPolylines - startIndex - blockSize),
                             [this, isLast, geoms, startIndex, &startTime, numPolylines]
                             (vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(1/6) Loading linestrings:", geoms->size(), numPolylines, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createLineStringRenderiables(*geoms.get()) ;}
                });
            }
        });

        getNumPolygons([this](const size_t numPolys)
        {
            if(!numPolys) { return ;}

            auto startTime = system_clock::now();

            const size_t blockSize = std::min((size_t)1024, numPolys);

            shared_ptr<vector<AttributedGeometry> > geoms(new vector<AttributedGeometry>());

            for(size_t i = 0; i < numPolys / blockSize; ++i)
            {
                const size_t startIndex = i * blockSize;

                const bool isLast = i + 1 >= numPolys / blockSize;

                getPolygons(startIndex, std::min(blockSize, numPolys - startIndex - blockSize),
                                [this, geoms, isLast, startIndex, numPolys, &startTime](vector<AttributedGeometry> geomsIn)
                {
                    doProgress("(4/6) Loading polygons:", geoms->size(), numPolys, startTime, 1);

                    geoms->insert(geoms->end(), geomsIn.begin(), geomsIn.end());

                    if(isLast) { createPolygonRenderiables(*geoms.get()) ;}
                });
            }
        });
    });
}

namespace
{
#ifdef __EMSCRIPTEN__

    void downloadSucceeded(emscripten_fetch_t *fetch)
    {
        ((GeoClient *)fetch->userData)->createWorld(fetch->data);

        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        emscripten_fetch_close(fetch); // Free data associated with the fetch.
    }

    void downloadFailed(emscripten_fetch_t *fetch)
    {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        emscripten_fetch_close(fetch); // Also free data on failure.
    }

    void downloadProgress(emscripten_fetch_t *fetch)
    {
        if (fetch->totalBytes) { GUI::progress("Downloading:", float(fetch->dataOffset) / (fetch->totalBytes)) ;}
        else                   { GUI::progress("Done Downloading:", 1.0) ;}
    }

#endif
}

#ifndef __EMSCRIPTEN__

void GeoClient::on_open(GeoClient * client, websocketpp::connection_hdl hdl)
{
    dmess("Connection Open!");
}

void GeoClient::on_message(GeoClient * client, websocketpp::connection_hdl hdl, message_ptr msg)
{
    Client::connection_ptr con = client->m_client->get_con_from_hdl(hdl);

    onMessage(msg->get_payload());
}

#else

EMSCRIPTEN_BINDINGS(GeoClientBindings)
{
    emscripten::function("onMessage", &GeoClient::onMessage);
}

#endif

