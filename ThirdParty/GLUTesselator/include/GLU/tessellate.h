
#include <cstdint>

typedef struct Vertex {
    double pt[3];
    int index;
    struct Vertex *prev;
} Vertex;

void tessellate(double **verts,
                uint32_t *nverts,
                uint32_t **tris,
                uint32_t *ntris,
                const double **contoursbegin, 
                const double **contoursend);
