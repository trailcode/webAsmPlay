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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
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
	enum
	{
		SPLIT_X = 0,
		SPLIT_Y,
		SPLIT_LEAF
	};

	struct MyPoint
	{
		template<typename PointType>
		MyPoint(const PointType & p) : m_pos(p.x, p.y)
		{
		}

		template<typename PointType>
		MyPoint& operator=(const PointType& other)
		{
			m_pos = {other.x, other.y};
			
			return *this;
		}

		dvec2 m_pos;

		uint8_t m_splitAxis = SPLIT_LEAF;

		MyPoint	* m_left	= nullptr;
		MyPoint	* m_right	= nullptr;

		//double m_queryDist = nan("");
		double m_queryDist = 0;
	};

	MyPoint a_dummyPoint(dvec2{nan(""), nan("")});

	vector<MyPoint> a_points;

	std::vector<std::unique_ptr<Renderable> > a_geoms;

	MyPoint * a_root = nullptr;

	int a_maxNum = 10;

	vector<const MyPoint *> a_results[4];

	size_t a_numResults[4] = {0, 0, 0, 0};

	double a_largestDist[4] = {0.0, 0.0, 0.0, 0.0};
	
	dvec2 a_queryPoint;

	//double a_scale = 1.0 / 1000.0;
	double a_scale = 1.0;

	double a_queryRadius = 0.1 / a_scale;
	//double a_queryRadius = 195.6846828;

	dmat4 a_trans;

	MyPoint * buildTree(MyPoint * begin, MyPoint * end, const uint32_t splitAxis)
	{
		switch(splitAxis)
		{
			// Could use partial sort here
			case SPLIT_X: sort(begin, end, [](const MyPoint & A, const MyPoint & B) { return A.m_pos.x < B.m_pos.x ;}); break;
			case SPLIT_Y: sort(begin, end, [](const MyPoint & A, const MyPoint & B) { return A.m_pos.y < B.m_pos.y ;}); break;

			default: dmessError("Bad state!");
		}

		auto & middle = begin[(end - begin) / 2];

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

	inline size_t getQuadrant(const dvec2 & center, const dvec2 & P, const size_t expected = 0)
	{
		size_t ret;

		if (P.x > center.x && P.y > center.y) {
			//cout << "lies in First quadrant"; 
			ret = 0;
		}
		else if (P.x < center.x && P.y > center.y) {
			//cout << "lies in Second quadrant"; 
			ret = 1;
		}
		else if (P.x < center.x && P.y < center.y) {
			//cout << "lies in Third quadrant"; 
			ret = 2;
		}
		else if (P.x > center.x && P.y < center.y) {
			//cout << "lies in Fourth quadrant"; 
			ret = 3;
		}
		else if (P.x == center.x && P.y > center.y) {
			//cout << "lies at positive y axis"; 
			ret = 1;
		}  
		else if (P.x == center.x && P.y < center.y) {
			//cout << "lies at negative y axis"; 
			ret = 3;
		}
		else if (P.y == center.y && P.x < center.x) {
			//cout << "lies at negative x axis"; 
			ret = 2;
		}
		else if (P.y == center.y && P.x > center.x) {
			//cout << "lies at positive x axis"; 
			ret = 0;
		}
		else {
			//cout << "lies at origin"; 
			ret = 0;
		}

		//dmess("ret " << ret << " expected " << expected);

		return ret;
	}

	inline void tryBubbleInsert(MyPoint * node)
	{
		const auto dist = distance(node->m_pos, a_queryPoint);

		//DeferredRenderable::addCrossHair(node->m_pos, 0.02, {0.2,0,1,1}, DEFER_FEATURES);

		

		const auto quadrant = getQuadrant(a_queryPoint, node->m_pos);

		//dmess("quadrant " << quadrant);

		switch(quadrant)
		{
			case 0: DeferredRenderable::addCrossHair(node->m_pos, 0.02 / a_scale, {0.2,0,1,1}, DEFER_FEATURES); break;
			case 1: DeferredRenderable::addCrossHair(node->m_pos, 0.02 / a_scale, {1,0,0,1}, DEFER_FEATURES); break;
			case 2: DeferredRenderable::addCrossHair(node->m_pos, 0.02 / a_scale, {0,1,0,1}, DEFER_FEATURES); break;
			case 3: DeferredRenderable::addCrossHair(node->m_pos, 0.02 / a_scale, {0,1,1,1}, DEFER_FEATURES); break;
			default:
				dmess("Error!");
		}

		if(dist > a_queryRadius) { return ;}

		node->m_queryDist = dist;

		const auto heap = &a_results[quadrant][1];
		
		heap[a_numResults[quadrant]++] = node;

		const MyPoint ** A = heap + a_numResults[quadrant] - 2;

		const MyPoint ** B = A + 1;

		while(B != heap)
		{
			if((*A)->m_queryDist < (*B)->m_queryDist) { break ;}
			
			swap(*A, *B); 

			--A;
			--B;
		}

		if(a_numResults[quadrant] > a_maxNum) { a_numResults[quadrant] = a_maxNum ;}

		a_largestDist[quadrant] = (*(heap + a_numResults[quadrant] - 1))->m_queryDist; 
	}

	size_t a_count1 = 0;
	size_t a_count2 = 0;

	double a_furthestDist = 0;

	inline bool gotFullResults()
	{
		for(size_t i = 0; i < 4; ++i)
		{
			if(a_numResults[i] != a_maxNum) { return false ;}
		}

		return true;
	}

	inline double getFurthestDistance()
	{
		//double furthestDist = a_results[0][a_numResults[0]]->m_queryDist;
		//double furthestDist = 0;

		if(gotFullResults()) { a_furthestDist = 0 ;}

		for(size_t i = 0; i < 4; ++i)
		{
			const auto numResults = a_numResults[i];

			/*
			if(numResults != a_maxNum)
			{
				++a_count1;

				return a_queryRadius * 0.5;
			}
			//*/

			const auto p = a_results[i][numResults];

			const auto dist = p->m_queryDist;

			if(dist < a_furthestDist) { continue ;}

			a_furthestDist = dist;
		}
		
		++a_count2;

		//dmess("furthestDist " << furthestDist);

		return a_furthestDist;
	}

	template<size_t AXIS>
	inline void traverseSplitAxis(MyPoint * node)
	{
		if(a_queryPoint[AXIS] < node->m_pos[AXIS])
		{
			query(node->m_left);

			tryBubbleInsert(node);
					
			const auto newRadius = getFurthestDistance();
			//const auto newRadius = a_queryRadius;

			if (a_queryPoint[AXIS] + newRadius >= node->m_pos[AXIS])
			{
				query(node->m_right);
			}
			else
			{
			 
			}
		}
		else if(a_queryPoint[AXIS] > node->m_pos[AXIS])
		{
			query(node->m_right);

			tryBubbleInsert(node);
					
			const auto newRadius = getFurthestDistance();
			//const auto newRadius = a_queryRadius;

			if (a_queryPoint[AXIS] - newRadius <= node->m_pos[AXIS])
			{
				query(node->m_left);
			}
			else
			{
				
			}
		}
		else
		{
			query(node->m_left);
			query(node->m_right);

			tryBubbleInsert(node);
		}
	}
	
	void query(MyPoint * node)
	{
		if(node == nullptr) { return ;} // Should be done above to prevent a recursive call.

		switch(node->m_splitAxis)
		{
			case SPLIT_X: traverseSplitAxis<SPLIT_X>(node); break;

			case SPLIT_Y: traverseSplitAxis<SPLIT_Y>(node); break;

			case SPLIT_LEAF:

				for(auto p = node->m_left; p != node->m_right; ++p) { tryBubbleInsert(p) ;}
				
				break;

			default:

				dmessError("Invalid state!");
		}
	}

	void fillPoints(Canvas * canvas)
	{
		if(a_points.size()) { return ;}

		vector<dvec2> _points;
	
		boostGeom::MultiPolygon pointCircles;

		//*
		for(size_t i = 0; i < 5000; ++i)
		{
			const auto p = dvec2(linearRand(-1.0f, 1.0f), linearRand(-1.0f, 1.0f));

			_points.push_back(p);

			pointCircles.push_back(boostGeom::makeCircle(p, 0.005, 10));
		}
		//*/

		/*
		FILE * fp = fopen("points.bin", "rb");

		uint32_t num;

		fread(&num, sizeof(uint32_t), 1, fp);

		dmess("num " << num);

		_points.resize(num);

		fread(&_points[0], sizeof(dvec2), num, fp);

		for(const auto & p : _points)
		{
			pointCircles.push_back(boostGeom::makeCircle(p, 10.005, 10));
		}

		fclose(fp);
		*/

		a_points.assign(_points.begin(), _points.end());

		a_root = buildTree(&a_points[0], &a_points[0] + a_points.size(), SPLIT_X);

		a_trans = glm::scale(dvec3(a_scale, a_scale, 1.0));

		auto r = RenderablePolygon::create(pointCircles, a_trans);

		r->setRenderOutline(false); 

		canvas->addRenderable(r);

		for(size_t i = 0; i < 4; ++i)
		{
			a_results[i].resize(a_maxNum + 1);

			a_results[i][0] = &a_dummyPoint;
		}

		canvas->addMouseMoveListener([canvas](const dvec3 & _posWC)
		{
			//dmess("Pos " << posWC.x << " " << posWC.y);

			dvec3 posWC = a_trans / dvec4(_posWC, 1);

			const auto pp = a_trans / dvec4(_posWC, 1);

			dmess("pp " << pp.x << " " << pp.y);

			//posWC = {-336.4085818,964.5771661, 0};

			a_maxNum = 2;

			a_geoms.clear();

			//vector<const MyPoint *> results(maxNum);

			for(size_t i = 0; i < 4; ++i)
			{
				a_numResults[i] = 0;

				a_largestDist[i] = a_queryRadius;
			}

			a_count1 = a_count2 = 0;

			//a_dummyPoint.m_queryDist = a_queryRadius;

			a_queryPoint = posWC;

			a_furthestDist = 0;

			//dmess("-----------------------------------------------");

			query(a_root);

			unordered_set<const MyPoint *> seen;

			for(size_t i = 0; i < 4; ++i)
			{
				for(size_t j = 1; j <= a_numResults[i]; ++j)
				{
					if(seen.find(a_results[i][j]) != seen.end())
					{
						dmess("Seen!");
					}

					seen.insert(a_results[i][j]);

					DeferredRenderable::addCircleFilled(a_results[i][j]->m_pos, 0.0052 / a_scale, {1,0,1,1}, DEFER_FEATURES);

					DeferredRenderable::addCircle(a_results[i][j]->m_pos, 0.0052 / a_scale, {1,1,0,1}, DEFER_FEATURES);
				}
			}

			DeferredRenderable::addCircle(posWC, a_queryRadius, {1, 0.5, 0, 1}, DEFER_FEATURES, 128);

			//const auto p = dvec2{0.1,-0.2};
			const auto p = dvec2{0,0};

			//DeferredRenderable::addCrossHair(p, 1.0, {1,1,0,1}, DEFER_FEATURES);
			DeferredRenderable::addCrossHair(posWC, 1.0 / a_scale, {1,1,0,1}, DEFER_FEATURES);

			DeferredRenderable::addCrossHair({-216.5898618, 974.9137852}, 0.4 / a_scale, {1,0,0,1}, DEFER_FEATURES);

			//getQuadrant(p, posWC);

			/*
			dmess("--------------------------");

			getQuadrant(p, {-1,0}, 2); // 2
			getQuadrant(p, {0,1}, 1); // 1
			getQuadrant(p, {0,0}, 0); // 0
			getQuadrant(p, {1,0}, 0); // 0
			getQuadrant(p, {0,-1}, 3); // 3
			*/

			auto r3 = DeferredRenderable::createFromQueued(DEFER_FEATURES, a_trans);
		
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
		float bufferKD_Tree = a_queryRadius;
		ImGui::SliderFloat("buffer1", &bufferKD_Tree, 0.001f, 1.5f / a_scale, "buffer1 = %.3f");
		a_queryRadius = bufferKD_Tree;

		if(ImGui::SliderInt("Max Num", &a_maxNum, 0, 128))
		{
			// Need an extra slot for the dummy result to avoid a if statement per each node visited.
			for(size_t i = 0; i < 4; ++i)
			{
				a_results[i].resize(a_maxNum + 1);

				a_results[i][0] = &a_dummyPoint;
			}
		}

		size_t found = 0;

		for(size_t i = 0; i < 4; ++i) { found += a_numResults[i] ;}

		ImGui::LabelText("Found", "Found: %i %i %i %i Pos: %f %f fur counts: %i %i", a_numResults[0], a_numResults[1], a_numResults[2], a_numResults[3], a_queryPoint.x, a_queryPoint.y, a_count1, a_count2);

	ImGui::End();
}

