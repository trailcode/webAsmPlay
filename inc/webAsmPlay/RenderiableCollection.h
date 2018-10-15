#ifndef __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__
#define __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__

#include <webAsmPlay/Renderiable.h>

class RenderiableLineString2D;
class RenderiablePolygon2D;

class RenderiableCollection : public Renderiable
{
public:

    RenderiableCollection(const bool takeOwnership = true);

    RenderiableCollection(const std::vector<Renderiable *> & renderiables, const bool takeOwnership = true);

    virtual ~RenderiableCollection();

    virtual void render(const glm::mat4 & MVP) const;

    Renderiable * addRenderiable(Renderiable * renderiable);

private:

    const bool takeOwnership;

    std::vector<RenderiableLineString2D *> lineStrings2D;
    std::vector<RenderiablePolygon2D    *> polygons2D;
};

#endif // __WEB_ASM_PLAY_RENDERIABLE_COLLECTION_H__