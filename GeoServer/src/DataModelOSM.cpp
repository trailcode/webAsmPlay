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

#include <geos/geom/CoordinateArraySequence.h>
#include <geoServer/DataModelOSM.h>

using namespace std;
using namespace geos::geom;
using namespace dataModelOSM;

namespace
{
    unordered_map<string, size_t> keyMap // TODO split into multiple maps for performance
    {
        { "osm",        OSM_KEY_OSM         },
        { "generator",  OSM_KEY_GENERATOR   },
        { "note",       OSM_KEY_NOTE        },
        { "remark",     OSM_KEY_REMARK      },
        { "bounds",     OSM_KEY_BOUNDS      },
        { "minlat",     OSM_KEY_MIN_LAT     },
        { "minlon",     OSM_KEY_MIN_LON     },
        { "maxlat",     OSM_KEY_MAX_LAT     },
        { "maxlon",     OSM_KEY_MAX_LON     },

        { "member",     OSM_KEY_MEMBER      },
        { "tag",        OSM_KEY_TAG         },
        { "meta",       OSM_KEY_META        },
        { "node",       OSM_KEY_NODE        },
        { "way",        OSM_KEY_WAY         },
        { "relation",   OSM_KEY_RELATION    },
        { "nd",         OSM_KEY_ND          },
        
        { "role",       OSM_KEY_ROLE        },
        { "ref",        OSM_KEY_REF         },
        { "type",       OSM_KEY_TYPE        },
        { "v",          OSM_KEY_V           },
        { "k",          OSM_KEY_K           },
        { "osm_base",   OSM_KEY_OSM_BASE    },
        { "changeset",  OSM_KEY_CHANGESET   },
        { "timestamp",  OSM_KEY_TIMESTAMP   },
        { "user",       OSM_KEY_USER        },
        { "lat",        OSM_KEY_LAT         },
        { "lon",        OSM_KEY_LON         },
        { "version",    OSM_KEY_VERSION     },
        { "uid",        OSM_KEY_UID         },
        { "id",         OSM_KEY_ID          },
    };

    unordered_map<string, size_t> relationKeyMap // TODO split into multiple maps for performance
    {
        { "node",       OSM_TYPE_NODE   },
        { "way",        OSM_TYPE_WAY    },

        { "inner",      OSM_ROLE_INNER  },
        { "outer",      OSM_ROLE_OUTER  },
    };
}

size_t dataModelOSM::getKey(const string & key)
{
    const auto i = keyMap.find(key);

    if(i == keyMap.end()) { return 0 ;}

    return i->second;
}

size_t dataModelOSM::getRelationKey(const string & key)
{
    const auto i = relationKeyMap.find(key);

    if(i == relationKeyMap.end()) { return 0 ;}

    return i->second;
}

CoordinateArraySequence * dataModelOSM::getGeosCoordinateSequence(const vector<const OSM_Node *> & nodes)
{
    vector<Coordinate> * coords = new vector<Coordinate>(nodes.size());

    for(size_t i = 0; i < nodes.size(); ++i)
    {
        const OSM_Node * n = nodes[i];

        (*coords)[i] = Coordinate(n->m_pos.x, n->m_pos.y);
    }

    return new CoordinateArraySequence(coords, 2);
}