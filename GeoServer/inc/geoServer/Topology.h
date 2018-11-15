#ifndef __WEB_ASM_PLAY_TOPOLOGY_H__
#define __WEB_ASM_PLAY_TOPOLOGY_H__

#include <webAsmPlay/Types.h>

namespace topology
{
    std::vector<AttributedLineString> breakLineStrings(std::vector<AttributedLineString> & lineStrings);

    void discoverTopologicalRelations(std::vector<AttributedPoligonalArea> & polygons);
}

#endif // __WEB_ASM_PLAY_TOPOLOGY_H__