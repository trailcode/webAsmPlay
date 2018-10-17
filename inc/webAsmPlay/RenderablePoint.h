#ifndef __WEB_ASM_PLAY_RENDERIABLE_POINT_H__
#define __WEB_ASM_PLAY_RENDERIABLE_POINT_H__

#include <webAsmPlay/Renderable.h>

class RenderablePoint : public Renderable
{
public:

    ~RenderablePoint();

    static Renderable * create(const glm::vec3 & pos);

    void render(const glm::mat4 & MVP) const;

    static void ensureShaders();

private:

    RenderablePoint(const GLuint      vao,
                    const GLuint      ebo,
                    const GLuint      vbo,
                    const bool        isMulti,
                    const glm::vec4 & fillColor,
                    const glm::vec4 & outlineColor,
                    const bool        renderOutline,
                    const bool        renderFill);

    const GLuint vao;
    const GLuint ebo;
    const GLuint vbo;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_POINT_H__
