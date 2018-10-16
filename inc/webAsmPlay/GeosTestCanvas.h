#ifndef __WEB_ASM_PLAY_GEOS_TEST_CANVAS_H__
#define __WEB_ASM_PLAY_GEOS_TEST_CANVAS_H__

#include <memory>
#include <vector>
#include <webAsmPlay/Canvas.h>

class GeosTestCanvas : public Canvas
{
public:

    GeosTestCanvas();
    ~GeosTestCanvas();

    void setGeomParameters( const float buffer1,
                            const float buffer2,
                            const float buffer3);

private:

    std::vector<std::unique_ptr<Renderiable> > geoms;

    float buffer1;
    float buffer2;
    float buffer3;
};

#endif // 