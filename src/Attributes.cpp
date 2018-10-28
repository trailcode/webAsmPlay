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

#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>

using namespace std;

Attributes::Attributes() {}

Attributes::Attributes(const char *& attrs)
{
    uint16_t nums[5];

    memcpy(nums, attrs, sizeof(nums));

    attrs += sizeof(nums);

    for(size_t i = 0; i < nums[0]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        ints32[key] = *(int32_t *)attrs; attrs += sizeof(int32_t);
    }

    for(size_t i = 0; i < nums[1]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        ints64[key] = *(int64_t *)attrs; attrs += sizeof(int64_t);
    }

    for(size_t i = 0; i < nums[2]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        doubles[key] = *(double *)attrs; attrs += sizeof(double);
    }

    for(size_t i = 0; i < nums[3]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        const string value(attrs); attrs += value.length() + 1;

        strings[key] = value;
    }

    //dmess("nums[4] " << nums[4]);

    for(size_t i = 0; i < nums[4]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        StrVec & values = multiStrs[key];

        const uint16_t numValues = *(uint16_t *)attrs; attrs += sizeof(uint16_t);

        for(size_t i = 0; i < numValues; ++i)
        {
            const string value = string(attrs);

            values.push_back(value);

            attrs += value.length() + 1;
        }
    }

    //dmess("done");
}

Attributes::Attributes(const unordered_map<string, unordered_set<string> > & multiStrsIn)
{
    //dmess("multiStrs " << multiStrs.size());

    for(const unordered_map<string, unordered_set<string> >::value_type & i : multiStrsIn)
    {
        for(const string & value : i.second) { multiStrs[i.first].push_back(value) ;}
    }
}

void Attributes::write(ostream & out) const
{
    const uint16_t nums [] = {  (uint16_t)ints32   .size(),
                                (uint16_t)ints64   .size(),
                                (uint16_t)doubles  .size(),
                                (uint16_t)strings  .size(),
                                (uint16_t)multiStrs.size(),
                             };

    out.write((const char *)nums, sizeof(nums));

    for(const Ints32::value_type & i : ints32)
    {
        out.write(i.first.data(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(int32_t));
    }

    for(const Ints64::value_type & i : ints64)
    {
        out.write(i.first.data(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(int64_t));
    }

    for(const Doubles::value_type & i : doubles)
    {
        out.write(i.first.data(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(double));
    }

    for(const Strings::value_type & i : strings)
    {
        out.write(i.first.data(), i.first.length() + 1);

        out.write(i.second.data(), i.second.length() + 1);
    }

    for(const MultiStrs::value_type & i : multiStrs)
    {
        out.write(i.first.data(), i.first.length() + 1);

        const uint16_t numValues = i.second.size();

        out.write((const char *)&numValues, sizeof(uint16_t));

        for(const string & value : i.second)
        {
            out.write(value.data(), value.length() + 1);
        }
    }
}

string Attributes::toString() const
{
    stringstream attrsStrStream;

    for(const Attributes::Ints32 ::value_type & i : ints32)  { attrsStrStream << i.first << ": " << i.second << endl ;}
    for(const Attributes::Ints64 ::value_type & i : ints64)  { attrsStrStream << i.first << ": " << i.second << endl ;}
    for(const Attributes::Doubles::value_type & i : doubles) { attrsStrStream << i.first << ": " << i.second << endl ;}
    for(const Attributes::Strings::value_type & i : strings) { attrsStrStream << "[" << i.first << "]: " << i.second << endl ;}

    dmess("multiStrs " << multiStrs.size());

    return attrsStrStream.str();
}

bool Attributes::hasStringKey(const string & key) const { return strings.find(key) != strings.end() ;}

bool Attributes::hasStringKeyValue(const string & key, const string & value) const
{
    Strings::const_iterator i = strings.find(key);

    if(i == strings.end()) { return false ;}

    return i->second == value;
}