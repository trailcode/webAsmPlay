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
  \copyright 2019
*/

#include <atomic>
#include <unordered_set>
#include <filesystem>
#include <tbb/concurrent_unordered_map.h>
#include <ctpl/ctpl.h>
#include <SDL_image.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Textures.h>
#include <webAsmPlay/CurlUtil.h>
#include <webAsmPlay/bing/Bubble.h>
#include <webAsmPlay/bing/BubbleTile.h>

using namespace std;
using namespace std::filesystem;
using namespace tbb;
using namespace ctpl;
using namespace curlUtil;

namespace
{
	//concurrent_unordered_map<string, size_t> a_bubbleTiles;

	thread_pool a_loaderQueue(16);

	std::atomic<int> a_numLoading = {0};

	std::atomic<int> a_numDownloading = {0};
}

BubbleTile::BubbleTile(const string & ID) : Texture(ID) {}

BubbleTile::~BubbleTile() { }

BubbleTile * BubbleTile::requestBubbleTile(const string & bubbleQuadKey, const size_t face, const string & tileID)
{
	return nullptr;

	/*
	const auto faceQuadKey = "s" + bubbleQuadKey + Bubble::s_faceKeys[face] + tileID;

	const auto i = s_textures.find(faceQuadKey);

	if(i != s_textures.end()) { return (BubbleTile *)i->second ;}

	auto bubbleTile = s_textures[faceQuadKey] = new BubbleTile(faceQuadKey);

	const string tileCachePath = "./bubbles/face_" + faceQuadKey;

	++a_numLoading;

	a_loaderQueue.push([bubbleTile, tileCachePath, faceQuadKey](int id)
	{
		if(fileExists(tileCachePath))
		{
			if(!file_size(tileCachePath.c_str()))
			{
				--a_numLoading;

				return;
			}

			auto img = IMG_Load(tileCachePath.c_str());

			if (!img) { goto doDownload ;}

			Textures::s_queue.push([img, faceQuadKey](int ID)
			{
				const auto ret = Textures::load(img);

				SDL_FreeSurface(img);

				a_bubbleTiles[faceQuadKey] = ret;

				--a_numLoading;
			});

			return;
		}

		doDownload:

		//const string streetsideImagesApi = "https://t.ssl.ak.tiles.virtualearth.net/tiles/hs";
		// TODO With https curl gives 60 error code. Try to fix.
		const string streetsideImagesApi = "http://t.ssl.ak.tiles.virtualearth.net/tiles/hs";

		const string imgUrlSuffix = ".jpg?g=6338&n=z";

		const auto url = streetsideImagesApi + faceQuadKey + imgUrlSuffix;

		++a_numDownloading;

		download(url, [faceQuadKey, tileCachePath](BufferStruct * buf)
		{
			auto tileBuffer = shared_ptr<BufferStruct>(buf);

			if (!tileBuffer->m_buffer || tileBuffer->m_size == 11)
			{
				dmess("No data!");

				--a_numLoading;

				--a_numDownloading;

				return;
			}

			auto img = IMG_LoadJPG_RW(SDL_RWFromConstMem(tileBuffer->m_buffer, tileBuffer->m_size));

			if(!img)
			{
				dmess("Bad data!");

				--a_numLoading;

				--a_numDownloading;

				return;
			}

			const auto bytesPerPixel = img->format->BytesPerPixel;

			if(bytesPerPixel < 3)
			{
				SDL_FreeSurface(img);

				// Must be the no data png image, mark as no data.
				//return markTileNoData(tile);

				dmess("No data!");

				--a_numLoading;

				--a_numDownloading;

				return;
			}

			////////////////////////////
			FILE * fp = fopen(tileCachePath.c_str(), "wb");

			if(fp)
			{
				fwrite(tileBuffer->m_buffer, sizeof(char), tileBuffer->m_size, fp);

				fclose(fp);
			}
			else { dmess("Warn could not write file: " << tileCachePath) ;}
			///////////////////////////

			--a_numDownloading;

			Textures::s_queue.push([img, faceQuadKey](int ID)
			{
				const auto ret = Textures::load(img);

				a_bubbleTiles[faceQuadKey] = ret;

				--a_numLoading;

				SDL_FreeSurface(img);
			});
		});
	});

	return 0;
	*/
}

size_t BubbleTile::getNumLoading() { return a_numLoading ;}

size_t BubbleTile::getNumDownloading() { return a_numDownloading ;}

size_t BubbleTile::getNumTiles() { 
return 0;
//return a_bubbleTiles.size();
}

void BubbleTile::freeAllTiles()
{
	/*
	vector<GLuint> IDs;

	for(const auto & [quadKey, ID] : a_bubbleTiles) { IDs.push_back(ID) ;}

	a_bubbleTiles.clear();

	Textures::deleteTextures(IDs);
	*/
}

string BubbleTile::getDownloadURL() const
{
	return "";
}