#include <webAsmPlay/Util.h>

using namespace std;
using namespace glm;

string mat4ToStr(const mat4 & m)
{
    char buf[1024];

    sprintf(buf,
            "[[% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]\n"
            " [% 2.9f, % 2.9f, % 2.9f, % 2.9f]]\n",
            static_cast<double>(m[0][0]), static_cast<double>(m[0][1]), static_cast<double>(m[0][2]), static_cast<double>(m[0][3]),
            static_cast<double>(m[1][0]), static_cast<double>(m[1][1]), static_cast<double>(m[1][2]), static_cast<double>(m[1][3]),
            static_cast<double>(m[2][0]), static_cast<double>(m[2][1]), static_cast<double>(m[2][2]), static_cast<double>(m[2][3]),
            static_cast<double>(m[3][0]), static_cast<double>(m[3][1]), static_cast<double>(m[3][2]), static_cast<double>(m[3][3]));

    return buf;
}