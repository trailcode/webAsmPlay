
#include <webAsmPlay/Debug.h>
#include <geoServer/GeoServer.h>

using namespace std;

void (*debugLoggerFunc)(const std::string & file, const size_t line, const std::string & message) = NULL;

void dmessCallback(const string & file, const size_t line, const string & message)
{
    cout << file << " " << line << " " << message;
}

int main(const int argc, char ** argv)
{
    debugLoggerFunc = &dmessCallback;

    //GeoServer s("/Users/trailcode/Downloads/cb_2017_08_tract_500k/cb_2017_08_tract_500k.shp");
    //GeoServer s("/Users/trailcode/osmPolygons.shp");
    GeoServer s("/Users/trailcode/osmPolygon2.shp");

    s.start();

    return 0;
}