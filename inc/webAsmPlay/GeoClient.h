#ifndef __WEB_ASM_PLAY_GEO_CLIENT_H__
#define __WEB_ASM_PLAY_GEO_CLIENT_H__

class GeoClient
{
public:

    static GeoClient * getInstance();

private:

    GeoClient();
    ~GeoClient();
};

#endif // __WEB_ASM_PLAY_GEO_CLIENT_H__

