#include <numeric>

#include "acmacs-base/stream.hh"
#include "seqdb/seqdb.hh"
#include "signature-page/tree.hh"
#include "signature-page/tree-export.hh"
#include "signature-page/tree-iterate.hh"

// ----------------------------------------------------------------------

static size_t hamming_distance(std::string seq1, std::string seq2);
static std::vector<std::string> diff_sequence(std::string seq1, std::string seq2);

// ----------------------------------------------------------------------

int main(int argc, const char* const* argv)
{
    int exit_code = 0;
    if (argc == 2) {
        std::string source_tree_file = argv[1];

        const auto seqdb = seqdb::get();
        Tree tree;
        tree::tree_import(source_tree_file, tree);
        tree.match_seqdb(seqdb);
        tree.set_number_strains();
        tree.ladderize(Tree::LadderizeMethod::NumberOfLeaves);
        tree.compute_cumulative_edge_length();

        Node* previous = nullptr;
        tree::iterate_leaf(tree, [&previous](Node& node) {
            if (previous) {
                // const auto dist = hamming_distance(previous->data.amino_acids(), node.data.amino_acids());
                // std::cout << std::setw(3) << std::right << dist << ' ' << node.seq_id << '\n';
                const auto diff = diff_sequence(previous->data.amino_acids(), node.data.amino_acids());
                std::cout << diff << " -- " << node.seq_id << " -- " << previous->seq_id << '\n';
            }
            previous = &node;
        });
    }
    else {
        std::cerr << "Usage: " << argv[0] << " tree.json[.xz]\n";
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------

size_t hamming_distance(std::string seq1, std::string seq2)
{
    size_t dist = 0;
    for (size_t pos = 0; pos < std::min(seq1.size(), seq2.size()); ++pos) {
        if (seq1[pos] != seq2[pos])
            ++dist;
    }
    return dist;

} // hamming_distance

// ----------------------------------------------------------------------

std::vector<std::string> diff_sequence(std::string seq1, std::string seq2)
{
    std::vector<std::string> result;
    for (size_t pos = 0; pos < std::min(seq1.size(), seq2.size()); ++pos) {
        if (seq1[pos] != seq2[pos])
            result.push_back(seq1[pos] + std::to_string(pos + 1) + seq2[pos]);
    }
    return result;

} // diff

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
