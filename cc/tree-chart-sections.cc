#include <iostream>

#include "acmacs-base/argc-argv.hh"
#include "acmacs-base/stream.hh"
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

                           {"--group-threshold", 10, "minimum nuber of antigens in the group"},
                           {"--group-series-sets", "", "write group-series-set-v1 json"},

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

        if (args["--group-series-sets"]) {
            using group_t = std::pair<std::string, std::vector<size_t>>;
            using groups_t = std::vector<group_t>;
            groups_t groups;
            const size_t max_in_group = tree.draw.matched_antigens / 3 * 2;
            auto make_groups = [&groups, max_in_group, threshold = static_cast<size_t>(args["--group-threshold"])](const Node& node, std::string path) {
                if (node.draw.matched_antigens >= threshold && node.draw.matched_antigens < max_in_group) {
                    auto& group = groups.emplace_back(path, std::vector<size_t>{});
                    tree::iterate_leaf(node, [&group](const Node& node2) {
                        if (node2.draw.chart_antigen_index)
                            group.second.push_back(*node2.draw.chart_antigen_index);
                    });
                }
            };
            tree::iterate_pre_path(tree, make_groups);
            for (const auto& group : groups)
                std::cout << std::setw(30) << std::left << group.first << ' ' << group.second << '\n';
        }
        else {
            auto report_antigens = [](const Node& node, std::string path) {
                if (node.draw.matched_antigens)
                    std::cout << std::setw(30) << std::left << path << ' ' << std::setw(4) << node.draw.matched_antigens << ' ' << std::setw(4) << node.subtree.size() << ' '
                              << find_first_leaf(node).seq_id << '\n';
            };

            tree::iterate_pre_path(tree, report_antigens);
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
