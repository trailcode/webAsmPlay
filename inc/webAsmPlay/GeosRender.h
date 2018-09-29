#ifndef __WEB_ASM_PLAY_GEOS_RENDER_H__

namespace geos
{
    namespace geom
    {
        class Polygon;
    }
}

class GeosRender
{
public:

    static bool initOpenGL();

    static geos::geom::Polygon * render(geos::geom::Polygon * poly);

private:

    GeosRender() {}
    ~GeosRender() {}
};

#endif // __WEB_ASM_PLAY_GEOS_RENDER_H__
