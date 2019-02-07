#pragma once

#include <string>
#include <stdexcept>
#include <memory>

#include "signature-page/tree.hh"

// ----------------------------------------------------------------------

namespace acmacs::chart { class Chart; }

namespace tree
{
    void export_to_json(std::string_view aFilename, const Tree& aTree, size_t aIndent);
    void export_to_newick(std::string_view aFilename, const Tree& aTree, size_t aIndent);
    void tree_import(std::string_view aFilename, Tree& aTree);
    Tree tree_import(std::string_view aFilename);
    Tree tree_import(std::string_view aFilename, std::shared_ptr<acmacs::chart::Chart> chart, Tree::LadderizeMethod aLadderizeMethod = Tree::LadderizeMethod::NumberOfLeaves);
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
