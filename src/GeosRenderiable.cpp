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

GeosRenderiable * GeosRenderiable::create(const Geometry * geom)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           dmess("Implement me!"); return NULL;
        case GEOS_LINEARRING:           dmess("Implement me!"); return NULL;
        case GEOS_POLYGON:              return prepairPolygon(dynamic_cast<const Polygon *>(geom));
        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         dmess("Implement me!"); return NULL;
        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return NULL;
        default:
            dmess("Error!");
            abort();
    }

    return NULL;
}

GeosRenderiable::GeosRenderiable(   const GLuint  vao,
                                    const GLuint  ebo,
                                    const GLuint  vbo,
                                    const int     numTriangles) :   vao         (vao),
                                                                    ebo         (ebo),
                                                                    vbo         (vbo),
                                                                    numTriangles(numTriangles)
{
}

GeosRenderiable * GeosRenderiable::prepairPolygon(const Polygon * poly)
{
    ensureShader();

    const Envelope * bounds = poly->getEnvelopeInternal();

    Coordinate center;

    if(!bounds->centre(center))
    {
        dmess("Bad geometry!");

        return NULL;
    }

    const LineString * ring = poly->getExteriorRing();

    const vector<Coordinate> & coords = *ring->getCoordinates()->toVector();

    if(coords.size() < 4)
    {
        dmess("Bad gemetry!");

        return NULL;
    }

    vector<double> verts;

    for(size_t i = 0; i < coords.size() - 1; ++i)
    {
        const Coordinate & C = coords[i];

        verts.push_back(C.x);
        verts.push_back(C.y);
    }

    vector<size_t> counterVertIndices;
    
    counterVertIndices.push_back(0);
    counterVertIndices.push_back(verts.size());
    
    for(size_t i = 0; i < poly->getNumInteriorRing(); ++i)
    {
        const vector<Coordinate> & coords = *poly->getInteriorRingN(i)->getCoordinates()->toVector();

        if(coords.size() < 4)
        {
            dmess("Bad gemetry!");

            return NULL;
        }

        for(size_t i = 0; i < coords.size() - 1; ++i)
        {
            const Coordinate & C = coords[i];

            verts.push_back(C.x);
            verts.push_back(C.y);
        }

        counterVertIndices.push_back(verts.size());
    }

    vector<const double *> counterVertPtrs;

    for(size_t i = 0; i < counterVertIndices.size(); ++i) { counterVertPtrs.push_back(&verts[0] + counterVertIndices[i]) ;}

    double  * vertsOut;
    int     * triangleIndices;
    int       numVerts;
    int       numTriangles;

    tessellate( &vertsOut,
                &numVerts,
                &triangleIndices,
                &numTriangles,
                &counterVertPtrs[0],
                &counterVertPtrs[0] + counterVertPtrs.size());

    GLuint  vao;
    GLuint  ebo;
    GLuint  vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    vector<GLfloat> verts2(numVerts * 2);

    for(size_t i = 0; i < numVerts * 2; ++i) { verts2[i] = vertsOut[i] ;}

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts2[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, triangleIndices, GL_STATIC_DRAW);

    free(vertsOut);
    if(triangleIndices) { free(triangleIndices) ;}

    return new GeosRenderiable( vao,
                                ebo,
                                vbo,
                                numTriangles);
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

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glBindVertexArray(vao);
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, 0);
}

