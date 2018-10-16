#ifndef __WEB_ASM_PLAY_GEOS_TEST_CANVAS_H__
#define __WEB_ASM_PLAY_GEOS_TEST_CANVAS_H__

#include <webAsmPlay/Canvas.h>

class GeosTestCanvas : public Canvas
{
public:

    GeosTestCanvas();
    ~GeosTestCanvas();

    void setGeomParameters(const float buffer1, const float buffer2);

private:

    Renderiable * geom1;
};

#endif // 