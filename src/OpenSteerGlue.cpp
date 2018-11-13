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

#include <unordered_set>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <OpenSteer/PolylineSegmentedPathwaySingleRadius.h>
#include <OpenSteer/OpenSteerDemo.h>
#include <OpenSteer/Draw.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/ZombiePlugin.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/OpenSteerGlue.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace OpenSteer;
using namespace geosUtil;

void openSteerDisplayFunc();

namespace
{
    const double scaleValue = 1.0 / 60.0;

    const dmat4 geomTrans(scale(dmat4(1.0), dvec3(scaleValue, scaleValue, scaleValue)));

    const dmat4 geomInverseTrans(inverse(geomTrans));
    //const dmat4 geomInverseTrans(scale(dmat4(1.0), dvec3(60, 60, 60)));
}

void OpenSteerGlue::init(Canvas * canvas, Network * network)
{
    dmess("OpenSteerGlue::init");

    //OpenSteer::OpenSteerDemo::initialize();

    ZombiePlugin::setNetwork(network);

    OpenSteer::OpenSteerDemo::initialize();

    GUI::addUpdatable([canvas]()
    {
        openSteerDisplayFunc();

        unique_ptr<Renderable>(DeferredRenderable::createFromQueued(geomTrans))->render(canvas);
    });
}

const dmat4 & OpenSteerGlue::getGeomTrans()         { return geomTrans ;}
const dmat4 & OpenSteerGlue::getGeomInverseTrans()  { return geomInverseTrans ;}

PolylineSegmentedPathwaySingleRadius * OpenSteerGlue::getPath(const unique_ptr<vector<Coordinate> > & path)
{
    dmess("OpenSteerGlue::getPath");

    const float pathRadius = 0.0001;

    vector<Vec3> points;

    points.reserve(path->size());

    unordered_set<dvec3> seen;

    dmess("path->size() " << path->size());

    for(size_t i = 0; i < path->size(); ++i)
    {
        const dvec3 pos(geomInverseTrans * dvec4(__((*path)[i]), 0, 1));
        
        //const dvec3 pos();
        //const dvec3 pos(__((*path)[i]), 0);

        //dmess("pos " << pos);

        if(seen.find(pos) != seen.end())
        {
            dmess("Seen!");

            continue;
        }

        seen.insert(pos);

        // TODO Where is this * 10 coming from?
        points.push_back(Vec3(pos.x * 10.0, 0, pos.y * 10.0));
    }

    return new PolylineSegmentedPathwaySingleRadius(points.size(),
                                                    &points[0],
                                                    pathRadius,
                                                    false);
}