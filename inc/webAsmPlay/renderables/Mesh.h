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
// Modified from: https://github.com/JoeyDeVries/LearnOpenGL
#pragma once

//#include <glad/glad.h> // holds all OpenGL type declarations

#include <string>
#include <functional>
#include <vector>
#include <glm/vec2.hpp>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/shaders/Material.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture
{
    GLuint m_ID;

    std::string m_type;

    std::string m_path;
};

typedef std::function<void(const Material & material)> SetMaterialFunctor;

class Mesh
{
public:
    /*  Mesh Data  */
    std::vector<Vertex> m_vertices;

    std::vector<unsigned int> m_indices;

    std::vector<Texture> m_textures;

	Material m_material;

    GLuint m_VAO = 0;

	/*  Render data  */
    GLuint m_VBO = 0;
	
	GLuint m_EBO = 0;

    /*  Functions  */
    // constructor
    Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned int> & indices, const std::vector<Texture> & textures, const Material & material);
    
    // render the mesh
    void draw(const SetMaterialFunctor & onMaterial);
    
	void ensureVAO();

private:
    
    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh();
};


