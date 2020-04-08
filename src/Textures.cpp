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

#include <fstream>
#include <SDL_image.h>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/Textures.h>

using namespace std;
using namespace glm;

#ifndef __EMSCRIPTEN__
using namespace ctpl;
#endif

namespace
{
    Textures * a_instance = nullptr;
} 

#ifndef __EMSCRIPTEN__
thread_pool<boost::lockfree::queue<std::function<void(int id)> *>> Textures::s_queue(1);
#endif

Textures * Textures::getInstance()
{
    if(a_instance) { return a_instance ;}

    return a_instance = new Textures();
}

Textures::Textures()
{

}

Textures::~Textures()
{

}

GLuint Textures::load(const string & filename)
{
    SDL_Surface * img = IMG_Load(filename.c_str());
	
	if(!img)
	{
		dmess("Error! could not load: " << filename);

		return 0;
	}

    const GLuint texture = load(img);

    return texture;
}

GLuint Textures::load(const SDL_Surface* img)
{
	GLuint texture;

    glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture);

    int format = GL_RGB;
	
	//int internalformat = GL_COMPRESSED_RGB;
	//int internalformat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	int internalformat = GL_RGB;

    if(img->format->BytesPerPixel == 4)
	{
		format = GL_RGBA;
		
		//internalformat = GL_COMPRESSED_RGBA;
		//internalformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		internalformat = GL_RGBA;
	}

    /* Generate The Texture */
    glTexImage2D(	GL_TEXTURE_2D,
					0,
					internalformat,
					img->w,
                    img->h,
					0,
					format,
                    GL_UNSIGNED_BYTE, 
					img->pixels);

    /* Linear Filtering */
	/*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//*/

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Needed?

	//glHint(GL_TEXTURE_COMPRESSION_HINT, GL_FASTEST);

	glFlush();

    return texture;
}

GLuint Textures::loadCube(const vector<string> & files)
{
    if(files.size() != 6)
    {
        dmess("Error, cube texture incorrect number of files!")
    }

    dmess("files[0] " << files[0]);

    auto xpos = IMG_Load(files[0].c_str());
    auto xneg = IMG_Load(files[1].c_str());
    auto ypos = IMG_Load(files[2].c_str());
    auto yneg = IMG_Load(files[3].c_str());
    auto zpos = IMG_Load(files[4].c_str());
    auto zneg = IMG_Load(files[5].c_str());

    if(!xpos) { dmess("Error cannot load: " << files[0]); return 0 ;}
    if(!xneg) { dmess("Error cannot load: " << files[1]); return 0 ;}
    if(!ypos) { dmess("Error cannot load: " << files[2]); return 0 ;}
    if(!yneg) { dmess("Error cannot load: " << files[3]); return 0 ;}
    if(!zpos) { dmess("Error cannot load: " << files[4]); return 0 ;}
    if(!zneg) { dmess("Error cannot load: " << files[5]); return 0 ;}

    GLuint texCube;
    
    glGenTextures(1, &texCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, xpos->w, xpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xpos->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, xneg->w, xneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xneg->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, ypos->w, ypos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, ypos->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, yneg->w, yneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, yneg->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, zpos->w, zpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zpos->pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, zneg->w, zneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zneg->pixels);
    
	glFlush();

    /*
    SDL_FreeSurface(xpos); // TODO Causes crashing. MUst not be the correct way to free memory.
    SDL_FreeSurface(xneg);
    SDL_FreeSurface(ypos);
    SDL_FreeSurface(yneg);
    SDL_FreeSurface(zpos);
    SDL_FreeSurface(zneg);
    */

    return texCube;
}

GLuint Textures::create(const vec4 * values, const size_t num)
{
    GLuint texture;

    glGenTextures(1, &texture);

    return set1D(texture, values, num);
};

GLuint Textures::set1D(const GLuint texture, const glm::vec4 * values, const size_t num)
{
    glBindTexture(GL_TEXTURE_2D, texture); // TODO Why the OpenGL error?

    // Generate The Texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)num, 1, 0, GL_RGBA, GL_FLOAT, values);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return texture;
}

void Textures::deleteTextures(const vector<GLuint> & IDs) { glDeleteTextures(IDs.size(), &IDs[0]) ;}