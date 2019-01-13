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

namespace
{
    Textures * instance = NULL;

    int invertImage(int width, int height, void *image_pixels)
    {
        auto temp_row = std::unique_ptr<char>(new char[width]);
        if (temp_row.get() == nullptr) {
            //SDL_SetError("Not enough memory for image inversion");
            return -1;
        }
        //if height is odd, don't need to swap middle row
        int height_div_2 = height / 2;
        for (int index = 0; index < height_div_2; index++)
        {
            //uses string.h
            memcpy((Uint8 *)temp_row.get(),
                (Uint8 *)(image_pixels)+
                width * index,
                width);

            memcpy(
                    (Uint8 *)(image_pixels)+
                    width * index,
                    (Uint8 *)(image_pixels)+
                    width * (height - index - 1),
                    width);
            memcpy(
                    (Uint8 *)(image_pixels)+
                    width * (height - index - 1),
                    temp_row.get(),
                    width);
        }
        return 0;
    }

    void invertImage(SDL_Surface * formattedSurf)
    {
        invertImage(formattedSurf->w*formattedSurf->format->BytesPerPixel, formattedSurf->h, (char *) formattedSurf->pixels);
    }
} 

Textures * Textures::getInstance()
{
    if(instance) { return instance ;}

    return instance = new Textures();
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

    GLuint texture;

    GL_CHECK(glGenTextures(1, &texture));

    /* Typical Texture Generation Using Data From The Bitmap */
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));

    int mode = GL_RGB;

    if(img->format->BytesPerPixel == 4)
    {
        mode = GL_RGBA;
    }

    /* Generate The Texture */
    GL_CHECK(glTexImage2D(  GL_TEXTURE_2D, 0, mode, img->w,
                            img->h, 0, mode,
                            GL_UNSIGNED_BYTE, img->pixels));

    /* Linear Filtering */
    //GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    //GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));



    //SDL_FreeSurface(img);

    return texture;
}

GLuint Textures::loadCube(const vector<string> & files)
{
    dmess("loadCube " << files.size());
    
    if(files.size() != 6)
    {
        dmess("Error, cube texture incorrect number of files!")
    }

    SDL_Surface * xpos = IMG_Load(files[0].c_str());
    SDL_Surface * xneg = IMG_Load(files[1].c_str());
    SDL_Surface * ypos = IMG_Load(files[2].c_str());
    SDL_Surface * yneg = IMG_Load(files[3].c_str());
    SDL_Surface * zpos = IMG_Load(files[4].c_str());
    SDL_Surface * zneg = IMG_Load(files[5].c_str());

    if(!xpos) { dmess("Error cannot load: " << files[0]); return 0 ;}
    if(!xneg) { dmess("Error cannot load: " << files[1]); return 0 ;}
    if(!ypos) { dmess("Error cannot load: " << files[2]); return 0 ;}
    if(!yneg) { dmess("Error cannot load: " << files[3]); return 0 ;}
    if(!zpos) { dmess("Error cannot load: " << files[4]); return 0 ;}
    if(!zneg) { dmess("Error cannot load: " << files[5]); return 0 ;}

    /*
    invertImage(xpos);
    invertImage(xneg);
    invertImage(ypos);
    invertImage(yneg);
    invertImage(zpos);
    invertImage(zneg);
    */

    GLuint texCube;
    
    GL_CHECK(glGenTextures(1, &texCube));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texCube));

    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, xpos->w, xpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xpos->pixels));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, xneg->w, xneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, xneg->pixels));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, ypos->w, ypos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, ypos->pixels));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, yneg->w, yneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, yneg->pixels));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, zpos->w, zpos->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zpos->pixels));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, zneg->w, zneg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, zneg->pixels));
    
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

    GL_CHECK(glGenTextures(1, &texture));

    return set1D(texture, values, num);
};

GLuint Textures::set1D(const GLuint texture, const glm::vec4 * values, const size_t num)
{
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture)); // TODO Why the OpenGL error?

    // Generate The Texture
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, num, 1, 0, GL_RGBA, GL_FLOAT, values));
    
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

    return texture;
}

GLuint Textures::createFromJpeg(const char * data, const size_t size)
{
    SDL_RWops * mem = SDL_RWFromConstMem(data, size);
    //SDL_RWops * mem = SDL_RWFromMem((void *)data, size);

    SDL_Surface * img = IMG_LoadJPG_RW(mem);

    //dmess("w " << image->w << " h " << image->h << " pitch " << image->pitch);

    SDL_RWclose(mem);

    if(!img)
    {
        dmess("Error!");

        return 0;
    }

    GLuint texture;

    GL_CHECK(glGenTextures(1, &texture));

    /* Typical Texture Generation Using Data From The Bitmap */
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));

    /* Generate The Texture */
    GL_CHECK(glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGB, img->w,
                            img->h, 0, GL_RGB,
                            GL_UNSIGNED_BYTE, img->pixels));

    /* Linear Filtering */
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    SDL_FreeSurface(img);

    return texture;
}

