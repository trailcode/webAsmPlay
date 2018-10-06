#ifndef __WEB_ASM_PLAY_GEO_CLIENT_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_H__

#include <string>
#include <functional>
#include <unordered_map>

class GeoRequestGetNumGeoms;

class GeoClient
{
public:

    static GeoClient * getInstance();

    static void onMessage(const std::string & data);

    void getNumGeoms(const std::function<void (const size_t)> & callback);

private:

    GeoClient();
    ~GeoClient();

    std::unordered_map<size_t, GeoRequestGetNumGeoms *> numGeomsRequests;
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_H__

