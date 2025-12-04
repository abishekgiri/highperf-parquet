#include "hpq/writer.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(hpq_py, m) {
  m.doc() = "High-Performance Parquet Writer";

  py::class_<hpq::ParquetWriter>(m, "ParquetWriter")
      .def(py::init<const std::string &>())
      .def("close", &hpq::ParquetWriter::Close);
}
