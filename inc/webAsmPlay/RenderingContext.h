#pragma once

#include <glm/mat4x4.hpp>
#include <webAsmPlay/OpenGL_Util.h>

class Frustum;

class RenderingContext
{
	public:

		virtual size_t getFrameNumber() const;

		virtual glm::ivec2 getFrameBufferSize() const;

		virtual glm::dmat4 getView()		const;
		virtual glm::dmat4 getModel()		const;
		virtual glm::dmat4 getProjection()	const;
		virtual glm::dmat4 getMVP()			const;
		virtual glm::dmat4 getMV()			const;

		virtual glm::vec3 getCameraCenter() const;

		virtual const Frustum * getCameraFrustum() const;

		virtual GLuint getG_Buffer() const;

	protected:
};
