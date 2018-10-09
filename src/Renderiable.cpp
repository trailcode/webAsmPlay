//#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LineString.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/RenderiableLineString2D.h>
#include <webAsmPlay/RenderiablePolygon2D.h>
#include <webAsmPlay/Renderiable.h>

using namespace std;
using namespace glm;
using namespace geos::geom;

GLuint  Renderiable::shaderProgram = 0;
GLint   Renderiable::posAttrib     = 0;
GLint   Renderiable::MVP_Attrib    = 0;
GLint   Renderiable::colorAttrib   = 0;

Shader * Renderiable::defaultShader = NULL;

void Renderiable::ensureShader()
{
    if(defaultShader) { return ;}
    
    //if(shaderProgram) { return ;}

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        in vec2 position;
        out vec4 vertexColor;
        uniform mat4 MVP;
        uniform vec4 vertexColorIn;

        void main()
        {
            gl_Position = MVP * vec4(position.xy, 0, 1);
            vertexColor = vertexColorIn;
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        out vec4 outColor;
        in vec4 vertexColor;

        void main()
        {
            outColor = vertexColor;
        }
    )glsl";

    defaultShader = Shader::create(vertexSource, fragmentSource);

    return;

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

Renderiable * Renderiable::create(const Geometry * geom, const mat4 & trans)
{
    switch(geom->getGeometryTypeId())
    {
        case GEOS_POINT:                dmess("Implement me!"); return NULL;
        case GEOS_LINESTRING:           dmess("Implement me!"); return NULL;
        case GEOS_LINEARRING:           return RenderiableLineString2D  ::create(dynamic_cast<const LineString *>   (geom), trans);
        case GEOS_POLYGON:              return RenderiablePolygon2D     ::create(dynamic_cast<const Polygon *>      (geom), trans);
        case GEOS_MULTIPOINT:           dmess("Implement me!"); return NULL;
        case GEOS_MULTILINESTRING:      dmess("Implement me!"); return NULL;
        case GEOS_MULTIPOLYGON:         return RenderiablePolygon2D     ::create(dynamic_cast<const MultiPolygon *> (geom), trans);
        case GEOS_GEOMETRYCOLLECTION:   dmess("Implement me!"); return NULL;
        default:
            dmess("Error!");
            abort();
    }

    return NULL;
}

Renderiable::Renderiable()
{
}

Renderiable::~Renderiable()
{
}

vec4 Renderiable::setFillColor(const vec4 & fillColor)
{
    return this->fillColor = fillColor;
}

vec4 Renderiable::getFillColor() const
{
    return fillColor;
}

vec4 Renderiable::setOutlineColor(const vec4 & outlineColor)
{
    return this->outlineColor = outlineColor;
}

vec4 Renderiable::getOutlineColor() const
{
    return outlineColor;
}
