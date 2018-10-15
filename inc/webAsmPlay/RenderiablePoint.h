#ifndef __WEB_ASM_PLAY_RENDERIABLE_POINT_H__
#define __WEB_ASM_PLAY_RENDERIABLE_POINT_H__

#include <webAsmPlay/Renderiable.h>

class RenderiablePoint : public Renderiable
{
public:

    ~RenderiablePoint();

    static Renderiable * create(const glm::vec3 & pos);

    void render(const glm::mat4 & MVP) const;

    static void ensureShaders();

private:
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_POINT_H__
