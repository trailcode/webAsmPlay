#ifndef __WEB_ASM_PLAY_GRID_PLANE_H__
#define __WEB_ASM_PLAY_GRID_PLANE_H__

#include <webAsmPlay/Renderiable.h>

class GridPlane : public Renderiable
{
public:

    GridPlane();
    virtual ~GridPlane();

    void render(const glm::mat4 & MVP) const;

    static void ensureShader();

private:

    GLuint vbo;
    GLuint vao;

    glm::mat4 model;
};

#endif // __WEB_ASM_PLAY_GRID_PLANE_H__
