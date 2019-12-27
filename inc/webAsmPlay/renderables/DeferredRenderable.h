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
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <webAsmPlay/OpenGL_Util.h>
#include <webAsmPlay/renderables/Renderable.h>

enum
{
	DEFER_GUI = 0,
	DEFER_OPEN_STEER,
	DEFER_FEATURES,
};

class DeferredRenderable : public Renderable
{
public:

    virtual ~DeferredRenderable();

    static DeferredRenderable * createFromQueued(const size_t slot, const glm::dmat4 & trans = glm::dmat4(1.0)); 

    static void addQuadrangle(  const glm::vec3 & A,
                                const glm::vec3 & B,
                                const glm::vec3 & C,
                                const glm::vec3 & D,
                                const glm::vec4 & color,
								const size_t	  slot);

    static void addTriangle(const glm::vec3 & A,
                            const glm::vec3 & B,
                            const glm::vec3 & C,
                            const glm::vec4 & color,
							const size_t	  slot);

    static void addLine(const glm::vec3 & A,
                        const glm::vec3 & B,
                        const glm::vec4 & color,
						const size_t	  slot);

	static void addLine(const glm::vec2 & A,
                        const glm::vec2 & B,
                        const glm::vec4 & color,
						const size_t	  slot);

	static void addWireBox(	const glm::vec2 & min,
							const glm::vec2 & max,
							const glm::vec4 & color,
							const size_t	  slot);

    void render(Canvas * canvas, const size_t renderStage = 0) override;

	void render(const glm::mat4	& model,
				const glm::mat4	& view,
				const glm::mat4	& projection,
				const size_t	  renderStage) override;

	void setFromQueued(const size_t slot, const glm::dmat4 & trans = glm::dmat4(1.0));

private:

    DeferredRenderable( const GLuint & vao,
                        const GLuint & ebo,
                        const GLuint & ebo2,
                        const GLuint & vbo,
                        const GLuint & numTriIndices,
                        const GLuint & numLineIndices);

	void render() const;

    const GLuint m_vao;
    const GLuint m_ebo;
    const GLuint m_ebo2;
    const GLuint m_vbo;
    GLuint		 m_numTriIndices;
    GLuint		 m_numLineIndices;
};
