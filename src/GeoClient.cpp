#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeoClient.h>

namespace
{
    GeoClient * instance = NULL;
}

GeoClient * GeoClient::getInstance()
{
    if(instance) { return instance ;}

    return instance = new GeoClient();
}

GeoClient::GeoClient()
{

}

GeoClient::~GeoClient()
{

}

