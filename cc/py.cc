#include "acmacs-base/pybind11.hh"

#include "tree.hh"
#include "tree-export.hh"

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(signature_page_cc)
{
    py::module m("signature_page_cc", "Tree and signature page generator plugin");

    py::class_<Node>(m, "Node")
            .def(py::init<>())
            ;

    py::class_<Tree, Node>(m, "Tree")
            .def(py::init<>())
            .def("match_seqdb", &Tree::match_seqdb, py::arg("seqdb"))
            ;

    m.def("tree_import", &tree_import, py::arg("filename"), py::arg("tree"), py::doc("Imports tree from json file."));
    m.def("tree_export", &tree_export, py::arg("filename"), py::arg("tree"), py::arg("indent") = 1, py::doc("Exports tree into json file."));

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
