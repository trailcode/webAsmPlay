#ifndef __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__
#define __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__

#ifdef WORKING

#include <webAsmPlay/Renderable.h>

class RenderableLineString;
class RenderablePolygon;

class RenderiableCollection : public Renderable
{
public:

    RenderiableCollection(const bool takeOwnership = true);

    RenderiableCollection(const std::vector<Renderable *> & renderiables, const bool takeOwnership = true);

    virtual ~RenderiableCollection();

    virtual void render(const glm::mat4 & MVP) const;

    Renderable * addRenderiable(Renderable * renderiable);

private:

    const bool takeOwnership;

    std::vector<RenderableLineString *> lineStrings2D;
    std::vector<RenderablePolygon    *> polygons2D;
};

#endif

#endif // __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__