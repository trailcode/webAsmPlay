#include <webAsmPlay/Debug.h>

void (*debugLoggerFunc)(const std::string & file, const std::string & line, const std::string & message) = NULL;

