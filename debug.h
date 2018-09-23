#ifndef __POINT_CLOUD_DEMO_DEBUG_H__
#define __POINT_CLOUD_DEMO_DEBUG_H__

#include <iostream>
#include <sstream>

extern void (*debugLoggerFunc)(const std::string & file, const std::string & line, const std::string & message);

#define dmess(a) { std::stringstream ss; ss << __FILE__ << ":" << __LINE__ << "[" << a << endl; std::cout << ss.str() ;}

#endif