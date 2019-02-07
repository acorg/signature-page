#include <iostream>
#include <set>

#include "acmacs-base/argv.hh"
#include "tree.hh"
#include "tree-export.hh"

// ----------------------------------------------------------------------

static std::set<std::string> get_nodes(std::string_view filename);

using namespace acmacs::argv;
struct Options : public argv
{
    Options(int a_argc, const char* const a_argv[], on_error on_err = on_error::exit) : argv() { parse(a_argc, a_argv, on_err); }

    argument<str> tree1{*this, arg_name{"tree-1"}, mandatory};
    argument<str> tree2{*this, arg_name{"tree-2"}, mandatory};
};

int main(int argc, const char* argv[])
{
    using namespace std::string_literals;
    try {
        Options opt(argc, argv);

        const auto nodes1 = get_nodes(opt.tree1);
        const auto nodes2 = get_nodes(opt.tree2);

        for (const auto& node1 : nodes1) {
            if (nodes2.find(node1) == nodes2.end())
                std::cout << "< " << node1 << '\n';
        }

        for (const auto& node2 : nodes2) {
            if (nodes1.find(node2) == nodes1.end())
                std::cout << "> " << node2 << '\n';
        }

        return 0;
    }
    catch (std::exception& err) {
        std::cerr << "ERROR: " << err.what() << '\n';
        return 1;
    }
}

// ----------------------------------------------------------------------

std::set<std::string> get_nodes(std::string_view filename)
{
    const Tree tree = tree::tree_import(filename);
    std::set<std::string> nodes;
    tree::iterate_leaf(tree, [&nodes](const Node& node) { nodes.insert(node.seq_id); });
    return nodes;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
