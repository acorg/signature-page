#include <iostream>
#include <cmath>

#include "acmacs-base/argc-argv.hh"
#include "acmacs-base/stream.hh"
#include "seqdb/seqdb.hh"
#include "acmacs-chart-2/factory-import.hh"
#include "tree.hh"
#include "tree-export.hh"

static void print_tree_leaves(const Tree& tree, double step);
static void print_tree(const Tree& tree, double step);

// ----------------------------------------------------------------------

int main(int argc, const char* argv[])
{
    using namespace std::string_literals;
    try {
        argc_argv args(argc, argv,
                       {
                           {"--db-dir", ""},
                           {"--seqdb", ""},
                           {"--chart", ""},
                           {"--max-leaf-offset", 80},
                           {"--leaves-only", false},

                           {"-v", false},
                           {"--verbose", false},
                           {"-h", false},
                           {"--help", false},
                       });

        if (args["-h"] || args["--help"] || args.number_of_arguments() != 1) {
            throw std::runtime_error("Usage: "s + args.program() + " [options] <tree.json>\n" + args.usage_options());
        }
        const bool verbose = args["-v"] || args["--verbose"];
        seqdb::setup_dbs(args["--db-dir"], verbose ? seqdb::report::yes : seqdb::report::no);
        if (args["--seqdb"])
            seqdb::setup(args["--seqdb"], verbose ? seqdb::report::yes : seqdb::report::no);

        Tree tree;
        tree::tree_import(args[0], tree);
        tree.match_seqdb(seqdb::get());
        if (args["--chart"]) {
            auto chart = acmacs::chart::import_from_file(args["--chart"], acmacs::chart::Verify::None, report_time::No);
            const auto matched_names = tree.match(*chart);
            if (matched_names)
                std::cout << "Tree sequences found in the chart: " << matched_names << std::endl;
        }
        tree.set_number_strains();
        tree.ladderize(Tree::LadderizeMethod::NumberOfLeaves);
        tree.compute_cumulative_edge_length();
        const auto [min_edge, max_edge] = tree.cumulative_edge_minmax();
        // std::cout << "mm: " << min_edge << ' ' << max_edge << '\n';
        const auto step = max_edge / static_cast<size_t>(args["--max-leaf-offset"]);

        if (args["--leaves-only"])
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
    auto count_antigens = [](const Node& node) -> size_t {
        size_t count = 0;
        tree::iterate_leaf(node, [&count](const Node& node2) {
            if (node2.draw.chart_antigen_index)
                ++count;
        });
        return count;
    };

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
        if (const auto ags = count_antigens(node); ags)
            std::cout << "  ags:" << ags;
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
