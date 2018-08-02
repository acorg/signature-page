#include <numeric>
#include <map>
#include <cmath>

#include "acmacs-base/stream.hh"
#include "acmacs-base/enumerate.hh"
#include "signature-page/tree.hh"
#include "signature-page/tree-export.hh"

// ----------------------------------------------------------------------

using AADiff = std::vector<std::string>;

struct DiffEntry
{
    const Node* previous;
    const Node* current;
    AADiff with_other;
};

inline std::ostream& operator<<(std::ostream& s, const DiffEntry& c)
{
    return s << c.previous->seq_id << " -> " << c.current->seq_id << ' ' << c.with_other;
}

using AllDiffs = std::map<AADiff, std::vector<DiffEntry>>;

// ----------------------------------------------------------------------

static size_t hamming_distance(std::string seq1, std::string seq2);
static AADiff diff_sequence(std::string seq1, std::string seq2);
static AllDiffs collect(const Tree& tree);
static void compute_entries_diffs(AllDiffs& diffs);

// ----------------------------------------------------------------------

int main(int argc, const char* const* argv)
{
    int exit_code = 0;
    if (argc == 2) {
        std::string source_tree_file = argv[1];

        const Tree tree = tree::tree_import(source_tree_file, nullptr);

        auto diffs = collect(tree);
        compute_entries_diffs(diffs);
        for (const auto& entry : diffs) {
            std::cout << entry.first << '\n';
            for (const auto& diff : entry.second) {
                std::cout << "  " << diff << '\n';
            }
        }

        std::map<size_t, std::map<char, size_t>> aa_per_pos;
        auto collect_aa_per_pos = [&](const Node& node) {
            const auto sequence = node.data.amino_acids();
            for (auto [pos, aa] : acmacs::enumerate(sequence)) {
                if (aa != 'X')
                    ++aa_per_pos[pos][aa];
            }
        };
        tree::iterate_leaf(tree, collect_aa_per_pos);
        for (auto it = aa_per_pos.begin(); it != aa_per_pos.end();) {
            if (it->second.size() < 2)
                it = aa_per_pos.erase(it);
            else
                ++it;
        }
        // std::cout << "======================================================================\n";
        // for (const auto& pos_e : aa_per_pos)
        //     std::cout << std::setw(3) << std::right << (pos_e.first + 1) << ' ' << pos_e.second << '\n';

          // https://en.wikipedia.org/wiki/Diversity_index
        using all_pos_t = std::pair<size_t, ssize_t>;
        std::vector <all_pos_t> all_pos(aa_per_pos.size());
        std::transform(aa_per_pos.begin(), aa_per_pos.end(), all_pos.begin(), [](const auto& src) -> all_pos_t {
            const auto sum = std::accumulate(src.second.begin(), src.second.end(), 0UL, [](auto accum, const auto& entry) { return accum + entry.second; });
            const auto shannon_index = -std::accumulate(src.second.begin(), src.second.end(), 0.0, [sum = double(sum)](auto accum, const auto& entry) {
                const double p = entry.second / sum;
                return accum + p * std::log(p);
            });
            return {src.first, std::lround(shannon_index * 100)};
        });
        std::sort(std::begin(all_pos), std::end(all_pos), [](const auto& p1, const auto& p2) { return p1.second > p2.second; });
        std::cout << "======================================================================\n";
        for (const auto& pos_e : all_pos)
            std::cout << std::setw(3) << std::right << (pos_e.first + 1) << ' ' << aa_per_pos[pos_e.first] << '\n';
    }
    else {
        std::cerr << "Usage: " << argv[0] << " tree.json[.xz]\n";
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------

void compute_entries_diffs(AllDiffs& diffs)
{
    for (auto& entry : diffs) {
        DiffEntry* prev = nullptr;
        for (auto& ee : entry.second) {
            if (prev) {
                ee.with_other = diff_sequence(prev->current->data.amino_acids(), ee.current->data.amino_acids());
            }
            prev = &ee;
        }
    }

} // compute_entries_diffs

// ----------------------------------------------------------------------

AllDiffs collect(const Tree &tree)
{
    AllDiffs diffs;
    const Node *previous = nullptr;
    tree::iterate_leaf(tree, [&previous, &diffs](const Node &node) {
        if (previous) {
            // const auto dist = hamming_distance(previous->data.amino_acids(), node.data.amino_acids());
            // std::cout << std::setw(3) << std::right << dist << ' ' << node.seq_id << '\n';
            const auto diff = diff_sequence(previous->data.amino_acids(), node.data.amino_acids());
            if (!diff.empty()) {
                auto found = diffs.find(diff);
                if (found == diffs.end())
                    diffs.emplace(diff, std::vector<DiffEntry>{DiffEntry{previous, &node, {}}});
                else
                    found->second.push_back(DiffEntry{previous, &node, {}});
                // std::cout << diff << " -- " << node.seq_id << " -- " << previous->seq_id << '\n';
            }
        }
        previous = &node;
    });
    return diffs;

} // collect

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

AADiff diff_sequence(std::string seq1, std::string seq2)
{
    AADiff result;
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
