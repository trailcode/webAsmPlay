#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Textures.h>

namespace
{
    Textures * instance = NULL;
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