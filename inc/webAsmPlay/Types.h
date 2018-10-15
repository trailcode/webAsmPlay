#ifndef __WEB_ASM_PLAY_TYPES_H__
#define __WEB_ASM_PLAY_TYPES_H__

#include <tuple>

#ifndef __EMSCRIPTEN__
#include <mutex>
#include <thread>
#endif

typedef std::tuple<double, double, double, double> AABB2D;

#ifndef __EMSCRIPTEN__

    #define ScopedLock std::scoped_lock ScopedLock
    //#define std::mutex Mutex;

#else

#endif

#endif // __WEB_ASM_PLAY_TYPES_H__

