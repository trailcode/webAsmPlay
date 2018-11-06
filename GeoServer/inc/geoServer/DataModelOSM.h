/**
 ╭━━━━╮╱╱╱╱╱╱╱╱╱╭╮╱╭━━━╮╱╱╱╱╱╱╭╮
 ┃╭╮╭╮┃╱╱╱╱╱╱╱╱╱┃┃╱┃╭━╮┃╱╱╱╱╱╱┃┃
 ╰╯┃┃╰╯╭━╮╭━━╮╭╮┃┃╱┃┃╱╰╯╭━━╮╭━╯┃╭━━╮
 ╱╱┃┃╱╱┃╭╯┃╭╮┃┣┫┃┃╱┃┃╱╭╮┃╭╮┃┃╭╮┃┃┃━┫
 ╱╱┃┃╱╱┃┃╱┃╭╮┃┃┃┃╰╮┃╰━╯┃┃╰╯┃┃╰╯┃┃┃━┫
 ╱╱╰╯╱╱╰╯╱╰╯╰╯╰╯╰━╯╰━━━╯╰━━╯╰━━╯╰━━╯
 //
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

#ifndef __WEB_ASM_PLAY__DATA_MODEL_OSM__H__
#define __WEB_ASM_PLAY__DATA_MODEL_OSM__H__

#include <memory>
#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <geos/geom/Geometry.h>
#include <webAsmPlay/Attributes.h>

class Relation;

namespace dataModelOSM
{
    enum
    {
        OSM_KEY_MEMBER = 1,
        OSM_KEY_TAG,
        OSM_KEY_META,
        OSM_KEY_NODE,
        OSM_KEY_WAY,
        OSM_KEY_RELATION,
        OSM_KEY_ND,
        OSM_KEY_OSM,
        OSM_KEY_NOTE,

        OSM_KEY_ROLE,
        OSM_KEY_REF,
        OSM_KEY_TYPE,
        OSM_KEY_V,
        OSM_KEY_K,
        OSM_KEY_OSM_BASE,
        OSM_KEY_CHANGESET,
        OSM_KEY_TIMESTAMP,
        OSM_KEY_USER,
        OSM_KEY_LAT,
        OSM_KEY_LON,
        OSM_KEY_VERSION,
        OSM_KEY_UID,
        OSM_KEY_ID,
        OSM_KEY_GENERATOR,

        OSM_TYPE_NODE,
        OSM_TYPE_WAY,

        OSM_ROLE_INNER,
        OSM_ROLE_OUTER,
    };

    struct OSM_Base
    {
        OSM_Base() : attrs(new Attributes()) {}

        std::unique_ptr<Attributes> attrs;
    };

    struct OSM_Member
    {
        std::string role;
        uint64_t    ref;
        std::string type;
    };

    struct OSM_Relation : OSM_Base
    {
        OSM_Relation() : relation(NULL) {}

        std::vector<const OSM_Member *> members;

        Relation * relation;
    };

    struct OSM_Node : OSM_Base
    {
        glm::dvec2 pos;

        std::vector<OSM_Relation *> relations;
    };

    struct OSM_Way : OSM_Base
    {
        OSM_Way() : used(false) {}

        std::vector<const OSM_Node *> nodes;

        bool used;

        std::unique_ptr<geos::geom::Geometry> geom;

        std::vector<OSM_Relation *> relations;
    };

    size_t getKey(const std::string & key);

    size_t getRelationKey(const std::string & key);

    geos::geom::CoordinateArraySequence * getGeosCoordinateSequence(const std::vector<const OSM_Node *> & nodes);

    typedef std::unordered_map<uint64_t, OSM_Node     *> OSM_Nodes;
    typedef std::unordered_map<uint64_t, OSM_Way      *> OSM_Ways;
    typedef std::unordered_map<uint64_t, OSM_Relation *> OSM_Relations;
}

#endif // __WEB_ASM_PLAY__DATA_MODEL_OSM__H__