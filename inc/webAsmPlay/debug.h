#ifndef __POINT_CLOUD_DEMO_DEBUG_H__
#define __POINT_CLOUD_DEMO_DEBUG_H__

#include <iostream>
#include <sstream>

extern void (*debugLoggerFunc)(const std::string & file, const size_t line, const std::string & message);

#define dmess(a) \
{ \
    if(debugLoggerFunc) \
    { \
        std::stringstream ss; ss << "[" << a << std::endl; \
        std::string f(__FILE__); \
        (*debugLoggerFunc)(f.substr(strlen(SOURCE_BASE_PATH)), __LINE__, ss.str()); \
    } \
} \

#endif