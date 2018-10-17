#ifdef WORKING

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderableLineString.h>
#include <webAsmPlay/RenderablePolygon.h>
#include <webAsmPlay/RenderiableCollection.h>

using namespace std;
using namespace glm;

RenderiableCollection::RenderiableCollection(const bool takeOwnership) : takeOwnership(takeOwnership)
{

}

RenderiableCollection::RenderiableCollection(   const vector<Renderable *> & renderiables,
                                                const bool takeOwnership) : takeOwnership(takeOwnership)
{
    for(Renderable * r : renderiables) { addRenderiable(r) ;}
}

RenderiableCollection::~RenderiableCollection()
{
    if(!takeOwnership) { return ;}

    for(size_t i = 0; i < polygons2D.size(); ++i) { delete polygons2D[i] ;}

    for(size_t i = 0; lineStrings2D.size(); ++i) { delete lineStrings2D[i] ;}
}

void RenderiableCollection::render(const mat4 & MVP) const
{
    for(size_t i = 0; i < polygons2D.size(); ++i) { polygons2D[i]->render(MVP) ;}

    for(size_t i = 0; lineStrings2D.size(); ++i) { lineStrings2D[i]->render(MVP) ;}
}

Renderable * RenderiableCollection::addRenderiable(Renderable * renderiable)
{
    if(dynamic_cast<RenderableLineString *>(renderiable)) { lineStrings2D.push_back((RenderableLineString *)renderiable) ;}

    else if(dynamic_cast<RenderablePolygon *>(renderiable)) { polygons2D.push_back((RenderablePolygon *)renderiable) ;}

    else
    {
        dmess("Error!");
    }

    return renderiable;
}

#endif