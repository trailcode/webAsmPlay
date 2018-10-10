#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Shader.h>
#include <webAsmPlay/GridPlane.h>

namespace
{
    Shader * shader = NULL;
}

void GridPlane::ensureShader()
{
    if(shader) { return ;}

    dmess("GridPlane::ensureShader");

    // Shader sources
    const GLchar* vertexSource = R"glsl(#version 330 core
        layout (location = 0) in vec4 in_position;
        layout (location = 2) in vec3 in_UV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        smooth out vec3 ex_UV;
        smooth out vec3 ex_originalPosition;
    
        void main()
        {
            gl_Position = projection * view * model * in_position;

            ex_UV = in_UV;
            ex_originalPosition = vec3(in_position.xyz);
        }
    )glsl";

    const GLchar* fragmentSource = R"glsl(#version 330 core
        layout (location = 0) out vec4 color;

        smooth in vec3 ex_UV;
        smooth in vec3 ex_originalPosition;

        uniform vec4 lineColor;

        void main(void)
        {
            if(fract(ex_UV.x / 0.001f) < 0.01f || fract(ex_UV.y / 0.001f) < 0.01f)
                color = lineColor;
            else
                color = vec4(0);
        }
    )glsl";

    shader = Shader::create(vertexSource, fragmentSource);
}