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

  Modified from: https://github.com/0xc0dec/demos

  \author Matthew Tang
  \email trailcode@gmail.com
  \copyright 2019
*/

//#define STB_TRUETYPE_IMPLEMENTATION
//#include <imgui/imstb_truetype.h>

#include <webAsmPlay/GUI/ImguiInclude.h>

#ifndef STB_TRUETYPE_IMPLEMENTATION                         // in case the user already have an implementation in the _same_ compilation unit (e.g. unity builds)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u)   ((void)(u), IM_ALLOC(x))
#define STBTT_free(x,u)     ((void)(u), IM_FREE(x))
#define STBTT_assert(x)     IM_ASSERT(x)
#define STBTT_fmod(x,y)     ImFmod(x,y)
#define STBTT_sqrt(x)       ImSqrt(x)
#define STBTT_pow(x,y)      ImPow(x,y)
#define STBTT_fabs(x)       ImFabs(x)
#define STBTT_ifloor(x)     ((int)ImFloorStd(x))
#define STBTT_iceil(x)      ((int)ImCeil(x))
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#ifdef IMGUI_STB_TRUETYPE_FILENAME
#include IMGUI_STB_TRUETYPE_FILENAME
#else
#include "imstb_truetype.h"
#endif
#endif

#include <fstream>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <webAsmPlay/Util.h>
#include <webAsmPlay/GUI/GUI.h>
#include <webAsmPlay/canvas/Canvas.h>
#include <webAsmPlay/renderables/RenderableText.h>

using namespace std;
using namespace glm;

namespace
{
	static struct
	{
		struct
		{
			const char* font = R"(
				#version 330 core
				in vec4 position;
				in vec2 texCoord0;
				uniform mat4 worldMatrix;
				uniform mat4 viewProjMatrix;
				out vec2 uv0;
				void main()
				{
					gl_Position = viewProjMatrix * worldMatrix * position;
					uv0 = texCoord0;
				}
			)";
		} vertex;

		struct
		{
			const char* font = R"(
				#version 330 core
				uniform sampler2D mainTex;
				in vec2 uv0;
				out vec4 fragColor;
				void main()
				{
					vec4 c = texture(mainTex, uv0);
					fragColor = vec4(c.r, c.r, c.r, c.r);
				}
			)";
		} fragment;
	} shaders;

	struct GlyphInfo
	{
		glm::vec3 positions[4];
		glm::vec2 uvs[4];

		float offsetX = 0;
		float offsetY = 0;
	};

	struct
	{
		GLuint handle = 0;

		struct
		{
			GLuint viewProjMatrix = 0;
			GLuint worldMatrix = 0;
			GLuint texture = 0;
		} uniforms;
	} program;

	glm::mat4 viewProjMatrix;

	struct
	{
		const uint32_t size = 80; // 40;
		const uint32_t atlasWidth = 1024 * 3;
		const uint32_t atlasHeight = 1024 * 3;
		const uint32_t oversampleX = 1; // 2
		const uint32_t oversampleY = 1; // 2
		const uint32_t firstChar = ' ';
		const uint32_t charCount = '~' - ' ';
		std::unique_ptr<stbtt_packedchar[]> charInfo;
		GLuint texture = 0;
	} font;

	auto compileShader(GLenum type, const char* src) -> GLuint
	{
		static std::unordered_map<GLuint, std::string> typeNames =
		{
			{ GL_VERTEX_SHADER, "vertex" },
			{ GL_FRAGMENT_SHADER, "fragment" }
		};

		const auto shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<GLchar> log(logLength);
			glGetShaderInfoLog(shader, logLength, nullptr, log.data());
			glDeleteShader(shader);
			dmessError("Failed to compile " << typeNames[type] << " shader: " << log.data());
		}

		return shader;
	}

	auto linkProgram(GLuint vs, GLuint fs) -> GLint
	{
		const auto program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<GLchar> log(logLength);
			glGetProgramInfoLog(program, logLength, nullptr, log.data());
			glDeleteProgram(program);
			dmessError("Failed to link program: " << log.data());
		}

		return program;
	}

	auto createProgram(const char* vs, const char* fs) -> GLuint
	{
		const auto vertex = compileShader(GL_VERTEX_SHADER, vs);
		const auto fragment = compileShader(GL_FRAGMENT_SHADER, fs);
		const auto program = linkProgram(vertex, fragment);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		return program;
	}

	auto getGlyphInfo(uint32_t character, float offsetX, float offsetY) -> GlyphInfo
	{
		stbtt_aligned_quad quad;

		stbtt_GetPackedQuad(font.charInfo.get(), font.atlasWidth, font.atlasHeight,
			character - font.firstChar, &offsetX, &offsetY, &quad, 0);

		const float scale = 0.001;

		auto xmin =  quad.x0 * scale;
		auto xmax =  quad.x1 * scale;
		auto ymin = -quad.y1 * scale;
		auto ymax = -quad.y0 * scale;

		GlyphInfo info{};
		info.offsetX = offsetX;
		info.offsetY = offsetY;
		info.positions[0] = {xmin, ymin, 0};
		info.positions[1] = {xmin, ymax, 0};
		info.positions[2] = {xmax, ymax, 0};
		info.positions[3] = {xmax, ymin, 0};
		info.uvs[0] = {quad.s0, quad.t1};
		info.uvs[1] = {quad.s0, quad.t0};
		info.uvs[2] = {quad.s1, quad.t0};
		info.uvs[3] = {quad.s1, quad.t1};

		return info;
	}

	auto readFile(const char* path) -> std::vector<uint8_t>
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
			
		if (!file.is_open())
			dmessError("Failed to open file " << path);
    
		const auto size = file.tellg();
		file.seekg(0, std::ios::beg);
		auto bytes = std::vector<uint8_t>(size);
		file.read(reinterpret_cast<char*>(&bytes[0]), size);
		file.close();
    
		return bytes;
	}

	void initFont()
	{
		//auto fontData = readFile("C:/build/webAsmPlay/ThirdParty/imgui/misc/fonts/ProggyClean.ttf");
		//auto fontData = readFile("C:/build/webAsmPlay/ThirdParty/imgui/misc/fonts/DroidSans.ttf");
		//auto fontData = readFile("C:/windows/fonts/arial.ttf");
		//auto fontData = readFile("C:\\build\\webAsmPlay\\readme.md");
		auto fontData = readFile("ProggyClean.ttf");

		auto atlasData = std::make_unique<uint8_t[]>(font.atlasWidth * font.atlasHeight);

		font.charInfo = std::make_unique<stbtt_packedchar[]>(font.charCount);

		stbtt_pack_context context;
		if (!stbtt_PackBegin(&context, atlasData.get(), font.atlasWidth, font.atlasHeight, 0, 1, nullptr))
			dmessError("Failed to initialize font");

		stbtt_PackSetOversampling(&context, font.oversampleX, font.oversampleY);
		if (!stbtt_PackFontRange(&context, fontData.data(), 0, font.size, font.firstChar, font.charCount, font.charInfo.get()))
			dmessError("Failed to pack font");

		stbtt_PackEnd(&context);

		glGenTextures(1, &font.texture);
		glBindTexture(GL_TEXTURE_2D, font.texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font.atlasWidth, font.atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
		//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFlush();
	}

	void initUniforms()
	{
		/*
		const glm::mat4 viewMatrix{};
		//const auto projMatrix = glm::perspective(glm::radians(60.0f), 1.0f * canvasWidth / canvasHeight, 0.05f, 100.0f);
		const auto projMatrix = mat4(theProjM);
		viewProjMatrix = projMatrix * viewMatrix;
		*/

		program.uniforms.viewProjMatrix = glGetUniformLocation(program.handle, "viewProjMatrix");
		program.uniforms.worldMatrix = glGetUniformLocation(program.handle, "worldMatrix");
		program.uniforms.texture = glGetUniformLocation(program.handle, "mainTex");
	}
}

Renderable * RenderableText::create(const string	& text,
									const dvec3		& pos,
									const dmat4		& trans)
{
	RenderableText * ret = new RenderableText();

	ret->setText(text);

	ret->setPos(pos);

	return ret;
}

RenderableText::RenderableText() : Renderable(	false,
												GUI::s_renderSettingsFillPolygons,
												GUI::s_renderSettingsRenderPolygonOutlines)
{
	if(!font.texture) { initFont() ;}
}

RenderableText::~RenderableText()
{

}

void RenderableText::render(Canvas * canvas, const size_t renderStage)
{
	ensureVAO();

	auto _viewProjMatrix = mat4(canvas->getProjectionRef() * canvas->getViewRef());

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(program.handle);

	glUniformMatrix4fv(program.uniforms.viewProjMatrix, 1, GL_FALSE, glm::value_ptr(_viewProjMatrix));

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, font.texture);
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	*/
			
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glUniform1i(program.uniforms.texture, 9);

	const vec3 theTrans(0,0,0);

	const auto worldMatrix = glm::translate(glm::scale(glm::mat4(1.0f), {1, 1, 1}), vec3(m_pos));

	glUniformMatrix4fv(program.uniforms.worldMatrix, 1, GL_FALSE, glm::value_ptr(worldMatrix));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, indexElementCount, GL_UNSIGNED_SHORT, nullptr);
}

void RenderableText::ensureVAO()
{
	if(!program.handle)
	{
		program.handle = createProgram(shaders.vertex.font, shaders.fragment.font);

		initUniforms();
	}
}

string RenderableText::setText(const string & text)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<uint16_t> indexes;

	uint16_t lastIndex = 0;
	float offsetX = 0, offsetY = 0;
	for (auto c : text)
	{
		const auto glyphInfo = getGlyphInfo(c, offsetX, offsetY);
		offsetX = glyphInfo.offsetX;
		offsetY = glyphInfo.offsetY;

		vertices.emplace_back(glyphInfo.positions[0]);
		vertices.emplace_back(glyphInfo.positions[1]);
		vertices.emplace_back(glyphInfo.positions[2]);
		vertices.emplace_back(glyphInfo.positions[3]);
		uvs.emplace_back(glyphInfo.uvs[0]);
		uvs.emplace_back(glyphInfo.uvs[1]);
		uvs.emplace_back(glyphInfo.uvs[2]);
		uvs.emplace_back(glyphInfo.uvs[3]);
		indexes.push_back(lastIndex);
		indexes.push_back(lastIndex + 1);
		indexes.push_back(lastIndex + 2);
		indexes.push_back(lastIndex);
		indexes.push_back(lastIndex + 2);
		indexes.push_back(lastIndex + 3);

		lastIndex += 4;
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &uvBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	indexElementCount = indexes.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexElementCount, indexes.data(), GL_STATIC_DRAW);

	return text;
}

dvec3 RenderableText::setPos(const dvec3 & pos) { return m_pos = pos ;}
