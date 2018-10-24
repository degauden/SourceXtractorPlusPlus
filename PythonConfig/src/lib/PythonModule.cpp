/* 
 * @file PythonModule.cpp
 * @author Nikolaos Apostolakos <nikoapos@gmail.com>
 */

#include <boost/python.hpp>
#include <PythonConfig/ObjectInfo.h>
#include <PythonConfig/PyMeasurementImage.h>
#include <PythonConfig/PyAperture.h>


namespace bp = boost::python;

namespace SExtractor {

BOOST_PYTHON_MODULE(libPythonConfig) {
  
  bp::class_<ObjectInfo>("ObjectInfo", bp::init<SourceInterface&>())
      .def("get_alpha_coord", &ObjectInfo::getAlphaCoord)
      .def("get_delta_coord", &ObjectInfo::getDeltaCoord)
      .def("get_iso_flux", &ObjectInfo::getIsoFlux)
      .def("get_radius_world", &ObjectInfo::getRadiusWorld);
  
  bp::class_<PyMeasurementImage>("MeasurementImage", bp::init<std::string, std::string, std::string>())
      .def_readonly("id", &PyMeasurementImage::id)
      .def_readonly("file", &PyMeasurementImage::file)
      .def_readwrite("gain", &PyMeasurementImage::gain)
      .def_readwrite("saturation", &PyMeasurementImage::saturation)
      .def_readwrite("flux_scale", &PyMeasurementImage::flux_scale)
      .def_readonly("psf_file", &PyMeasurementImage::psf_file)
      .def_readonly("weight_file", &PyMeasurementImage::weight_file)
      .def_readwrite("weight_type", &PyMeasurementImage::weight_type)
      .def_readwrite("weight_absolute", &PyMeasurementImage::weight_absolute)
      .def_readwrite("weight_scaling", &PyMeasurementImage::weight_scaling)
      .def_readwrite("has_weight_threshold", &PyMeasurementImage::has_weight_threshold)
      .def_readwrite("weight_threshold", &PyMeasurementImage::weight_threshold);

  bp::class_<PyColumn>("Column", bp::init<>())
    .def_readonly("id", &PyColumn::id);

  bp::class_<PyAperture, bp::bases<PyColumn>>("Aperture", bp::init<bp::list>())
    .def_readonly("apertures", &PyAperture::apertures)
    .def("__str__", &PyAperture::toString)
    .def("__repr__", &PyAperture::toString);
}

} // namespace SExtractor
