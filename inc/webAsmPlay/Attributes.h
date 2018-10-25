#ifndef __WEB_ASM_PLAY_ATTRIBUTES_H__
#define __WEB_ASM_PLAY_ATTRIBUTES_H__

#include <string>
#include <ostream>
#include <unordered_map>

class Attributes
{
public:

    Attributes();
    Attributes(const char *& attrs);

    void write(std::ostream & out) const;

    std::string toString() const;

    bool hasStringKey(const std::string & key) const;

    bool hasStringKeyValue(const std::string & key, const std::string & value) const;

    typedef std::unordered_map<std::string, int32_t>        Ints32;
    typedef std::unordered_map<std::string, int64_t>        Ints64;
    typedef std::unordered_map<std::string, double >        Doubles;
    typedef std::unordered_map<std::string, std::string>    Strings;

    Ints32  ints32;
    Ints64  ints64;
    Doubles doubles;
    Strings strings;
};

#endif // __WEB_ASM_PLAY_ATTRIBUTES_H__