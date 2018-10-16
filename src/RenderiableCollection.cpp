#ifdef WORKING

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/RenderiableLineString2D.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/RenderiableCollection.h>

using namespace std;
using namespace glm;

RenderiableCollection::RenderiableCollection(const bool takeOwnership) : takeOwnership(takeOwnership)
{

}

RenderiableCollection::RenderiableCollection(   const vector<Renderiable *> & renderiables,
                                                const bool takeOwnership) : takeOwnership(takeOwnership)
{
    for(Renderiable * r : renderiables) { addRenderiable(r) ;}
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

Renderiable * RenderiableCollection::addRenderiable(Renderiable * renderiable)
{
    if(dynamic_cast<RenderiableLineString2D *>(renderiable)) { lineStrings2D.push_back((RenderiableLineString2D *)renderiable) ;}

    else if(dynamic_cast<RenderiablePolygon2D *>(renderiable)) { polygons2D.push_back((RenderiablePolygon2D *)renderiable) ;}

    else
    {
        dmess("Error!");
    }

    return renderiable;
}

#endif