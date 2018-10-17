#include <webAsmPlay/Debug.h>

void (*debugLoggerFunc)(const std::string & file, const size_t line, const std::string & message) = NULL;

