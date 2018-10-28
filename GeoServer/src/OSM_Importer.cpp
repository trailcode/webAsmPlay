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

#include <cstdio>
#include <functional>
#include <expat.h>
#include <unordered_map>
#include <unordered_set>
#include <glm/vec2.hpp>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryFactory.h>
#include <webAsmPlay/GeosUtil.h>
#include <webAsmPlay/Attributes.h>
#include <webAsmPlay/Debug.h>
#include <geoServer/OSM_Importer.h>

#ifdef XML_LARGE_SIZE
# if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#  define XML_FMT_INT_MOD "I64"
# else
#  define XML_FMT_INT_MOD "ll"
# endif
#else
# define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
# include <wchar.h>
# define XML_FMT_STR "ls"
#else
# define XML_FMT_STR "s"
#endif

using namespace std;
using namespace glm;
using namespace geos::geom;
using namespace geosUtil;

#define WARN_UNKNOWN_TAG dmess("Implment: " << name); \
                         for(size_t i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;} \

namespace
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

    unordered_map<string, size_t> keyMap // TODO split into multiple maps for performance
    {
        { "osm",        OSM_KEY_OSM },
        { "generator",  OSM_KEY_GENERATOR },

        { "member",     OSM_KEY_MEMBER },
        { "tag",        OSM_KEY_TAG },
        { "meta",       OSM_KEY_META },
        { "node",       OSM_KEY_NODE },
        { "way",        OSM_KEY_WAY },
        { "relation",   OSM_KEY_RELATION },
        { "nd",         OSM_KEY_ND },
        
        { "role",       OSM_KEY_ROLE },
        { "ref",        OSM_KEY_REF },
        { "type",       OSM_KEY_TYPE },
        { "v",          OSM_KEY_V },
        { "k",          OSM_KEY_K },
        { "osm_base",   OSM_KEY_OSM_BASE },
        { "changeset",  OSM_KEY_CHANGESET },
        { "timestamp",  OSM_KEY_TIMESTAMP },
        { "user",       OSM_KEY_USER },
        { "lat",        OSM_KEY_LAT },
        { "lon",        OSM_KEY_LON },
        { "version",    OSM_KEY_VERSION },
        { "uid",        OSM_KEY_UID },
        { "id",         OSM_KEY_ID },
    };

    unordered_map<string, size_t> relationKeyMap // TODO split into multiple maps for performance
    {
        { "node",       OSM_TYPE_NODE },
        { "way",        OSM_TYPE_WAY },

        { "inner",      OSM_ROLE_INNER },
        { "outer",      OSM_ROLE_OUTER },
    };

    struct OSM_Base
    {
        uint64_t uid;

        // TODO There might be a lot of repeating of the below. Good to hash to compress.
        uint64_t changeset;
        uint8_t  version;
        string   user;
        string   timestamp; // TODO convert to binary structure.

        unordered_map<string, string> keyValues;
    };

    struct Node : OSM_Base
    {
        dvec2 pos;
    };

    struct Way : OSM_Base
    {
        Way() : used(false) {}

        vector<const Node *> nodes;

        bool used;

        unique_ptr<Geometry> geom;
    };

    struct Member
    {
        string   role;
        uint64_t ref;
        string   type;
    };

    struct Relation : OSM_Base
    {
        vector<const Member *> members;
    };

    typedef unordered_map<uint64_t, Node     *> Nodes;
    typedef unordered_map<uint64_t, Way      *> Ways;
    typedef unordered_map<uint64_t, Relation *> Relations;

    Nodes     nodes;
    Ways      ways;
    Relations relations;

    OSM_Base * curr         = NULL;
    Node     * currNode     = NULL;
    Way      * currWay      = NULL;
    Relation * currRelation = NULL;

    inline size_t getKey(const string & key)
    {
        const auto i = keyMap.find(key);

        if(i == keyMap.end()) { return 0 ;}

        return i->second;
    }

    inline size_t getRelationKey(const string & key)
    {
        const auto i = relationKeyMap.find(key);

        if(i == relationKeyMap.end()) { return 0 ;}

        return i->second;
    }

    static void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts)
    {
        switch(getKey(name))
        {
            case OSM_KEY_RELATION:
            
                curr = currRelation = new Relation();

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_USER:      currRelation->user = atts[i + 1]; break;
                        case OSM_KEY_UID:       currRelation->uid =  stoull(atts[i + 1]); break;
                        case OSM_KEY_TIMESTAMP: currRelation->timestamp = atts[i + 1]; break;
                        case OSM_KEY_CHANGESET: currRelation->changeset = stoull(atts[i + 1]); break;
                        case OSM_KEY_VERSION:   currRelation->version = atoi(atts[i + 1]); break;

                        case OSM_KEY_ID:        relations[stoull(atts[i + 1])] = currRelation; break;

                        default: WARN_UNKNOWN_TAG
                    }
                }

                break;

            case OSM_KEY_MEMBER:
            {
                Member * member = new Member();

                currRelation->members.push_back(member);

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_ROLE: member->role = atts[i + 1]; break;
                        case OSM_KEY_REF:  member->ref = stoull(atts[i + 1]); break;
                        case OSM_KEY_TYPE: member->type = atts[i + 1]; break;

                        default: WARN_UNKNOWN_TAG
                    }
                }

                break;
            } 

            case OSM_KEY_TAG:
            {
                string key;
                string value;

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_K: key   = atts[i + 1];  break;
                        case OSM_KEY_V: value = atts[i + 1]; break;
                    }
                }

                curr->keyValues[key] = value;

                break;
            }

            case OSM_KEY_META:
            
                break;

            case OSM_KEY_NODE:

                curr = currNode = new Node;

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_CHANGESET: currNode->changeset  = stoull(atts[i + 1]); break;
                        case OSM_KEY_TIMESTAMP: currNode->timestamp  = atts[i + 1]; break;
                        case OSM_KEY_USER:      currNode->user       = atts[i + 1]; break;
                        case OSM_KEY_LAT:       currNode->pos.y      = atof(atts[i + 1]); break;
                        case OSM_KEY_LON:       currNode->pos.x      = atof(atts[i + 1]); break;
                        case OSM_KEY_VERSION:   currNode->version    = atoi(atts[i + 1]); break;
                        case OSM_KEY_UID:       currNode->uid        = stoull(atts[i + 1]); break;

                        case OSM_KEY_ID:        nodes[stoull(atts[i + 1])] = currNode; break;

                        default: WARN_UNKNOWN_TAG
                    }
                }

                break;

            case OSM_KEY_WAY:

                curr = currWay = new Way;

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_USER:      currWay->user = atts[i + 1]; break;
                        case OSM_KEY_UID:       currWay->uid =  stoull(atts[i + 1]); break;
                        case OSM_KEY_TIMESTAMP: currWay->timestamp = atts[i + 1]; break;
                        case OSM_KEY_CHANGESET: currWay->changeset = stoull(atts[i + 1]); break;
                        case OSM_KEY_VERSION:   currWay->version = atoi(atts[i + 1]); break;
                        case OSM_KEY_ID:        ways[stoull(atts[i + 1])] = currWay; break;

                        default: WARN_UNKNOWN_TAG
                    }
                }

                break;

            case OSM_KEY_ND:

                for(size_t i = 0; atts[i] != NULL; i += 2)
                {
                    switch(getKey(atts[i]))
                    {
                        case OSM_KEY_REF:
                        {
                            Nodes::const_iterator n = nodes.find(stoull(atts[i + 1]));

                            if(n == nodes.end()) // TODO assume data is correct?
                            {
                                dmess("Parse werror!");

                                break;
                            }

                            currWay->nodes.push_back(n->second);

                            break;
                        }
                    }
                }
                break;

            case OSM_KEY_OSM: break;

            default: WARN_UNKNOWN_TAG
        }
    }

    static void XMLCALL endElement(void *userData, const XML_Char *name) { }

    CoordinateArraySequence * getCoordinateSequence( const vector<const Node *>                    & nodes,
                                                     unordered_map<string, unordered_set<string> > & attributes)
    {
        //dmess("nodes.size() " << nodes.size());

        vector<Coordinate> * coords = new vector<Coordinate>(nodes.size());

        for(size_t i = 0; i < nodes.size(); ++i)
        {
            const Node * n = nodes[i];

            //(*coords)[i] = Coordinate(n->lat, n->lon);
            (*coords)[i] = Coordinate(n->pos.x, n->pos.y);

            for(const unordered_map<string, string>::value_type & i : n->keyValues)
            {
                attributes[i.first].insert(i.second);
            }
        }

        return new CoordinateArraySequence(coords, 2);
    }
}

unordered_set<string> _types;
unordered_set<string> _rolls;

bool OSM_Importer::import(  const string   & fileName,
                            vector<AttributedGeometry> & polygons,
                            vector<string> & serializedLineStrings,
                            vector<string> & serializedPoints)
{
    char buf[BUFSIZ];
    XML_Parser parser = XML_ParserCreate(NULL);
    int done;
    int depth = 0;
    
    XML_SetUserData(parser, &depth);
    XML_SetElementHandler(parser, startElement, endElement);

    FILE * fp = fopen(fileName.c_str(), "r");

    do
    {
        size_t len = fread(buf, 1, sizeof(buf), fp);
        done = len < sizeof(buf);
        if (XML_Parse(parser, buf, (int)len, done) == XML_STATUS_ERROR)
        {
            fprintf(stderr,
            "%" XML_FMT_STR " at line %" XML_FMT_INT_MOD "u\n",
            XML_ErrorString(XML_GetErrorCode(parser)),
            XML_GetCurrentLineNumber(parser));
            return false;
        }
    } while (!done);

    XML_ParserFree(parser);
    
    fclose(fp);

    dmess("nodes " << nodes.size());
    dmess("ways " << ways.size());
    dmess("relations " << relations.size());

    // TODO wonder how many of the nodes are duplicated? 

    size_t numPoints = 0;
    size_t numLineStrings = 0;
    size_t numPolygons = 0;

    for(const Ways::value_type & i : ways)
    {
        const uint64_t   id  = i.first;
        Way      * way = i.second;

        const vector<const Node *> & nodes = way->nodes;

        unordered_map<string, unordered_set<string> > attributeMap; // TODO, do not need this here

        CoordinateArraySequence * coords = getCoordinateSequence(nodes, attributeMap); // TODO grow layer AABBB

        //Attributes * attrs = new Attributes(attributeMap);

        const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

        if((*nodes.begin())->pos != (*nodes.rbegin())->pos)
        {
            // TODO, implement!
        }
        else
        {
            if(coords->getSize() == 3) 
            {
                coords->add(Coordinate(coords->getAt(0)));

                continue;
            }
            
            LinearRing * externalRing = factory->createLinearRing(coords);

            way->geom = unique_ptr<Geometry>(factory->createPolygon(externalRing, NULL));
        }
    }

    /*
    struct Member
    {
        string   role;
        uint64_t ref;
        string   type; // TODO Needed? Seems ref implies.
    };
    */
    for(const Relations::value_type & i : relations)
    {
        vector<Way *> outers;
        vector<Way *> inners;

        for(const Member * member : i.second->members)
        {
            _types.insert(member->type);
            _rolls.insert(member->role);

            switch(getRelationKey(member->type))
            {
                case OSM_TYPE_NODE:
                    break;

                case OSM_TYPE_WAY:
                {  
                    Way * way = ways[member->ref];

                    switch(getRelationKey(member->role))
                    {
                        case OSM_ROLE_INNER:

                            inners.push_back(way); // Will not mark as used so that it will be exported later on.

                            break;

                        default:

                            way->used = true;

                            outers.push_back(way);

                            //if(way->keyValues.size()) { dmess("way->keyValues.size() " << way->keyValues.size()) ;}

                            break; // TODO handle the others
                    }
                    
                    break;
                }
                    

                default:

                    dmess("Implement me! [" << member->type << "]");

                    break;
            }
        }

        //dmess("outers " << outers.size());
        //dmess("inners " << inners.size());

        for(auto inner : inners)
        {
            if(!inner->geom)
            {
                //dmess("!inner");

                continue;
            }

            for(auto outer : outers)
            {
                if(!outer->geom)
                {
                    //dmess("!outer");

                    continue;
                }

                if(!contains(outer->geom, inner->geom)) { continue ;}

                if(!subtract(outer->geom, inner->geom)) { dmess("Diff error!") ;}
            }
        }
    }

    size_t unusedWays = 0;

    for(const Ways::value_type & i : ways)
    {
        const uint64_t   id  = i.first;
        const Way      * way = i.second;

        //if(way->used) { continue ;}

        ++unusedWays;

        //dmess("id " << id);

        /*
        uint64_t uid;

        // TODO There might be a lot of repeating of the below. Good to hash to compress.
        uint64_t changeset;
        uint8_t  version;
        string   user;
        string   timestamp; // TODO convert to binary structure.

        unordered_map<string, string> keyValues;
        */

        const vector<const Node *> & nodes = way->nodes;

        unordered_map<string, unordered_set<string> > attributeMap;

        CoordinateArraySequence * coords = getCoordinateSequence(nodes, attributeMap); // TODO grow layer AABBB

        Attributes * attrs = new Attributes(attributeMap);

        const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

        if((*nodes.begin())->pos != (*nodes.rbegin())->pos)
        {
            ++numLineStrings;
        }
        else
        {
            if(coords->getSize() == 3) 
            {
                coords->add(Coordinate(coords->getAt(0)));

                continue;
            }
            
            LinearRing * externalRing = factory->createLinearRing(coords);

            polygons.push_back(AttributedGeometry(attrs, factory->createPolygon(externalRing, NULL)));
            
            ++numPolygons;
        }
    }

    dmess("unusedWays " << unusedWays);


    dmess("points " << numPoints);
    dmess("lineStrings " << numLineStrings);
    dmess("polygons " << numPolygons);

    /*
    dmess("types: "); for(const string & i : _types) { dmess("   " << i) ;}
    dmess("Rolls"); for(const string & i : _rolls) { dmess("   " << i) ;}
    //*/

    return true;
}