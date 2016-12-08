#include "acmacs-base/pybind11.hh"

// ----------------------------------------------------------------------

PYBIND11_PLUGIN(signature_page_cc)
{
    py::module m("signature_page_cc", "Tree and signature page generator plugin");


    return m.ptr();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
