#include <iostream>
#include <cmath>

#include "acmacs-base/argc-argv.hh"
#include "acmacs-base/stream.hh"
#include "seqdb/seqdb.hh"
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
                           {"--chart", ""},
                           {"--max-leaf-offset", 80},

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
        // if (args["--chart"]) {
        //     auto chart = acmacs::chart::import_from_file(args["--chart"], acmacs::chart::Verify::None, report_time::No);
        // }
        tree.set_number_strains();
        tree.ladderize(Tree::LadderizeMethod::NumberOfLeaves);
        tree.compute_cumulative_edge_length();
        const auto [min_edge, max_edge] = tree.cumulative_edge_minmax();
          // std::cout << "mm: " << min_edge << ' ' << max_edge << '\n';
        const auto step = max_edge / static_cast<size_t>(args["--max-leaf-offset"]);

        for (const auto* node : tree.leaf_nodes()) {
            const auto edge = node->data.cumulative_edge_length / step;
            std::cout << std::string(static_cast<size_t>(std::lround(edge)), ' ')
                      << node->seq_id
                      << "  [edge: " << edge << ']'
                      << '\n';
        }

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
