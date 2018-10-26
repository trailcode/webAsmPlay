/**
╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 // This software is provided 'as-is', without any express or implied
 // warranty.  In no event will the authors be held liable for any damages
 // arising from the use of this software.
 // Permission is granted to anyone to use this software for any purpose,
 // including commercial applications, and to alter it and redistribute it
 // freely, subject to the following restrictions:
 // 1. The origin of this software must not be misrepresented; you must not
 //    claim that you wrote the original software. If you use this software
 //    in a product, an acknowledgment in the product documentation would be
 //    appreciated but is not required.
 // 2. Altered source versions must be plainly marked as such, and must not be
 //    misrepresented as being the original software.
 // 3. This notice may not be removed or altered from any source distribution.

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2018
*/

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