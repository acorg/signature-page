// Converts tree.json to data.csv and tree.newick for https://github.com/acorg/isig

#include "acmacs-base/data-formatter.hh"
#include "acmacs-base/read-file.hh"
#include "acmacs-base/enumerate.hh"
#include "seqdb/seqdb.hh"
#include "acmacs-chart-2/chart.hh"
#include "acmacs-chart-2/factory-import.hh"
#include "signature-page/tree.hh"
#include "signature-page/tree-export.hh"

// ----------------------------------------------------------------------

const size_t sBjornPos[] = {145, 155, 156, 158, 159, 189, 193};

int main(int argc, const char* const* argv)
{
    int exit_code = 0;
    if (argc == 5) {
        std::string source_tree_file = argv[1];
        std::string source_chart = argv[2];
        std::string target_csv = argv[3];
        std::string target_tree_file = argv[4];

        const auto seqdb = seqdb::get();
        Tree tree = tree::tree_import(source_tree_file);
          // tree.match_seqdb(seqdb);

        auto chart = acmacs::chart::import_from_file(source_chart); // , acmacs::chart::Verify::None, report_time::No
        auto antigens = chart->antigens();
        const auto per_antigen = seqdb.match(*antigens, chart->info()->virus_type());

          // data.csv: no,map_x,map_y,date,antigen name,bjorn-145,155,156,158,159,189,193
        auto layout = chart->projection(0)->layout();
        const auto number_of_dimensions = layout->number_of_dimensions();
        std::string data_csv;
        using DF = acmacs::DataFormatterCSV;
        size_t antigens_in_tree = 0;
        for (auto [ag_no, antigen] : acmacs::enumerate(*antigens)) {
            DF::first_field(data_csv, ag_no);
            for (auto dim : acmacs::range(number_of_dimensions))
                DF::second_field(data_csv, (*layout)(ag_no, dim));
            DF::second_field(data_csv, antigen->date());
            DF::second_field(data_csv, antigen->full_name());
            const auto& entry = per_antigen[ag_no];
            if (entry) {
                // std::cout << "entry " << entry.entry().name() << " -- " << antigen->full_name() << '\n';
                const auto seq = entry.seq().amino_acids(true);
                for (auto bp : sBjornPos)
                    DF::second_field(data_csv, seq[bp - 1]);
                if (auto node = tree.find_leaf_by_seqid(entry.seq_id(seqdb::SeqdbEntrySeq::encoded_t::yes)); node) {
                      // std::cout << ag_no << ' ' << node->seq_id << '\n';
                    node->seq_id = "s-" + std::to_string(ag_no); // rename node according to isig spec
                    ++antigens_in_tree;
                }
            }
            else {
                for ([[maybe_unused ]] auto bp : sBjornPos)
                    DF::second_field(data_csv, "");
            }
            DF::end_of_record(data_csv);
        }
        acmacs::file::write(target_csv, data_csv);
        std::cerr << "INFO: antigens of chart found in the tree: " << antigens_in_tree << '\n';

          // rename rest of tree nodes
        size_t tree_node_id = layout->number_of_points();
        tree::iterate_leaf(tree, [&tree_node_id](Node& node) {
            if (node.seq_id.substr(0, 2) != "s-")
                node.seq_id = "s-" + std::to_string(++tree_node_id);
        });

        tree::export_to_newick(target_tree_file, tree, 2);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " tree.json[.xz] chart.ace data.csv tree.newick\n";
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
