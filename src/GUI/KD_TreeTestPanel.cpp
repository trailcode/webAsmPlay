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
  \copyright 2020
*/

#include <glm/gtc/random.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/GUI/ImguiInclude.h>
#include <webAsmPlay/renderables/RenderablePolygon.h>
#include <webAsmPlay/renderables/DeferredRenderable.h>
#include <webAsmPlay/canvas/KD_TreeTestCanvas.h>
#include <webAsmPlay/FrameBuffer.h>

using namespace std;
using namespace glm;

namespace
{
	float bufferKD_Tree = 0.1f;

	int maxNum = 10;

	enum
	{
		SPLIT_X = 0,
		SPLIT_Y,
		SPLIT_LEAF
	};

	struct MyPoint
	{
		template<typename PointType>
		MyPoint(const PointType & p)
		{
			m_x	= p.x;
			m_y	= p.y;
		}

		template<typename PointType>
		MyPoint& operator=(const PointType& other)
		{
			m_x	= other.x;
			m_y	= other.y;

			return *this;
		}

		double m_x = 0.0;
		double m_y = 0.0;

		uint32_t m_splitAxis = SPLIT_LEAF;

		MyPoint	* m_left	= nullptr;
		MyPoint	* m_right	= nullptr;

		double m_queryDist = 0.0;
	};

	vector<MyPoint> points;

	std::vector<std::unique_ptr<Renderable> > a_geoms;

	MyPoint * a_root = nullptr;

	MyPoint * buildTree(MyPoint * begin, MyPoint * end, const uint32_t splitAxis)
	{
		switch(splitAxis)
		{
			case SPLIT_X: sort(begin, end, [](const MyPoint & A, const MyPoint & B) { return A.m_x < B.m_x ;}); break;
			case SPLIT_Y: sort(begin, end, [](const MyPoint & A, const MyPoint & B) { return A.m_y < B.m_y ;}); break;

			default: dmessError("Bad state!");
		}

		auto & middle = begin[(end - begin) / 2];

		// I guess the question here is to use a leaf based tree where all the points are in the leafs, or have points in the nodes.

		const int leftSize	= &middle - begin   - 1;
		const int rightSize = end	  - &middle - 1;  

		const size_t pointsPerLeaf = 1;
			
		middle.m_splitAxis = splitAxis;

		if(leftSize > pointsPerLeaf) { middle.m_left = buildTree(begin, &middle, !splitAxis) ;}
		else
		{
			const auto left = &middle - 1;

			if(left >= begin)
			{
				left->m_left = begin;
				left->m_right = &middle;

				middle.m_left = left;
			}
		}

		if(rightSize > pointsPerLeaf) { middle.m_right = buildTree(&middle + 1, end, !splitAxis) ;}
		else
		{
			const auto right = &middle + 1;

			if(right < end)
			{
				right->m_left = right;
				right->m_right = end;

				middle.m_right = right;
			}
		}

		return &middle;
	}

	template<typename PointType>
	inline double bubbleInsert(const PointType ** heap, const size_t size, size_t & used, const PointType * c)
	{
		heap[used++] = c;

		const PointType ** c0 = heap + used - 2;

		const PointType ** c1 = c0 + 1;

		while(c1 != heap)
		{
			if((*c0)->m_queryDist < (*c1)->m_queryDist) { break ;}
			//if((*c0)->m_queryDist >= (*c1)->m_queryDist) { break ;}

			auto t = *c0;
			*c0 = *c1;
			*c1 = t;

			--c0;
			--c1;
		}

		if(used > size) { used = size ;}

		// Return the lowest rank
		return (*(heap + used - 1))->m_queryDist; 
	}

	void query(	MyPoint * node,
				const double queryX,
				const double queryY,
				const double radius,
				const size_t maxNum,
				vector<const MyPoint *> & results,
				size_t & numResults,
				double & largestDist)
	{
		if(node == nullptr) { return ;} // Should be done above to prevent a recursive call.

		// Need to figure out if split, on the left or the right.

		switch(node->m_splitAxis)
		{
			case SPLIT_X:

				/*
				if (queryX - radius <= node->m_x) { query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}

				if (queryX + radius >= node->m_x) { query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
				*/

				//*
				if(queryX < node->m_x)
				{
					query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------

					if(numResults)
					{
						const auto furthest = results[numResults - 1];

						const auto newRadius = distance(dvec2(furthest->m_x, furthest->m_y), dvec2(queryX, queryY));

						//dmess("newRadius " << newRadius << " radius " << radius);

						if (queryX + newRadius >= node->m_x) { query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
					}

				}
				else if(queryX > node->m_x)
				{
					query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------

					if(numResults)
					{
						const auto furthest = results[numResults - 1];

						const auto newRadius = distance(dvec2(furthest->m_x, furthest->m_y), dvec2(queryX, queryY));

						//dmess("newRadius " << newRadius << " radius " << radius << " numResults " << numResults);

						if (queryX - newRadius <= node->m_x) { query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
					}
				}

				else
				{
					query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist);
					query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------
				}
				//*/

				break;

			case SPLIT_Y:

				/*
				if (queryY - radius <= node->m_y) { query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}

				if (queryY + radius >= node->m_y) { query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
				*/

				//*
				if(queryY < node->m_y)
				{
					query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------

					if(numResults)
					{
						const auto furthest = results[numResults - 1];

						const auto newRadius = distance(dvec2(furthest->m_x, furthest->m_y), dvec2(queryX, queryY));

						//dmess("newRadius " << newRadius << " radius " << radius << " numResults " << numResults);

						if (queryY + newRadius >= node->m_y) { query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
					}
				}

				else if(queryY > node->m_y)
				{
					query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------

					if(numResults)
					{
						const auto furthest = results[numResults - 1];

						const auto newRadius = distance(dvec2(furthest->m_x, furthest->m_y), dvec2(queryX, queryY));

						if (queryY - newRadius <= node->m_y) { query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist) ;}
					}
				}

				else
				{
					query(node->m_left, queryX, queryY, radius, maxNum, results, numResults, largestDist);
					query(node->m_right, queryX, queryY, radius, maxNum, results, numResults, largestDist);

					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------
				}
				//*/

				break;

			case SPLIT_LEAF:

				for(auto p = node->m_left; p != node->m_right; ++p)
				{
					const auto dist = distance(dvec2(p->m_x, p->m_y), dvec2(queryX, queryY));

					if(dist <= largestDist)
					{
						p->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, p);
					}
				}
				
				{
					/// -------------
					const auto dist = distance(dvec2(node->m_x, node->m_y), dvec2(queryX, queryY));

					DeferredRenderable::addCrossHair({node->m_x, node->m_y}, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

					if(dist <= radius)
					{
						node->m_queryDist = dist;

						largestDist = bubbleInsert(&results[0], maxNum, numResults, node);
					}
					/// -------------
				}
				break;

			default:

				dmessError("Invalid state!");
		}
	}

	void fillPoints(Canvas * canvas)
	{
		if(points.size()) { return ;}

		vector<dvec2> _points;
	
		boostGeom::MultiPolygon pointCircles;

		for(size_t i = 0; i < 5000; ++i)
		{
			const auto p = dvec2(linearRand(-1.0f, 1.0f), linearRand(-1.0f, 1.0f));

			_points.push_back(p);

			pointCircles.push_back(boostGeom::makeCircle(p, 0.005, 10));
		}

		points.assign(_points.begin(), _points.end());

		a_root = buildTree(&points[0], &points[0] + points.size(), SPLIT_X);

		auto r = RenderablePolygon::create(pointCircles);

		r->setRenderOutline(false);

		canvas->addRenderable(r);

		//RenderablePoint::create()

		canvas->addMouseMoveListener([canvas](const dvec3 & posWC)
		{
			//dmess("Pos " << posWC.x << " " << posWC.y);

			a_geoms.clear();

			vector<const MyPoint *> results(maxNum);

			size_t found = 0;

			double largestDist = bufferKD_Tree;

			//double largestDist = numeric_limits<double>::max();
			//double largestDist = 10000000.0;

			//dmess("-------------------------");

			query(a_root, posWC.x, posWC.y, bufferKD_Tree, maxNum, results, found, largestDist);

			//dmess("found " << found);

			for(size_t i = 0; i < found; ++i)
			{
				DeferredRenderable::addCrossHair({results[i]->m_x, results[i]->m_y}, 0.03, {1,0,1,1}, DEFER_FEATURES);

				//dmess("results[i] " << results[i]->m_queryDist);
			}

			//DeferredRenderable::addCrossHair(posWC, 0.105, {1,0,1,1}, DEFER_FEATURES);

			DeferredRenderable::addCircle(posWC, bufferKD_Tree, {1, 0.5, 0, 1}, DEFER_FEATURES, 33);

			const mat4 trans = mat4(1.0);

			auto r3 = DeferredRenderable::createFromQueued(DEFER_FEATURES, trans);
		
			canvas->addRenderable(r3);

			a_geoms.push_back(unique_ptr<Renderable>(r3));
		});
	}
}

void GUI::KD_TreeTestPanel()
{
	s_KD_TreeTestCanvas->setEnabled(s_showKD_TreeTestPanel);

	if(!s_showKD_TreeTestPanel) { return ;}

	fillPoints(s_KD_TreeTestCanvas);

	ImGui::Begin("KD Tree Tests", &s_showKD_TreeTestPanel);

		const ImVec2 pos = ImGui::GetCursorScreenPos();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();

		const auto startPos = vMin;

		//dmess("Posa " << startPos.x << "," << startPos.y);

        const ImVec2 sceneWindowSize = ImGui::GetWindowSize();

		s_KD_TreeTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(pos));
		//s_KD_TreeTestCanvas->setFrameBufferSize(__(sceneWindowSize), __(startPos) - __(pos));

        s_KD_TreeTestCanvas->setWantMouseCapture(GImGui->IO.WantCaptureMouse);

        ImGui::GetWindowDrawList()->AddImage(   (void *)(size_t)s_KD_TreeTestCanvas->render(),
                                                pos,
                                                ImVec2(pos.x + sceneWindowSize.x, pos.y + sceneWindowSize.y),
                                                ImVec2(0, 1),
                                                ImVec2(1, 0));

		ImGui::SliderFloat("buffer1", &bufferKD_Tree, 0.001f, 1.5f, "buffer1 = %.3f");

		ImGui::SliderInt("Max Num", &maxNum, 0, 128);

	ImGui::End();
}

