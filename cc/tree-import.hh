#pragma once

#include <string>
#include <stdexcept>

// ----------------------------------------------------------------------

class Tree;

class Error : public std::runtime_error { public: using std::runtime_error::runtime_error; };

void read_tree(std::string buffer, Tree& aTree);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
