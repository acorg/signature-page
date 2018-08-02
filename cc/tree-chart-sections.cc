#include <iostream>

#include "acmacs-base/argc-argv.hh"
#include "acmacs-chart-2/factory-import.hh"
#include "tree.hh"
#include "tree-export.hh"

// ----------------------------------------------------------------------

int main(int argc, const char* argv[])
{
    using namespace std::string_literals;
    try {
        argc_argv args(argc, argv,
                       {
                           {"--db-dir", ""},
                           {"--seqdb", ""},

                           {"-v", false},
                           {"--verbose", false},
                           {"-h", false},
                           {"--help", false},
                       });

        if (args["-h"] || args["--help"] || args.number_of_arguments() != 2) {
            throw std::runtime_error("Usage: "s + args.program() + " [options] <tree.json> <chart.ace>\n" + args.usage_options());
        }
        const bool verbose = args["-v"] || args["--verbose"];
        seqdb::setup_dbs(args["--db-dir"], verbose ? seqdb::report::yes : seqdb::report::no);
        if (args["--seqdb"])
            seqdb::setup(args["--seqdb"], verbose ? seqdb::report::yes : seqdb::report::no);

        std::shared_ptr<acmacs::chart::Chart> chart = acmacs::chart::import_from_file(args[1], acmacs::chart::Verify::None, report_time::No);
        Tree tree = tree::tree_import(args[0], chart);

        auto report_antigens = [](const Node& node, std::string path) {
            if (node.draw.matched_antigens)
                std::cout << std::setw(30) << std::left << path << ' ' << std::setw(4) << node.draw.matched_antigens << ' ' << std::setw(4) << node.subtree.size() << ' ' << find_first_leaf(node).seq_id << '\n';
        };

        tree::iterate_pre_path(tree, report_antigens);

        return 0;
    }
    catch (std::exception& err) {
        std::cerr << "ERROR: " << err.what() << '\n';
        return 1;
    }
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
