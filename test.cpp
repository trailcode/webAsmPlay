
#include <iostream>
//#include <emscripten/bind.h>
//#include <emscripten/emscripten.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>
#include "./GLUTesselator/include/GLU/tessellate.h"
//#include <GLU/tessellate.h>

using namespace std;
//using namespace emscripten;
using namespace geos::geom;


const GeometryFactory * geomFact;

extern "C" {
geos::geom::Polygon *MakeBox(double xmin, double ymin, double xmax, double ymax) {
    cout << "boxa " << endl;

    geomFact = GeometryFactory::getDefaultInstance();
    cout << "geomFact->getCoordinateSequenceFactory() " << geomFact->getCoordinateSequenceFactory() << endl;
    geos::geom::CoordinateSequence *temp = geomFact->getCoordinateSequenceFactory()->create((std::size_t) 0, 0);
    cout << "temp " << temp << endl;

    temp->add(geos::geom::Coordinate(xmin, ymin));
    temp->add(geos::geom::Coordinate(xmin, ymax));
    temp->add(geos::geom::Coordinate(xmax, ymax));
    temp->add(geos::geom::Coordinate(xmax, ymin));
    //Must close the linear ring or we will get an error:
    //"Points of LinearRing do not form a closed linestring"
    temp->add(geos::geom::Coordinate(xmin, ymin));

    geos::geom::LinearRing *shell = geomFact->createLinearRing(temp);

    //NULL in this case could instead be a collection of one or more holes
    //in the interior of the polygon
    return geomFact->createPolygon(shell, NULL);
}
}

void run_example(const double vertices_array[],
                 const double *contours_array[],
                 int contours_size)
{
    double *coordinates_out;
    int *tris_out;
    int nverts, ntris, i;

    const double *p = vertices_array;
    /* const double **contours = contours_array; */

    tessellate(&coordinates_out, &nverts,
               &tris_out, &ntris,
               contours_array, contours_array + contours_size);

    cout << "nverts " << nverts << endl;

    for (i=0; i<2 * nverts; ++i) {
        fprintf(stdout, "%g ", coordinates_out[i]);
    }
    fprintf(stdout, "\n");
    for (i=0; i<3 * ntris; ++i) {
        fprintf(stdout, "%d ", tris_out[i]);
    }
    fprintf(stdout, "\n");
    free(coordinates_out);
    if (tris_out)
        free(tris_out);
}

/*

extern "C" {
    int main() {
        std::cout << "Hello, World!" << std::endl;

        //Coordinate * c = new Coordinate(1,2);

        //cout << " c " << c->x << " y " << c->y << endl;

        geomFact = GeometryFactory::getDefaultInstance();

        cout << "geomFact " << geomFact << endl;

        //LinearRing * ring = geomFact->createLinearRing();

        Polygon * p = MakeBox(-1,-1,1,1);

        cout << " p " << p->getArea() << endl;


        double a1[] = { 0, 0, 1, 5, 2, 0, -1, 3, 3, 3 };
        const double *c1[] = {a1, a1+10};
        int s1 = 2;
        run_example(a1, c1, 2);

        double a2[] = { 0, 0, 3, 0, 3, 3, 0, 3,
                        1, 1, 2, 1, 2, 2, 1, 2 };
        const double *c2[] = {a2, a2+8, a2+16};
        int s2 = 3;
        run_example(a2, c2, s2);

        double a3[] = { 441, 0, 326, 0, 326, 889, 12, 889, 12, 992, 755, 992, 755, 889, 441, 889 };
        const double *c3[] = { a3, a3+16 };
        int s3 = 2;
        run_example(a3, c3, s3);


        return 0;
    }
}
*/

/*
EMSCRIPTEN_BINDINGS(module) {

        class_<MyClass>("MyClass")
                .constructor<int, std::string>()
                .function("incrementX", &MyClass::incrementX)
                .property("x", &MyClass::getX, &MyClass::setX)
                .class_function("getStringFromInstance", &MyClass::getStringFromInstance)
        ;

        using emscripten::function;
        //function("doIta", &doIta);
        //function("doItb", &doItb);
}
 */

