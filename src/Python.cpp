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
  \copyright 2019
*/

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/circular_buffer.hpp>
#include <webAsmPlay/Debug.h>
#include <webAsmPlay/Texture.h>
#include <webAsmPlay/ImageFeatures.h>
#include <webAsmPlay/Python.h>

using namespace std;
using namespace boost::python;

namespace p = boost::python;
namespace np = boost::python::numpy;

namespace
{
	class PythonStdIoRedirect
	{
	public:
		typedef boost::circular_buffer<std::string> ContainerType;

		void Write( std::string const& str )
		{
			if (m_outputs.capacity()<100) { m_outputs.resize(100) ;}

			m_outputs.push_back(str);
		}

		static std::string GetOutput()
		{
			std::stringstream ss;

			for(boost::circular_buffer<std::string>::const_iterator it=m_outputs.begin(); it!=m_outputs.end(); it++) { ss << *it ;}

			m_outputs.clear();

			return ss.str();
		}
	private:

		static ContainerType m_outputs; // must be static, otherwise output is missing
	};

	PythonStdIoRedirect::ContainerType PythonStdIoRedirect::m_outputs;

	PythonStdIoRedirect python_stdio_redirector;

	struct World
	{
		void set(std::string msg) { this->msg = msg; }
		std::string greet() { return msg; }
		std::string msg;
		np::ndarray getArray()
		{
			p::tuple shape = p::make_tuple(3, 3);
			np::dtype dtype = np::dtype::get_builtin<float>();
			np::ndarray a = np::zeros(shape, dtype);

			return a;
		}
	};
}

namespace
{
	struct TextureWrap : Texture, wrapper<Texture>
	{
		TextureWrap(const string & ID) : Texture(ID) {}

		string getDownloadURL() const { return this->get_override("getDownloadURL")() ;}
	};
}

BOOST_PYTHON_MODULE(webAsmPlay)
{
	PyEval_InitThreads();

	class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
		.def("getArray", &World::getArray)
    ;

	class_<TextureWrap, boost::noncopyable>("Texture", init<string>())
		.def("textureToNdArray", &Texture::textureToNdArray).staticmethod("textureToNdArray");
	;

	class_<ImageFeatures>("ImageFeatures")
		.def("add", &ImageFeatures::add)
		.def("addFeatures", &ImageFeatures::addFeatures).staticmethod("addFeatures")
	;
}

void Python::initPython()
{
	try
	{
		if (PyImport_AppendInittab("webAsmPlay", &PyInit_webAsmPlay) == -1) { dmessError("Failed to add webAsmPlay to the interpreter's builtin modules") ;}

		Py_Initialize();

		np::initialize();

		object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));

		object main_namespace = main_module.attr("__dict__");

		object cpp_module( (handle<>(PyImport_ImportModule("webAsmPlay"))) );

		main_namespace["webAsmPlay"] = cpp_module;

		main_namespace["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
															.def("write", &PythonStdIoRedirect::Write);

		boost::python::import("sys").attr("stderr") = python_stdio_redirector;
		boost::python::import("sys").attr("stdout") = python_stdio_redirector;

	} catch( error_already_set )
	{
		PyErr_Print();
	}

	dmess(Python::execute("from webAsmPlay import *"));
}

string Python::execute(const string & command)
{
	object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));

	object main_namespace = main_module.attr("__dict__");

	try
	{
		handle<>((PyRun_String( command.c_str(), Py_file_input, main_namespace.ptr(), main_namespace.ptr())));

	} catch( error_already_set )
	{
		PyErr_Print();
	}

	return python_stdio_redirector.GetOutput();
}

