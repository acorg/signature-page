#include <iostream>
#include <cmath>

#include "acmacs-base/argv.hh"
#include "acmacs-base/stream.hh"
#include "acmacs-chart-2/factory-import.hh"
#include "tree.hh"
#include "tree-export.hh"

static void print_tree_leaves(const Tree& tree, double step);
static void print_tree(const Tree& tree, double step);

// ----------------------------------------------------------------------

using namespace acmacs::argv;
struct Options : public argv
{
    Options(int a_argc, const char* const a_argv[], on_error on_err = on_error::exit) : argv() { parse(a_argc, a_argv, on_err); }

    option<str> seqdb{*this, "seqdb"};

    option<str>       chart{*this, "chart"};
    option<size_t>    max_leaf_offset{*this, "max-leaf-offset", dflt{80UL}};
    option<bool>      leaves_only{*this, "leaves-only"};

    option<bool>      verbose{*this, 'v', "verbose"};

    argument<str> tree_file{*this, arg_name{"tree.json[.xz]"}, mandatory};
};

int main(int argc, const char* argv[])
{
    using namespace std::string_literals;
    try {
        Options opt(argc, argv);
        acmacs::seqdb::setup(opt.seqdb);

        std::shared_ptr<acmacs::chart::Chart> chart;
        if (!opt.chart->empty())
            chart = acmacs::chart::import_from_file(opt.chart);

        Tree tree = tree::tree_import(opt.tree_file, chart);

        const auto [min_edge, max_edge] = tree.cumulative_edge_minmax();
        // std::cout << "mm: " << min_edge << ' ' << max_edge << '\n';
        const auto step = max_edge / static_cast<size_t>(opt.max_leaf_offset);

        if (opt.leaves_only)
            print_tree_leaves(tree, step);
        else
            print_tree(tree, step);

        return 0;
    }
    catch (std::exception& err) {
        std::cerr << "ERROR: " << err.what() << '\n';
        return 1;
    }
}

// ----------------------------------------------------------------------

void print_tree(const Tree& tree, double step)
{
    auto print_prefix = [step](const Node& node) {
        const auto edge = node.data.cumulative_edge_length / step;
        std::cout << std::string(static_cast<size_t>(std::lround(edge)), ' ');
    };

    auto print_cumul = [](const Node& node) { std::cout << "  [cumul: " << node.data.cumulative_edge_length << ']'; };

    auto print_leaf = [&](const Node& node) {
        print_prefix(node);
        std::cout << node.seq_id;
        if (node.draw.chart_antigen_index)
            std::cout << " [antigen: " << *node.draw.chart_antigen_index << ']';
        print_cumul(node);
        std::cout << '\n';
    };

    auto print_node = [&](const Node& node) {
        print_prefix(node);
        std::cout << '+';
        if (node.draw.matched_antigens)
            std::cout << "  ags:" << node.draw.matched_antigens;
        // print_cumul(node);
        std::cout << '\n';
    };

    tree::iterate_leaf_pre(tree, print_leaf, print_node);

} // print_tree

// ----------------------------------------------------------------------

void print_tree_leaves(const Tree& tree, double step)
{
    for (const auto* node : tree.leaf_nodes()) {
        const auto edge = node->data.cumulative_edge_length / step;
        std::cout << std::string(static_cast<size_t>(std::lround(edge)), ' ') << node->seq_id;
        if (node->draw.chart_antigen_index)
            std::cout << " [antigen: " << *node->draw.chart_antigen_index << ']';
        std::cout << "  [cumul: " << node->data.cumulative_edge_length
                  << ']'
                  // << "  [edge: " << edge << ']'
                  << '\n';
    }

} // print_tree_leaves

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
