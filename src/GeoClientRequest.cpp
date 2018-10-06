#include <webAsmPlay/GeoClientRequest.h>

using namespace std;

namespace
{
    size_t lastRequestID = 0;
} 

GeoClientRequest::GeoClientRequest(const uint8_t type, const function<void ()> & callback) : callback(callback)
{

}

GeoRequestGetNumGeoms::GeoRequestGetNumGeoms(const function<void (const size_t)> & callback) : callback(callback), requestID(++lastRequestID)
{

}