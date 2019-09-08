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

#include <initializer_list>
#include <unordered_set>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <OpenSteer/Vec3.h>
#include <OpenSteer/Color.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/GUI/GUI.h>
#include <imgui.h>

// TODO add util namespace

std::string toStr(const glm::mat4 & m);

std::string toStr(const glm::vec2 & v);

std::string toStr(const glm::ivec2 & v);

std::string toStr(const glm::vec3 & v);

std::string toStr(const glm::vec4 & v);

template<typename T>
inline std::vector<T> toVec(const std::initializer_list<T>& list)
{
	std::vector<T> ret;

	ret.reserve(list.size());

	for (const auto& i : list) { ret.push_back(i); }

	return ret;
}

template<typename T>
inline std::vector<T> toVec(const std::unordered_set<T> & list)
{
	std::vector<T> ret;

	ret.reserve(list.size());

	for (const auto& i : list) { ret.push_back(i); }

	return ret;
}

static glm::ivec2 __(const ImVec2 & v) { return glm::ivec2(v.x, v.y) ;}

static ImVec4 __(const glm::vec4 & v) { return ImVec4(v.x, v.y, v.z, v.q) ;}

static glm::vec4 __(const ImVec4 & v) { return glm::vec4(v.x, v.y, v.z, v.w) ;}

static glm::vec3 __(const OpenSteer::Vec3 & v) { return glm::vec3(v.x, v.y, v.z) ;}

static glm::vec4 __(const OpenSteer::Color & v) { return glm::vec4(v.r(), v.g(), v.b(), v.a()) ;}

static auto toTuple(const glm::vec1 & P) { return std::make_tuple(P.x) ;}

static auto toTuple(const glm::vec2 & P) { return std::make_tuple(P.x, P.y) ;}

static auto toTuple(const glm::vec3 & P) { return std::make_tuple(P.x, P.y, P.z) ;}

static auto toTuple(const glm::vec4 & P) { return std::make_tuple(P.x, P.y, P.z, P.w) ;}

static glm::vec1 toVec1(const nlohmann::json & P) { return glm::vec1(P[0]) ;}

static glm::vec2 toVec2(const nlohmann::json & P) { return glm::vec2(P[0], P[1]) ;}

static glm::vec3 toVec3(const nlohmann::json & P) { return glm::vec3(P[0], P[1], P[2]) ;}

static glm::vec4 toVec4(const nlohmann::json & P) { return glm::vec4(P[0], P[1], P[2], P[3]) ;}

std::wstring stringToWstring(const std::string& t_str);

std::string wstringToString(const std::wstring& t_str);

template<typename T>
inline std::unordered_set<T> toSet(const std::vector<T>& elms)
{
	std::unordered_set<T> ret;

	for (size_t i = 0; i < elms.size(); ++i) { ret.insert(elms[i]) ;}

	return ret;
}

void doProgress(const std::string                                    & message,
                const size_t                                         i,
                const size_t                                         num,
                std::chrono::time_point<std::chrono::system_clock> & startTime,
                const size_t                                         sampleRate = 1000);

inline uint32_t getUint32(const char *& dataStream)
{
    const uint32_t ret = *(uint32_t *)dataStream; dataStream += sizeof(uint32_t);

    return ret;
}

inline double getDouble(const char *& dataStream)
{
    const double ret = *(double *)dataStream; dataStream += sizeof(double);

    return ret;
}

inline glm::dvec2 getDVec2(const char *& dataStream)
{
    const double x = getDouble(dataStream);
    const double y = getDouble(dataStream);

    return glm::dvec2(x, y);
}

inline float append(float *& dataStream, const float value)
{
    *dataStream = value; 

    ++dataStream;

    return value;
}

inline uint32_t append(uint32_t *& dataStream, const uint32_t value)
{
    *dataStream = value; 

    ++dataStream;

    return value;
}

inline void append( uint32_t       *& dataStream,
                    const uint32_t    valueA,
                    const uint32_t    valueB)
{
    *dataStream = valueA; ++dataStream;
    *dataStream = valueB; ++dataStream;
}

inline void append( float       *& dataStream,
                    const float    valueA,
                    const float    valueB,
                    const float    valueC)
{
    *dataStream = valueA; ++dataStream;
    *dataStream = valueB; ++dataStream;
    *dataStream = valueC; ++dataStream;
}

inline glm::vec3 append(float *& dataStream, const glm::vec3 & value)
{
    memcpy(dataStream, value_ptr(value), sizeof(glm::vec3));

    dataStream += 3;

    return value;
}

inline glm::vec4 append2f(float *& dataStream, const glm::vec4 & value)
{
    *dataStream = value.x; ++dataStream;
    *dataStream = value.y; ++dataStream;

    return value;
}

inline glm::dvec4 append2d(std::vector<double> & data, const glm::dvec4 & value)
{
    data.push_back(value.x);
    data.push_back(value.y);

    return value;
}

template<typename T>
inline void append2ui(std::vector<uint32_t> & data, const T A, const T B)
{
    data.push_back((uint32_t)A);
    data.push_back((uint32_t)B);
}

template<typename T>
inline uint32_t appendUint32(char *& dataStream, const T value)
{
	*(uint32_t *)dataStream = (uint32_t)value;

	dataStream += sizeof(uint32_t);

	return (uint32_t)value;
}

template<typename T>
inline void appendUint32s(char *& dataStream, const T A, const T B, const T C)
{
	appendUint32(dataStream, A);
	appendUint32(dataStream, B);
	appendUint32(dataStream, C);
}

inline char * appendChar(std::vector<char> & data, const char value)
{
	char * ret = &data[0];

	*ret = value;

	return ret + 1;
}

bool fileExists(const std::string& fileName);

std::string readFile(const std::string fileName);

nlohmann::json loadJson(const std::string & fileName);

inline bool hasKey(const nlohmann::json & data, const std::string & key) { return data.find(key) != data.end() ;}

// Convert base 10 to arbitrary base
// - Base must be between 2 and 36
// - If base is invalid, returns "0"
// - NOTE: this whole function could be done with itoa
inline std::string convertFrom10(size_t value, const size_t base)
{
    if(base < 2 || base > 36) { return "0" ;}
    
    const bool isNegative = (value < 0);
	
    if(isNegative) { value *= -1 ;}
    
    // NOTE: it's probably possible to reserve string based on value
    std::string output;
    
    do
    {
        char digit = char(value % base);
     
        // Convert to appropriate base character
        if(digit < 10)	{ digit += '0'				;} // 0-9
        else			{ digit = digit + 'A' - 10	;} // A-Z
        
        // Append digit to string (in reverse order)
        output += digit;
        
        value /= base;
        
    } while (value > 0);
    
    if(isNegative) { output += '-' ;}
    
    // Reverse the string - NOTE: could be done with std::reverse
    const int len = output.size() - 1;

    for(int i = 0; i < len; ++i)
    {
        // Swap characters - NOTE: Could be done with std::swap
        char temp = output[i];
        output[i] = output[len-i];
        output[len-i] = temp;
    }
    
    return output;
}