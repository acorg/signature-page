#include "acmacs-base/pybind11.hh"

#include "tree.hh"
#include "tree-export.hh"

// ----------------------------------------------------------------------

inline Tree::LadderizeMethod ladderize_type_decode(std::string ladderize_type)
{
    if (ladderize_type == "max-edge-length")
        return Tree::LadderizeMethod::MaxEdgeLength;
    if (ladderize_type == "number-of-leaves")
        return Tree::LadderizeMethod::NumberOfLeaves;
    throw std::runtime_error("Unrecognized ladderize type: " + ladderize_type);
}

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
            .def("ladderize", [](Tree& tree, std::string ladderize_type) { tree.ladderize(ladderize_type_decode(ladderize_type)); }, py::arg("ladderize_type"))
            ;

    m.def("tree_import", &tree_import, py::arg("filename"), py::arg("tree"), py::doc("Imports tree from json file."));
    m.def("tree_export", &tree_export, py::arg("filename"), py::arg("tree"), py::arg("indent") = 1, py::doc("Exports tree into json file."));

    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
