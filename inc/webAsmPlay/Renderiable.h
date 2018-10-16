#ifndef __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__
#define __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__

#ifdef __EMSCRIPTEN__
    // GLEW
    #define GLEW_STATIC
    #include <GL/glew.h>
#else
    #include <GL/gl3w.h>
#endif

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <geos/geom/Geometry.h>

class Shader;

class Renderiable
{
public:

    typedef std::function<void (Renderiable *)> OnDelete;

    virtual ~Renderiable();

    virtual void render(const glm::mat4 & MVP) const = 0;

    static Renderiable * create(const geos::geom::Geometry::Ptr & geom, const glm::mat4 & trans = glm::mat4(1.0));

    static Renderiable * create(const geos::geom::Geometry * geom, const glm::mat4 & trans = glm::mat4(1.0));

    static void ensureShader();

    static Shader * getDefaultShader();

    glm::vec4 setFillColor(const glm::vec4 & fillColor);
    glm::vec4 getFillColor() const;

    glm::vec4 setOutlineColor(const glm::vec4 & outlineColor);
    glm::vec4 getOutlineColor() const;

    bool setRenderOutline(const bool renderOutline);
    bool getRenderOutline() const;

    bool setRenderFill(const bool renderFill);
    bool getRenderFill() const;

    void addOnDeleteCallback(const OnDelete & callback);

protected:

    Renderiable(const bool        isMulti,
                const glm::vec4 & fillColor,
                const glm::vec4 & outlineColor,
                const bool        renderOutline,
                const bool        renderFill);

    std::vector<OnDelete> onDeleteCallbacks;

    bool        isMulti;
    glm::vec4   fillColor;
    glm::vec4   outlineColor;
    bool        renderOutline;
    bool        renderFill;
};

#endif // __WEB_ASM_PLAY__GEOS_RENDERIABLE_H__