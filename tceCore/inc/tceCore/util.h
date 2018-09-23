#ifndef UTIL_H_
#define UTIL_H_

#include <sys/types.h>
#include <string.h>

static inline void zeroMemory(void * buffer, const u_int len) { memset(buffer, 0, len) ;}

#endif /*UTIL_H_*/
