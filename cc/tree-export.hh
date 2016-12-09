#pragma once

#include <string>
#include <stdexcept>

// ----------------------------------------------------------------------

class Tree;

void tree_export(std::string aFilename, const Tree& aTree, size_t aIndent);
void tree_import(std::string aFilename, Tree& aTree);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
