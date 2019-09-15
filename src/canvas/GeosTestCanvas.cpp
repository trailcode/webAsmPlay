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

#include <glm/gtc/matrix_transform.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/shaders/ColorShader.h>
#include <webAsmPlay/shaders/ColorVertexShader.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/RenderableLineString.h>
#include <webAsmPlay/canvas/GeosTestCanvas.h>

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

GeosTestCanvas::GeosTestCanvas() :  m_buffer1(-1),
                                    m_buffer2(-1),
                                    m_buffer3(-1)
{
    
}

GeosTestCanvas::~GeosTestCanvas()
{

}

namespace
{
	vector<Geometry::Ptr> doGeosTest1(	const float buffer1,
										const float buffer2,
										const float buffer3)
	{
		vector<Geometry::Ptr> ret;

		auto shape = makeBox(-0.5,-0.5,0.5,0.5);

		auto inside = unionPolygons({	makeBox(-0.1,-0.1,0.1,0.1),
										makeBox(-0.05,-0.6,0.05,0.6),
										makeBox(-0.6,-0.05,0.6,0.05)});

		shape = Geometry::Ptr(shape->buffer(buffer1));

		ret.push_back(Geometry::Ptr(inside->buffer(buffer2)));

		ret.push_back(Geometry::Ptr(shape->difference(inside.get())));

		for(const LineString * ring : getExternalRings(shape))
		{
			Geometry::Ptr buffered(ring->buffer(buffer3));

			Geometry::Ptr buffered2(ring->buffer(buffer3 + 0.15));

			buffered = Geometry::Ptr(buffered->difference(inside.get()));

			for(const auto ring : getExternalRings(buffered)) { ret.push_back(Geometry::Ptr(ring->clone())) ;}
		}

		return ret;
	}
}

void GeosTestCanvas::setGeomParameters( const float buffer1,
                                        const float buffer2,
                                        const float buffer3)
{
	
    if( m_buffer1 == buffer1 &&
        m_buffer2 == buffer2 &&
        m_buffer3 == buffer3) { return ;}

    m_buffer1 = buffer1;
    m_buffer2 = buffer2;
    m_buffer3 = buffer3;

    m_geoms.clear();

	const mat4 trans = scale(mat4(1.0), vec3(0.6, 0.6, 0.6));

	for (const auto& geom : doGeosTest1(buffer1, buffer2, buffer3))
	{
		Renderable * renderable = Renderable::create(geom, trans);

		static ColorVertexShader * shader = nullptr;

		if(!shader)
		{
			shader = new ColorVertexShader();
		}

		renderable->setShader(shader);
		//renderable->setShader(ColorVertexShader::getDefaultInstance());

		addRenderable(renderable);

		m_geoms.push_back(unique_ptr<Renderable>(renderable));
	}
}

void GeosTestCanvas::exportGeoJson() const
{
	geosUtil::writeGeoJsonFile("geomsOut.geojson", doGeosTest1(m_buffer1, m_buffer2, m_buffer3));
}