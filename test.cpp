
#include <iostream>
//#include <emscripten/bind.h>
//#include <emscripten/emscripten.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>


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

        return 0;
    }
}

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

