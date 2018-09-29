#include <glm/gtc/type_ptr.hpp>
#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/debug.h>
#include "../GLUTesselator/include/GLU/tessellate.h"
#include <webAsmPlay/GeosRenderiable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

namespace
{
    GLuint shaderProgram2 = 0;
    GLint   posAttrib = 0;
    GLint   MVP_Attrib = 0;

    void ensureShader()
    {
        if(shaderProgram2) { return ;}

        // Shader sources
        const GLchar* vertexSource = R"glsl(#version 330 core
            in vec2 position;
            uniform mat4 MVP;
            void main()
            {
                gl_Position = MVP * vec4(position.xy, 0, 1);
            }
        )glsl";

        const GLchar* fragmentSource = R"glsl(#version 330 core
            out vec4 outColor;
            
            void main()
            {
                outColor = vec4(1,1,1,0.5);
            }
        )glsl";

        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        GLint success = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        dmess("success " << success);

        GLchar infoLog[512];

        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            dmess("ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << infoLog);
            return;
        }

        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        dmess("success " << success);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            dmess("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog);
            return;
        }

        // Link the vertex and fragment shader into a shader program
        shaderProgram2 = glCreateProgram();
        dmess("shaderProgram2 " << shaderProgram2);
        glAttachShader(shaderProgram2, vertexShader);
        glAttachShader(shaderProgram2, fragmentShader);
        //glBindFragDataLocation(shaderProgram2, 0, "outColor");
        glLinkProgram(shaderProgram2);
        //glUseProgram(shaderProgram2);

        glUseProgram(shaderProgram2);

        // Specify the layout of the vertex data
        posAttrib = glGetAttribLocation(shaderProgram2, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

        MVP_Attrib = glGetUniformLocation(shaderProgram2, "MVP");

        dmess("MVP_Attrib " << MVP_Attrib);
    }
}

namespace 
{
    tuple<double *, int *, int, int> run_example(   const double vertices_array[],
                        const double **contours_array,
                        int contours_size)
    {
        double *coordinates_out;
        int *tris_out;
        int nverts, ntris, i;

        const double *p = vertices_array;
        /* const double **contours = contours_array; */

        tessellate(&coordinates_out, &nverts,
                &tris_out, &ntris,
                contours_array, contours_array + contours_size);

        /*
        for (i=0; i<2 * nverts; ++i) {
            fprintf(stdout, "%g ", coordinates_out[i]);
        }
        fprintf(stdout, "\n");
        for (i=0; i<3 * ntris; ++i) {
            fprintf(stdout, "%d ", tris_out[i]);
        }
        fprintf(stdout, "\n");
        */
        /*
        free(coordinates_out);
        if (tris_out)
            free(tris_out);
            */
        return make_tuple(coordinates_out, tris_out, nverts, ntris);
    }
}

GeosRenderiable::GeosRenderiable(Polygon * poly) :  vao(0),
                                                    ebo(0),
                                                    vbo(0),
                                                    ntris(0)
{
    ensureShader();

    const Envelope * bounds = poly->getEnvelopeInternal();

    Coordinate center;

    if(!bounds->centre(center))
    {
        dmess("Bad geometry!");

        return;
    }

    center.x; // TODO shift the geometry by this amount.

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinates()->toVector();

    if(coords.size() < 4)
    {
        dmess("Bad gemetry!");

        return;
    }

    //dmess("coords.size() " << coords.size());

    vector<double> verts;

    for(size_t i = 0; i < coords.size() - 1; ++i)
    {
        const Coordinate & C = coords[i];

        verts.push_back(C.x);
        verts.push_back(C.y);
    }

    //const double *c1[] = {&verts[0], &verts[0] + verts.size()};
    //*
    vector<size_t> c2;
    vector<const double *> c1;
    //c1.push_back(&verts[0]);
    //c1.push_back(&verts[0] + verts.size());
    c2.push_back(0);
    c2.push_back(verts.size());
    //*/

    dmess("poly->getNumInteriorRing() " << poly->getNumInteriorRing());

    //*
    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad gemetry!");

            return;
        }

        size_t startIndex = verts.size();

        dmess("startIndex " << startIndex << " coords.size() " << coords.size());

        for(size_t i = 0; i < coords.size() - 1; ++i)
        {
            const Coordinate & C = coords[i];

            verts.push_back(C.x);
            verts.push_back(C.y);
        }

        //c1.push_back(&verts[startIndex]);

        //c1.push_back(&verts[0] + verts.size());
        c2.push_back(verts.size());
    }
    //*/
    for(size_t i = 0; i < c2.size(); ++i)
    {
        c1.push_back(&verts[0] + c2[i]);
    }

    double *coordinates_out;
    int *tris_out;
    int numVerts;

    //tie(coordinates_out, tris_out, numVerts, ntris) = run_example(&verts[0], &c1[0], 2);
    tie(coordinates_out, tris_out, numVerts, ntris) = run_example(&verts[0], &c1[0], c1.size());

    dmess("coordinates_out " << coordinates_out << " tris_out " << tris_out << " numVerts " << numVerts);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //dmess("vao " << vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    //GLuint vbo;
    glGenBuffers(1, &vbo);

    //dmess("vbo " << vbo);

    /*
    GLfloat vertices[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
    };
    */

    GLfloat vertices[] = {
        -0.5f,  0.5f, // Top-left
         0.5f,  0.5f, // Top-right
         0.5f, -0.5f, // Bottom-right
        -0.5f, -0.5f  // Bottom-left
    };

    vector<GLfloat> verts2(numVerts * 2);

    //dmess("numVerts " << numVerts);

    for(size_t i = 0; i < numVerts * 2; ++i)
    {
        verts2[i] = coordinates_out[i];
        //cout << verts2[i] << " ";
    }
    //cout << endl;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts2[0], GL_STATIC_DRAW);

    // Create an element array
    //GLuint ebo;
    glGenBuffers(1, &ebo);
    //dmess("ebo " << ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    vector<GLuint> elements2(ntris * 3);

    //dmess("ntris * 3 " << ntris * 3);

    for(size_t i = 0; i < ntris * 3; ++i)
    {
        elements2[i] = tris_out[i];
        //cout << elements2[i] << " ";
    }
    //cout << endl;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ntris * 3, &elements2[0], GL_STATIC_DRAW);

    free(coordinates_out);
    if (tris_out) free(tris_out);
}

GeosRenderiable::~GeosRenderiable()
{
    //dmess("GeosRenderiable::~GeosRenderiable()");
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void GeosRenderiable::render(const mat4 & MVP) const
{
    glUseProgram(shaderProgram2);

    //dmess("MVP_Attrib " << MVP_Attrib);

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glBindVertexArray(vao);
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(posAttrib);
    //glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //dmess("ntris " << ntris);
    glDrawElements(GL_TRIANGLES, ntris * 3, GL_UNSIGNED_INT, 0);
}

