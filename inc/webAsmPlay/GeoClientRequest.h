#ifndef __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__

#include <functional>

class GeoClient;

class GeoClientRequest
{
public:

    GeoClientRequest(const uint8_t type, const std::function<void ()> & callback);

protected:
private:
    
    const std::function<void ()> callback;
};

class GeoRequestGetNumGeoms
{
public:

    GeoRequestGetNumGeoms(const std::function<void (const size_t numGeoms)> & callback);

protected:
private:

    friend class GeoClient;

    const std::function<void (const size_t)> callback;

    const size_t requestID;
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_REQUEST_H__