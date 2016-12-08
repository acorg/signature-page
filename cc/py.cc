#include "acmacs-base/pybind11.hh"
#include "tree.hh"
#include "tree-import.hh"

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(signature_page_cc)
{
    py::module m("signature_page_cc", "Tree and signature page generator plugin");

    py::class_<Node>(m, "Node")
            .def(py::init<>())
            ;

    py::class_<Tree, Node>(m, "Tree")
            .def(py::init<>())
            ;

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
