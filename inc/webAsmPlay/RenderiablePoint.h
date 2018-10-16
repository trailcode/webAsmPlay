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

    RenderiablePoint(   const GLuint      vao,
                        const GLuint      eao,
                        const GLuint      vbo,
                        const bool        isMulti,
                        const glm::vec4 & fillColor,
                        const glm::vec4 & outlineColor,
                        const bool        renderOutline,
                        const bool        renderFill);

    const GLuint vao;
    const GLuint eao;
    const GLuint vbo;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_POINT_H__
