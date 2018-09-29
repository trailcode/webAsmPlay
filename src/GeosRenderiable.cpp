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
using namespace tce::geom;

namespace
{
    GLuint  shaderProgram = 0;
    GLint   posAttrib     = 0;
    GLint   MVP_Attrib    = 0;
    GLint   colorAttrib   = 0;

    void ensureShader()
    {
        if(shaderProgram) { return ;}

        // Shader sources
        const GLchar* vertexSource = R"glsl(#version 330 core
            in vec2 position;
            out vec4 vertexColor;
            uniform mat4 MVP;
            uniform vec4 vertexColorIn;

            void main()
            {
                gl_Position = MVP * vec4(position.xy, 0, 1);
                //vertexColorOut = vertexColor;
                //vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
                vertexColor = vertexColorIn;
            }
        )glsl";

        const GLchar* fragmentSource = R"glsl(#version 330 core
            out vec4 outColor;
            in vec4 vertexColor;

            void main()
            {
                //outColor = vec4(1,1,1,0.5);
                outColor = vertexColor;
            }
        )glsl";

        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        GLint success = 0;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

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

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            dmess("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << infoLog);
            return;
        }

        // Link the vertex and fragment shader into a shader program
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram (shaderProgram);
        glUseProgram  (shaderProgram);

        // Specify the layout of the vertex data
        posAttrib = glGetAttribLocation(shaderProgram, "position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

        MVP_Attrib = glGetUniformLocation(shaderProgram, "MVP");

        colorAttrib = glGetUniformLocation(shaderProgram, "vertexColorIn");

        dmess("colorAttrib " << colorAttrib);
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
                                    const int     numTriangles,
                                    const vector<size_t> & counterVertIndices) :    vao         (vao),
                                                                                    ebo         (ebo),
                                                                                    vbo         (vbo),
                                                                                    numTriangles(numTriangles),
                                                                                    counterVertIndices(counterVertIndices)
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
    
    //dmess("verts.size() " << verts.size());

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

    GLuint vao          = 0;
    GLuint ebo          = 0;
    GLuint eboOutline   = 0;
    GLuint vbo          = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    vector<GLfloat> verts2(numVerts * 2);

    for(size_t i = 0; i < numVerts * 2; ++i) { verts2[i] = vertsOut[i] ;}

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 2, &verts2[0], GL_STATIC_DRAW);
    //dmess("numVerts * 2 " << numVerts * 2);

    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * numTriangles * 3, triangleIndices, GL_STATIC_DRAW);
    
    free(vertsOut);
    if(triangleIndices) { free(triangleIndices) ;}

    return new GeosRenderiable( vao,
                                ebo,
                                vbo,
                                numTriangles,
                                counterVertIndices);
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
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(MVP_Attrib, 1, false, glm::value_ptr(MVP));

    glUniform4f(colorAttrib, fillColor.x, fillColor.y, fillColor.z, fillColor.w);

    glBindVertexArray(vao);
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboOutline);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_LINE_STRIP, numTriangles * 3, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_LINE_STRIP, numTriangles * 3, GL_UNSIGNED_INT, 0);

    /*
    dmess("counterVertIndices.size() " << counterVertIndices.size());
    for(int i = 0; i < counterVertIndices.size() - 1; ++i)
    {
        const size_t a = counterVertIndices[i];
        const size_t b = counterVertIndices[i + 1];
        dmess("a " << a << " b - a " << (b - a) / 2);
        glBindVertexArray(vao);
        // Specify the layout of the vertex data
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_LINE_STRIP, a, (b - a) / 2);
        //glDrawArrays(GL_LINE_STRIP, a, 10);
    }
    */

    //glDrawArrays(GL_LINE_STRIP, 0, 10);

    //glDrawElements(GL_LINE_STRIP, 10, GL_UNSIGNED_INT, numTriangles * 3 * sizeof(GLuint));

    glDisable(GL_BLEND);

    
}

vec4 GeosRenderiable::setFillColor(const vec4 & fillColor)
{
    return this->fillColor = fillColor;
}

vec4 GeosRenderiable::getFillColor() const
{
    return fillColor;
}

vec4 GeosRenderiable::setOutlineColor(const vec4 & outlineColor)
{
    return this->outlineColor = outlineColor;
}

vec4 GeosRenderiable::getOutlineColor() const
{
    return outlineColor;
}
