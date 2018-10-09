
#include <fstream>
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Textures.h>

using namespace std;

namespace
{
    Textures * instance = NULL;

    int invertImage(int width, int height, void *image_pixels)
    {
        auto temp_row = std::unique_ptr<char>(new char[width]);
        if (temp_row.get() == nullptr) {
            SDL_SetError("Not enough memory for image inversion");
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
    /* Status indicator */
    int Status = false;
    GLuint texture;

    /* Create storage space for the texture */
    SDL_Surface *TextureImage[1];

    std::ifstream input( filename, std::ios::binary );
    // copies all data into buffer
    std::vector<char> buffer((
                            std::istreambuf_iterator<char>(input)),
                            (std::istreambuf_iterator<char>()));

    dmess(filename <<" size "<< buffer.size());

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( TextureImage[0] = IMG_Load( filename.c_str() ) ) )
    {
        /* Set the status to true */
        Status = true;

        /* Create The Texture */
        glGenTextures( 1, &texture );

        /* Typical Texture Generation Using Data From The Bitmap */
        glBindTexture( GL_TEXTURE_2D, texture );

        dmess("Loaded "<<TextureImage[0]->w<<" "<<TextureImage[0]->h);


        // Enforce RGB/RGBA
        int format;
        SDL_Surface* formattedSurf;
        if (TextureImage[0]->format->BytesPerPixel==3) {

            formattedSurf = SDL_ConvertSurfaceFormat(TextureImage[0],
                                                    SDL_PIXELFORMAT_RGB24,
                                                    0);
            format = GL_RGB;
        }
        else
        {
            formattedSurf = SDL_ConvertSurfaceFormat(TextureImage[0],
                                                    SDL_PIXELFORMAT_ABGR8888,
                                                    0);
            format = GL_RGBA;
        }

        invertImage(formattedSurf->w*formattedSurf->format->BytesPerPixel, formattedSurf->h, (char *) formattedSurf->pixels);

        /* Generate The Texture */
        glTexImage2D( GL_TEXTURE_2D, 0, format, formattedSurf->w,
                    formattedSurf->h, 0, format,
                    GL_UNSIGNED_BYTE, formattedSurf->pixels );

        /* Linear Filtering */
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        SDL_FreeSurface(formattedSurf);
        SDL_FreeSurface( TextureImage[0] );
    } else {
        dmess("Cannot load "<<filename);
    }

    return texture;
}