
#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

int main(const int argc, char ** argv)
{
    //GeoServer s("/Users/trailcode/Downloads/cb_2017_08_tract_500k/cb_2017_08_tract_500k.shp");
    GeoServer s("/Users/trailcode/osmPolygons.shp");

    s.start();

    return 0;
}