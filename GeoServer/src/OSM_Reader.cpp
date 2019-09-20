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
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/GeometryFactory.h>
#include <webAsmPlay/geom/GeosUtil.h>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Relation.h>
#include <geoServer/DataModelOSM.h>
#include <geoServer/OSM_Reader.h>

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
using namespace dataModelOSM;

namespace
{
    OSM_Nodes     nodes;
    OSM_Ways      ways;
    OSM_Relations relations;

    OSM_Base     * curr         = nullptr;
    OSM_Node     * currNode     = nullptr;
    OSM_Way      * currWay      = nullptr;
    OSM_Relation * currRelation = nullptr;

    double boundsMinX = 0;
    double boundsMaxX = 0;
    double boundsMinY = 0;
    double boundsMaxY = 0;

    inline OSM_Node * getNode(const uint64_t ID)
    {
        const auto i = nodes.find(ID);

        if(i == nodes.end()) { return nullptr ;}

		++i->second->m_useCount;

        return i->second;
    }

    inline OSM_Way * getWay(const uint64_t ID)
    {
        const auto i = ways.find(ID);

        if(i == ways.end()) { return nullptr ;}

        return i->second;
    }

    inline OSM_Relation * getRelation(const uint64_t ID)
    {
        const auto i = relations.find(ID);

        if(i == relations.end()) { return nullptr ;}

        return i->second;
    }
}

MapData OSM_Reader::import(const string & fileName)
{
    MapData ret;

    boundsMinX = 0;
    boundsMaxX = 0;
    boundsMinY = 0;
    boundsMaxY = 0;

    char buf[BUFSIZ];

    XML_Parser parser = XML_ParserCreate(nullptr);
    
    int done;
    int depth = 0;

    XML_SetUserData(parser, &depth);
    XML_SetElementHandler(parser, startElement, endElement);

    FILE * fp = fopen(fileName.c_str(), "r");

	if (!fp) { dmessError("Error! Could not open: " << fileName); }

    do
    {
        size_t len = fread(buf, 1, sizeof(buf), fp);
        
        done = len < sizeof(buf);

        if (XML_Parse(parser, buf, (int)len, done) == XML_STATUS_ERROR)
        {
            fprintf(stderr, "%" XML_FMT_STR " at line %" XML_FMT_INT_MOD "u\n",
            XML_ErrorString(XML_GetErrorCode(parser)),
            XML_GetCurrentLineNumber(parser));

            return ret;
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

    for(const OSM_Ways::value_type & i : ways)
    {
        const uint64_t   id  = i.first;
        OSM_Way        * way = i.second;

        const vector<const OSM_Node *> & nodes = way->m_nodes;

        CoordinateArraySequence * coords = getGeosCoordinateSequence(nodes); // TODO grow layer AABBB

        const GeometryFactory * factory = GeometryFactory::getDefaultInstance();

        if((*nodes.begin())->m_pos != (*nodes.rbegin())->m_pos)
        {
            way->m_geom = unique_ptr<Geometry>(factory->createLineString(coords));
        }
        else
        {
            if(coords->getSize() < 3) { continue ;}

            else if(coords->getSize() == 3) 
            {
                coords->add(Coordinate(coords->getAt(0)));

                continue;
            }
            
            LinearRing * externalRing = factory->createLinearRing(coords);

            way->m_geom = unique_ptr<Geometry>(factory->createPolygon(externalRing, nullptr));
        }
    }

    size_t numInvalidWays      = 0;
    size_t numInvalidPoints    = 0;
    size_t geomOperationErrors = 0;

    Geometry::Ptr bounds = makeBox({boundsMinX, boundsMinY}, {boundsMaxX, boundsMaxY});

    for(const OSM_Relations::value_type & i : relations)
    {
        vector<OSM_Way *> outers;
        vector<OSM_Way *> inners;

        for(const OSM_Member * member : i.second->m_members)
        {
            switch(getRelationKey(member->m_type))
            {
                case OSM_TYPE_NODE:
                {
                    OSM_Node * node = getNode(member->m_ref);

                    if(!node)
                    {
                        ++numInvalidPoints;

                        break;
                    }

                    // TODO, do we need to clip to the bounds?
                    ret.m_geometry.push_back(AttributedGeometry(node->m_attrs.release(), __(node->m_pos)));

                    break;
                }
                case OSM_TYPE_WAY:
                {  
                    OSM_Way * way = getWay(member->m_ref);

                    if(!way)
                    {
                        ++numInvalidWays;

                        continue;
                    }

                    switch(getRelationKey(member->m_role))
                    {
                        case OSM_ROLE_INNER:

                            inners.push_back(way); // Will not mark as used so that it will be exported later on.

                            break;

                        default:

                            way->used = true;

                            outers.push_back(way);

                            way->m_relations.push_back(i.second);

                            break;
                    }
                    
                    break;
                }
                    
                default:

                    //dmess("Implement me! [" << member->m_type << "]");

                    break;
            }
        }

        for(auto inner : inners)
        {
            if(!inner->m_geom) { continue ;}

            for(auto outer : outers)
            {
                if(!outer->m_geom) { continue ;}

                try
                {
                    if(!contains(outer->m_geom, inner->m_geom)) { continue ;}

                    if(!subtract(outer->m_geom, inner->m_geom)) { ++geomOperationErrors ;}
                }
                catch(...) { ++geomOperationErrors ;}
            }
        }
    }

    for(const auto & i : relations)
    {
        OSM_Relation * relation = i.second;

        Relation * _realtion = new Relation();
    }

    for(const auto & i : ways)
    {
        const uint64_t   id  = i.first;
        OSM_Way        * way = i.second;

		try
		{
			if (way && way->m_geom)
			{
				Geometry* geom = bounds->intersection(way->m_geom.get());
				//Geometry* geom = way->m_geom.release(); 

				if (!geom)
				{
					dmess("!geom");

					continue;
				}

				Attributes* attrs = way->m_attrs.release();

				ret.m_geometry.push_back(AttributedGeometry(attrs, geom)); // TODO not safe!
			}
			else
			{
				dmess("!way->geom");
			}
		}
		catch (...)
		{
			dmess("Here!");
		}
    }

    dmess("Nodes " << nodes.size());

    size_t a = 0;
    size_t b = 0;

    for(const auto & i : nodes)
    {
        if(!i.second->m_relations.size()) { ++a ;}
        else { ++b ;}

		if(!i.second->m_useCount)
		{
			// Most of these seems to be trees
			// TODO, do we need to clip to the bounds?
            ret.m_geometry.push_back(AttributedGeometry(i.second->m_attrs.release(), __(i.second->m_pos)));
		}
    }

    dmess("a " << a << " b " << b);

    dmess("numInvalidWays " << numInvalidWays);
    dmess("numInvalidPoints " << numInvalidPoints);
    dmess("geomOperationErrors " << geomOperationErrors);

    ret.m_boundsMinY = boundsMinY;
    ret.m_boundsMinX = boundsMinX;
    ret.m_boundsMaxY = boundsMaxY;
    ret.m_boundsMaxX = boundsMaxX;

    return ret;
}

void OSM_Reader::startElement(void *userData, const char *name, const char **atts)
{
    switch(getKey(name))
    {
        case OSM_KEY_RELATION:  handleRelation (atts); break;
        case OSM_KEY_MEMBER:    handleMember   (atts); break;
        case OSM_KEY_TAG:       handleTag      (atts); break;
        case OSM_KEY_NODE:      handleNode     (atts); break;
        case OSM_KEY_WAY:       handleWay      (atts); break;
        case OSM_KEY_ND:        handleND       (atts); break;
        case OSM_KEY_BOUNDS:    handleBounds   (atts); break;

        // Unused
        case OSM_KEY_META: 
        case OSM_KEY_OSM:  
        case OSM_KEY_NOTE: 
        case OSM_KEY_REMARK: break;

        default: dmess("Unknown tag: " << name);
    }
}

void OSM_Reader::handleRelation(const char **atts)
{
    curr = currRelation = new OSM_Relation();

    Attributes * attrs = curr->m_attrs.get();

    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_USER:      attrs->m_strings["userOSM"]      =        atts[i + 1];  break;
            case OSM_KEY_TIMESTAMP: attrs->m_strings["timestampOSM"] =        atts[i + 1];  break;
            case OSM_KEY_VERSION:   attrs->m_uints32["versionOSM"]   =   atoi(atts[i + 1]); break;
            case OSM_KEY_CHANGESET: attrs->m_uints64["changesetOSM"] = stoull(atts[i + 1]); break;
            case OSM_KEY_UID:       attrs->m_uints32["uidOSM"]       = stoull(atts[i + 1]); break;

            case OSM_KEY_ID:
            {
                const uint64_t ID = stoull(atts[i + 1]);

                relations[ID] = currRelation;

                attrs->m_uints64["ID_OSM"] = ID;

                break;
            }

            default: dmess("Unknown tag: " << atts[i]);
        }
    }
}

void OSM_Reader::handleMember(const char **atts)
{
    OSM_Member * member = new OSM_Member();

    currRelation->m_members.push_back(member);

    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_ROLE: member->m_role =        atts[i + 1];	 break;
            case OSM_KEY_REF:  member->m_ref  = stoull(atts[i + 1]); break;
            case OSM_KEY_TYPE: member->m_type =        atts[i + 1];	 break;

            default: dmess("Unknown tag: " << atts[i]);
        }
    }
}

void OSM_Reader::handleTag(const char **atts)
{
    string key;
    string value;

    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_K: key   = atts[i + 1]; break;
            case OSM_KEY_V: value = atts[i + 1]; break;

            default: dmess("Unknown tag: " << atts[i]);
        }
    }

    curr->m_attrs->m_strings[key] = value;
}

void OSM_Reader::handleNode(const char **atts)
{
    curr = currNode = new OSM_Node;

    Attributes * attrs = curr->m_attrs.get();

    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_CHANGESET: attrs   ->m_uints64["changesetOSM"] = stoull(atts[i + 1]); break;
            case OSM_KEY_TIMESTAMP: attrs   ->m_strings["timestampOSM"] =        atts[i + 1];  break;
            case OSM_KEY_USER:      attrs   ->m_strings["userOSM"]      =        atts[i + 1];  break;
            case OSM_KEY_VERSION:   attrs   ->m_uints32["versionOSM"]   =   atoi(atts[i + 1]); break;
            case OSM_KEY_UID:       attrs   ->m_uints32["uidOSM"]       = stoull(atts[i + 1]); break;
            case OSM_KEY_LAT:       currNode->m_pos.y                   =   atof(atts[i + 1]); break;
            case OSM_KEY_LON:       currNode->m_pos.x                   =   atof(atts[i + 1]); break;

            case OSM_KEY_ID:
            {
                const uint64_t ID = stoull(atts[i + 1]);

                nodes[ID] = currNode;

                attrs->m_uints64["ID_OSM"] = ID;
                
                break;
            }

            default: dmess("Unknown tag: " << atts[i]);
        }
    }
}

void OSM_Reader::handleWay(const char **atts)
{
    curr = currWay = new OSM_Way;

    Attributes * attrs = curr->m_attrs.get();

    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_USER:      attrs->m_strings["userOSM"]      =        atts[i + 1];  break;
            case OSM_KEY_UID:       attrs->m_uints32["uidOSM"]       = stoull(atts[i + 1]); break;
            case OSM_KEY_TIMESTAMP: attrs->m_strings["timestampOSM"] =        atts[i + 1];  break;
            case OSM_KEY_CHANGESET: attrs->m_uints64["changesetOSM"] = stoull(atts[i + 1]); break;
            case OSM_KEY_VERSION:   attrs->m_uints32["versionOSM"]   =   atoi(atts[i + 1]); break;

            case OSM_KEY_ID:
            {   
                const uint64_t ID = stoull(atts[i + 1]);

                ways[ID] = currWay;

                attrs->m_uints64["ID_OSM"] = ID;
                
                break;
            }

            default: dmess("Unknown tag: " << atts[i]);
        }
    }
}

void OSM_Reader::handleND(const char **atts)
{
    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
            case OSM_KEY_REF:
            {
                OSM_Node * node = getNode(stoull(atts[i + 1]));

                if(node) { currWay->m_nodes.push_back(node) ;}

                break;
            }

            default: dmess("Unknown tag: " << atts[i]);
        }
    }
}

void OSM_Reader::handleBounds(const char **atts)
{
    for(size_t i = 0; atts[i] != nullptr; i += 2)
    {
        switch(getKey(atts[i]))
        {
        case OSM_KEY_MIN_LAT: boundsMinY = atof(atts[i + 1]); break;
        case OSM_KEY_MIN_LON: boundsMinX = atof(atts[i + 1]); break;
        case OSM_KEY_MAX_LAT: boundsMaxY = atof(atts[i + 1]); break;
        case OSM_KEY_MAX_LON: boundsMaxX = atof(atts[i + 1]); break;

        default: dmess("Unknown tag: " << atts[i]);
        }
    }
}
