
//#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <tceRender/CGLState.h>

using namespace tce;
using namespace tce::geom::quake3;

void
CGLState::setBlendFunc (const Q3BSPShaderLayer & layer)
{

	if(layer.blendingParms == BLEND_NO_BLENDING)
	{
		glDisable(GL_BLEND);
		return;
	}

	glEnable (GL_BLEND);

	unsigned int sourceCommand;

	if ((layer.blendingParms & BLEND_SOURCE_MASK) == BLEND_SOURCE_GL_ONE)
	{
		sourceCommand = GL_ONE;

	}
	else if ((layer.blendingParms & BLEND_SOURCE_MASK) == BLEND_SOURCE_GL_ZERO)
	{
		sourceCommand = GL_ZERO;

	}
	else if ((layer.blendingParms & BLEND_SOURCE_MASK) == BLEND_SOURCE_GL_DST_COLOR)
	{
		sourceCommand = GL_DST_COLOR;

	}
	else if ((layer.blendingParms & BLEND_SOURCE_MASK) ==
		 BLEND_SOURCE_GL_ONE_MINUS_DST_COLOR)
	{
		sourceCommand = GL_ONE_MINUS_DST_COLOR;

	}
	else if ((layer.blendingParms & BLEND_SOURCE_MASK) == BLEND_SOURCE_GL_SRC_ALPHA)
	{
		sourceCommand = GL_SRC_ALPHA;

	}
	else if ((layer.
		  blendingParms & BLEND_SOURCE_MASK) == BLEND_SOURCE_GL_ONE_MINUS_SRC_ALPHA)
	{
		sourceCommand = GL_ONE_MINUS_SRC_ALPHA;

	}

	unsigned int destCommand;

	if ((layer.blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_ONE)
	{
		destCommand = GL_ONE;

	}
	else if ((layer.blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_ZERO)
	{
		destCommand = GL_ZERO;

	}
	else if ((layer.blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_SRC_COLOR)
	{
		destCommand = GL_SRC_COLOR;

	}
	else if ((layer.
		  blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_ONE_MINUS_SRC_COLOR)
	{
		destCommand = GL_ONE_MINUS_SRC_COLOR;

	}
	else if ((layer.blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_SRC_ALPHA)
	{
		destCommand = GL_SRC_ALPHA;

	}
	else if ((layer.
		  blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_ONE_MINUS_SRC_ALPHA)
	{
		destCommand = GL_ONE_MINUS_SRC_ALPHA;

	} else if((layer.blendingParms & BLEND_DESTIN_MASK) == BLEND_DESTIN_GL_ONE_MINUS_DST_ALPHA)
	{
		destCommand = GL_ONE_MINUS_DST_ALPHA;
	}

	glBlendFunc (sourceCommand, destCommand);
}

void CGLState::setDepthWrite(const Q3BSPShaderLayer & layer)
{
	if(layer.depthWrite)
	{
		glDepthMask(GL_TRUE);
	} else {
		glDepthMask(GL_FALSE);
	}
};

void CGLState::setAlphaFunc(const Q3BSPShaderLayer & layer)
{
	
	switch(layer.alphaFunc)
	{
		case ALPHA_FUNC_NO_ALPHA:

			glDisable(GL_ALPHA_TEST);

			break;
		case ALPHA_FUNC_GREATER_THAN_0:

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0);

			break;
		case ALPHA_FUNC_LESS_THAN_128:

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_LESS, 0.5);

			break;
		case ALPHA_FUNC_GREATER_THAN_OR_EQUAL_TO_128:

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.5);

			break;
	}

}

void CGLState::setDefaultState()
{
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_ALPHA_TEST);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	// Enable front face culling, since that's what Quake3 does
	glCullFace (GL_FRONT);
	glEnable (GL_CULL_FACE);

}

void CGLState::setDepthFunc(const Q3BSPShaderLayer & layer)
{
	switch(layer.depthFunc)
	{
		case DEPTH_FUNC_LESS_EQUAL:
			glDepthFunc(GL_LEQUAL);

			break;
		case DEPTH_FUNC_EQUAL:
			glDepthFunc(GL_EQUAL);
			break;
	}

}

void CGLState::setCullFunc(const Q3BSPShader & shader)
{
	switch(shader.params)
	{
		case CULL_FUNC_FRONT:

			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);

			break;

		case CULL_FUNC_BACK:

			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			break;

		case CULL_FUNC_NONE:

			glDisable(GL_CULL_FACE);
			break;

		default:

			break;

	}
}
