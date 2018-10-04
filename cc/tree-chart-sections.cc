#include <iostream>

#include "acmacs-base/argc-argv.hh"
#include "acmacs-base/json-writer.hh"
#include "acmacs-chart-2/factory-import.hh"
#include "tree.hh"
#include "tree-export.hh"

// ----------------------------------------------------------------------

struct group_t
{
    group_t(std::string a_path, size_t a_group_index) : path(a_path), group_index(a_group_index) {}
    std::string path;
    std::string first, last;
    size_t group_index;
    std::vector<size_t> members;
};

struct groups_t
{
    std::vector<group_t> groups;
};

template <typename RW> inline json_writer::writer<RW>& operator<<(json_writer::writer<RW>& writer, const group_t& group)
{
    writer << json_writer::start_object
           << "N" << std::to_string(group.group_index) + ' ' + group.first + ' ' + group.last + " (" + std::to_string(group.members.size()) + ')'
           // << "N" << std::to_string(group.group_index) + ": " + group.path + " (" + std::to_string(group.members.size()) + ')'
           // << "path" << group.path
           // << "num_members" << group.members.size()
           << "members" << group.members
           << json_writer::end_object;
    return writer;
}

template <typename RW> inline json_writer::writer<RW>& operator<<(json_writer::writer<RW>& writer, const groups_t& groups)
{
    writer << json_writer::start_object << "  version"
           << "group-series-set-v1"
           << "group_sets" << json_writer::start_array << json_writer::start_object << "N"
           << "tree-chart-sections"
           << "groups";
    json_writer::write_list(writer, groups.groups);
    writer << json_writer::end_object << json_writer::end_array << json_writer::end_object;
    return writer;
}

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
        seqdb::setup_dbs(static_cast<std::string>(args["--db-dir"]), verbose ? seqdb::report::yes : seqdb::report::no);
        if (args["--seqdb"])
            seqdb::setup(static_cast<std::string>(args["--seqdb"]), verbose ? seqdb::report::yes : seqdb::report::no);

        std::shared_ptr<acmacs::chart::Chart> chart = acmacs::chart::import_from_file(args[1], acmacs::chart::Verify::None, report_time::No);
        Tree tree = tree::tree_import(std::string(args[0]), chart);

        if (args["--group-series-sets"]) {
            groups_t groups;
            const size_t max_in_group = tree.draw.matched_antigens / 3 * 2;
            auto make_groups = [&groups, max_in_group, threshold = static_cast<size_t>(args["--group-threshold"])](const Node& node, std::string path) {
                if (node.draw.matched_antigens >= threshold && node.draw.matched_antigens < max_in_group) {
                    auto& group = groups.groups.emplace_back(path, groups.groups.size() + 1);
                    tree::iterate_leaf(node, [&group](const Node& node2) {
                        if (node2.draw.chart_antigen_index)
                            group.members.push_back(*node2.draw.chart_antigen_index);
                        if (group.first.empty())
                            group.first = node2.seq_id;
                        group.last = node2.seq_id;
                    });
                }
            };
            tree::iterate_pre_path(tree, make_groups);
            json_writer::export_to_json(groups, static_cast<std::string>(args["--group-series-sets"]), 2);
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
