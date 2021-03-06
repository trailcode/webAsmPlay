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
    uint16_t nums[8];

    memcpy(nums, attrs, sizeof(nums));

    attrs += sizeof(nums);

    for(size_t i = 0; i < nums[0]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        m_ints32[key] = *(int32_t *)attrs; attrs += sizeof(int32_t);
    }

    for(size_t i = 0; i < nums[1]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        m_ints64[key] = *(int64_t *)attrs; attrs += sizeof(int64_t);
    }

    for(size_t i = 0; i < nums[2]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

		m_uints32[key] = *(int32_t *)attrs; attrs += sizeof(uint32_t);
    }

    for(size_t i = 0; i < nums[3]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

		m_uints64[key] = *(int64_t *)attrs; attrs += sizeof(uint64_t);
    }

    for(size_t i = 0; i < nums[4]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

		m_doubles[key] = *(double *)attrs; attrs += sizeof(double);
    }

    for(size_t i = 0; i < nums[5]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        const string value(attrs); attrs += value.length() + 1;

		m_strings[key] = value;
    }

    for(size_t i = 0; i < nums[6]; ++i)
    {
        const string key(attrs); attrs += key.length() + 1;

        StrVec & values = m_multiStrs[key];

        const uint16_t numValues = *(uint16_t *)attrs; attrs += sizeof(uint16_t);

        for(size_t i = 0; i < numValues; ++i)
        {
            const string value = string(attrs);

            values.push_back(value);

            attrs += value.length() + 1;
        }
    }

	for (size_t i = 0; i < nums[7]; ++i)
	{
		const string key(attrs); attrs += key.length() + 1;

		Uint32Vec& values = m_multiUints32s[key];

		const uint16_t numValues = *(uint16_t*)attrs; attrs += sizeof(uint16_t);

		for (size_t i = 0; i < numValues; ++i)
		{
			values.push_back(*(int32_t*)attrs); attrs += sizeof(uint32_t);
		}
	}
}

Attributes::Attributes(const unordered_map<string, unordered_set<string> > & multiStrsIn)
{
    for(const unordered_map<string, unordered_set<string> >::value_type & i : multiStrsIn)
    {
        for(const string & value : i.second) { m_multiStrs[i.first].push_back(value) ;}
    }
}

void Attributes::write(ostream & out) const
{
    const uint16_t nums [] = {  (uint16_t)m_ints32			.size(),
                                (uint16_t)m_ints64			.size(),
                                (uint16_t)m_uints32			.size(),
                                (uint16_t)m_uints64			.size(),
                                (uint16_t)m_doubles			.size(),
                                (uint16_t)m_strings			.size(),
                                (uint16_t)m_multiStrs		.size(),
								(uint16_t)m_multiUints32s	.size(),
                             };

    out.write((const char *)nums, sizeof(nums));

    for(const auto & i : m_ints32)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(int32_t));
    }

    for(const auto & i : m_ints64)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(int64_t));
    }

    for(const auto & i : m_uints32)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(uint32_t));
    }

    for(const auto & i : m_uints64)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(uint64_t));
    }

    for(const auto & i : m_doubles)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write((const char *)&i.second, sizeof(double));
    }

    for(const auto & i : m_strings)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        out.write(i.second.data(), i.second.length() + 1);
    }

    for(const auto & i : m_multiStrs)
    {
        out.write(i.first.c_str(), i.first.length() + 1);

        const uint16_t numValues = (uint16_t)i.second.size();

        out.write((const char *)&numValues, sizeof(uint16_t));

        for(const string & value : i.second) { out.write(value.c_str(), value.length() + 1) ;}
    }

	for (const auto& i : m_multiUints32s)
	{
		out.write(i.first.c_str(), i.first.length() + 1);

		const uint16_t numValues = (uint16_t)i.second.size();

		out.write((const char*)& numValues, sizeof(uint16_t));

		for (const uint32_t value : i.second) { out.write((const char*)& value, sizeof(uint32_t)) ;}
	}
}

string Attributes::toString() const
{
    stringstream attrsStrStream;

    for(const auto & i : m_ints32)  { attrsStrStream << "int32 "  << i.first << ": "  << i.second << endl ;}
    for(const auto & i : m_ints64)  { attrsStrStream << "int64 "  << i.first << ": "  << i.second << endl ;}
    for(const auto & i : m_uints32) { attrsStrStream << "uint32 " << i.first << ": "  << i.second << endl ;}
    for(const auto & i : m_uints64) { attrsStrStream << "uint64 " << i.first << ": "  << i.second << endl ;}
    for(const auto & i : m_doubles) { attrsStrStream << "dbl "    << i.first << ": "  << i.second << endl ;}
    for(const auto & i : m_strings) { attrsStrStream << "["       << i.first << "]: " << i.second << endl ;}

    for(const auto & i : m_multiStrs)
    {
        attrsStrStream << "[ " << i.first;

        for(const auto & j : i.second) { attrsStrStream << endl << "  " << j ;}

        attrsStrStream << "]" << endl;
    }

	for (const auto& i : m_multiUints32s)
	{
		attrsStrStream << "[ " << i.first;

		for (const auto& j : i.second) { attrsStrStream <<  "  " << j; }

		attrsStrStream << "]" << endl;
	}

    return attrsStrStream.str();
}

bool Attributes::hasStringKey(const string & key) const { return m_strings.find(key) != m_strings.end() ;}

bool Attributes::hasStringKeyValue(const string & key, const string & value) const
{
    Strings::const_iterator i = m_strings.find(key);

    if(i == m_strings.end()) { return false ;}

    return i->second == value;
}