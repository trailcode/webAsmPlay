#ifndef __PLANE_2_H__
#define __PLANE_2_H__

namespace tce
{
	namespace geom
	{
		template <typename VectorType>
		class Plane2
		{
		public:
			
			int classifyPoly(const TriPolygon & poly) const;
			int classifyPoly(const TriPolygon * poly) const;
			
			int classifyPoint(const tce::geom::Vec3f & pos) const;
			
			std::pair<TriPolygon *, TriPolygon *> splitPolygon(const TriPolygon * poly) const;
			std::pair<TriPolygon *, TriPolygon *> splitPolygon(const TriPolygon & poly) const;
			
			void splitPolygon(const TriPolygon * poly, TriPolygon * FrontSplit, TriPolygon * BackSplit) const;
			void splitPolygon(const TriPolygon & poly, TriPolygon * FrontSplit, TriPolygon * BackSplit) const;
			
			bool getIntersect(const VectorType & linestart, const VectorType & lineend, VectorType & intersection, float & percentage) const;
			
			VectorType getIntersect(const VectorType & linestart, const VectorType & lineend, float & percentage, bool & intersects) const;
			VectorType getIntersect(const VectorType & linestart, const VectorType & lineend, float & percentage) const;
			VectorType getIntersect(const VectorType & linestart, const VectorType & lineend) const;

			float getDistance(const VectorType & pos) const;
			
			VectorType getClosestPointOnPlane(const VectorType & pos, float & distanceToPlane) const;
			VectorType getClosestPointOnPlane(const VectorType & pos) const;
			
			VectorType getMajorAxis() const;
			
			VectorType pointOnPlane;
			VectorType normal;
		};
	}
}

#endif // __PLANE_2_H__