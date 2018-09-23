#ifdef BUILD_PYTHON_BINDINGS

#include <boost/python.hpp>
#include <tceGeom/vec2.h>

using namespace boost::python;
using namespace tce::geom;

namespace
{
	template<typename T>
	void doExport(const char * name)
	{
		Vec2<T> a; // @@@ TODO use template instantiation!
		
		class_<Vec2<T> >(name, "this is a 2D vector", init<>(args("self")))
		
				        	.def(init<T,T>(args("self", "x", "y")))
				        	
				        	.def("dot", 					&Vec2<T>::dot, 						args("self", "v"))
				        	.def("length", 					&Vec2<T>::length)
				        	.def("lengthSquared", 			&Vec2<T>::lengthSquared)
				        	.def("normalize", 				&Vec2<T>::normalize)
				        	.def("parallelComponent", 		&Vec2<T>::parallelComponent, 		args("self", "v"))
				        	.def("perpendicularComponent", 	&Vec2<T>::perpendicularComponent, 	args("self", "v"))
				        	.def("truncateLength", 			&Vec2<T>::truncateLength, 			args("self", "maxLength"))
				        	.def("printVec", 				&Vec2<T>::print)
				        	;
	}
}

void pyExport_Vec2_int()
{
	doExport<int>("Vec2i");	       
}

void pyExport_Vec2_float()
{
	doExport<float>("Vec2f");
}

void pyExport_Vec2_double()
{
	doExport<double>("Vec2d");
}

#endif // BUILD_PYTHON_BINDINGS
