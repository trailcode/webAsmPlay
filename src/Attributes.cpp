#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Attributes.h>

using namespace std;

Attributes::Attributes() {}

Attributes::Attributes(const char *& attrs)
{
    uint16_t nums[4];

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
}

void Attributes::write(ostream & out) const
{
    const uint16_t nums [] = {  (uint16_t)ints32.size(),
                                (uint16_t)ints64.size(),
                                (uint16_t)doubles.size(),
                                (uint16_t)strings.size()
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
}