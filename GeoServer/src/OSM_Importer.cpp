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
#include <webAsmPlay/Debug.h>
#include <glm/vec2.hpp>
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
};

unordered_map<string, size_t> keyMap
{
    { "member", OSM_KEY_MEMBER },
    { "tag", OSM_KEY_TAG },
    { "meta", OSM_KEY_META },
    { "node", OSM_KEY_NODE },
    { "way", OSM_KEY_WAY },
    { "relation", OSM_KEY_RELATION },
    { "nd", OSM_KEY_ND },
    { "osm", OSM_KEY_OSM },

    { "role", OSM_KEY_ROLE },
    { "ref", OSM_KEY_REF },
    { "type", OSM_KEY_TYPE },
    { "v", OSM_KEY_V },
    { "k", OSM_KEY_K },
    { "osm_base", OSM_KEY_OSM_BASE },
    { "changeset", OSM_KEY_CHANGESET },
    { "timestamp", OSM_KEY_TIMESTAMP },
    { "user", OSM_KEY_USER },
    { "lat", OSM_KEY_LAT },
    { "lon", OSM_KEY_LON },
    { "version", OSM_KEY_VERSION },
    { "uid", OSM_KEY_UID },
    { "id", OSM_KEY_ID },
    { "generator", OSM_KEY_GENERATOR },
};

/*
c/OSM_Importer.cpp 141 [node: member
c/OSM_Importer.cpp 145 [  key: role
c/OSM_Importer.cpp 145 [  key: ref
c/OSM_Importer.cpp 145 [  key: type
c/OSM_Importer.cpp 141 [node: tag
c/OSM_Importer.cpp 145 [  key: v
c/OSM_Importer.cpp 145 [  key: k
c/OSM_Importer.cpp 141 [node: meta
c/OSM_Importer.cpp 145 [  key: osm_base
c/OSM_Importer.cpp 141 [node: node
c/OSM_Importer.cpp 145 [  key: changeset
c/OSM_Importer.cpp 145 [  key: timestamp
c/OSM_Importer.cpp 145 [  key: user
c/OSM_Importer.cpp 145 [  key: lat
c/OSM_Importer.cpp 145 [  key: version
c/OSM_Importer.cpp 145 [  key: uid
c/OSM_Importer.cpp 145 [  key: lon
c/OSM_Importer.cpp 145 [  key: id
c/OSM_Importer.cpp 141 [node: way
c/OSM_Importer.cpp 145 [  key: user
c/OSM_Importer.cpp 145 [  key: id
c/OSM_Importer.cpp 145 [  key: uid
c/OSM_Importer.cpp 145 [  key: timestamp
c/OSM_Importer.cpp 145 [  key: changeset
c/OSM_Importer.cpp 145 [  key: version
c/OSM_Importer.cpp 141 [node: relation
c/OSM_Importer.cpp 145 [  key: user
c/OSM_Importer.cpp 145 [  key: id
c/OSM_Importer.cpp 145 [  key: uid
c/OSM_Importer.cpp 145 [  key: timestamp
c/OSM_Importer.cpp 145 [  key: changeset
c/OSM_Importer.cpp 145 [  key: version
c/OSM_Importer.cpp 141 [node: nd
c/OSM_Importer.cpp 145 [  key: ref
c/OSM_Importer.cpp 141 [node: osm
c/OSM_Importer.cpp 145 [  key: generator
c/OSM_Importer.cpp 145 [  key: version
*/

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
    vector<Node *> nodes;
};

struct Member
{
    string   role;
    uint64_t ref;
    string   type; // TODO Needed? Seems ref implies.
};

struct Relation : OSM_Base
{
    vector<Member *> members;
};

//unordered_set<string> attrKeys;
unordered_map<string, unordered_set<string> > structure;

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

static void XMLCALL startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
    int i;
    int *depthPtr = (int *)userData;
    
    switch(keyMap[name])
    {
        case OSM_KEY_RELATION:
        
            curr = currRelation = new Relation();

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
                {
                    case OSM_KEY_USER:      currRelation->user = atts[i + 1]; break;
                    case OSM_KEY_UID:       currRelation->uid =  stoull(atts[i + 1]); break;
                    case OSM_KEY_TIMESTAMP: currRelation->timestamp = atts[i + 1]; break;
                    case OSM_KEY_CHANGESET: currRelation->changeset = stoull(atts[i + 1]); break;
                    case OSM_KEY_VERSION:   currRelation->version = atoi(atts[i + 1]); break;

                    case OSM_KEY_ID:        relations[stoull(atts[i + 1])] = currRelation; break;

                    default:

                            dmess("Implment: " << name);

                            for (i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;}
                }
            }

            break;

        case OSM_KEY_MEMBER:
        {
            Member * member = new Member();

            currRelation->members.push_back(member);

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
                {
                    case OSM_KEY_ROLE: member->role = atts[i + 1]; break;
                    case OSM_KEY_REF:  member->ref = stoull(atts[i + 1]); break;
                    case OSM_KEY_TYPE: member->type = atts[i + 1]; break;

                    default:

                        dmess("Implment: " << name);

                        for (i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;}
                }
            }

            break;
        } 

        case OSM_KEY_TAG:
        {
            string key;
            string value;

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
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

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
                {
                    case OSM_KEY_CHANGESET: currNode->changeset  = stoull(atts[i + 1]); break;
                    case OSM_KEY_TIMESTAMP: currNode->timestamp  = atts[i + 1]; break;
                    case OSM_KEY_USER:      currNode->user       = atts[i + 1]; break;
                    case OSM_KEY_LAT:       currNode->pos.x      = atof(atts[i + 1]); break;
                    case OSM_KEY_LON:       currNode->pos.y      = atof(atts[i + 1]); break;
                    case OSM_KEY_VERSION:   currNode->version    = atoi(atts[i + 1]); break;
                    case OSM_KEY_UID:       currNode->uid        = stoull(atts[i + 1]); break;

                    case OSM_KEY_ID:        nodes[stoull(atts[i + 1])] = currNode; break;

                    default:

                        dmess("Implment: " << name);

                        for (i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;}
                }
            }

            break;

        case OSM_KEY_WAY:

            curr = currWay = new Way;

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
                {
                    case OSM_KEY_USER:      currWay->user = atts[i + 1]; break;
                    case OSM_KEY_UID:       currWay->uid =  stoull(atts[i + 1]); break;
                    case OSM_KEY_TIMESTAMP: currWay->timestamp = atts[i + 1]; break;
                    case OSM_KEY_CHANGESET: currWay->changeset = stoull(atts[i + 1]); break;
                    case OSM_KEY_VERSION:   currWay->version = atoi(atts[i + 1]); break;
                    case OSM_KEY_ID:        ways[stoull(atts[i + 1])] = currWay; break;

                    default:

                        dmess("Implment: " << name);

                        for (i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;}
                }
            }

            break;

        case OSM_KEY_ND:

            for (i = 0; atts[i] != NULL; i += 2)
            {
                switch(keyMap[atts[i]])
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

        default:

            dmess("Implment: " << name);

            for (i = 0; atts[i] != NULL; i += 2) { dmess("key: " << atts[i] << " value: " << atts[i + 1]) ;}
    }

    *depthPtr += 1;
}

static void XMLCALL
endElement(void *userData, const XML_Char *name)
{
  int *depthPtr = (int *)userData;
  (void)name;

  *depthPtr -= 1;
}


bool OSM_Importer::import(  const string   & fileName,
                            vector<string> & serializedPolygons,
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

    return true;
}