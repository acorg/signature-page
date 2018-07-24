#include "acmacs-base/stream.hh"
#include "acmacs-base/enumerate.hh"
#include "aa-at-pos-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

void AAAtPosDraw::prepare()
{
    if (mSettings.width > 0) {
        if (!mSettings.positions.empty()) {
            positions_.resize(mSettings.positions.size());
            std::transform(mSettings.positions.begin(), mSettings.positions.end(), positions_.begin(), [](size_t pos_based_1) { return pos_based_1 - 1; });
            collect_aa_per_pos();
        }
        else if (mSettings.most_diverse_positions > 0) {
            find_most_diverse_positions();
        }
        if (!positions_.empty()) {
            for (auto pos : positions_) {
                const auto& aa_freq = aa_per_pos_[pos];
                std::vector<char> aas(aa_freq.size());
                std::transform(aa_freq.begin(), aa_freq.end(), aas.begin(), [](const auto& entry) { return entry.first; });
                std::sort(aas.begin(), aas.end(), [&](char aa1, char aa2) { return aa_freq.find(aa1)->second > aa_freq.find(aa2)->second; }); // most frequent aa first
                std::cout << pos << ' ' << aas << ' ' << aa_freq << '\n';
                for (size_t no = 1; no < aas.size(); ++no) // no color for the most frequent aa
                    colors_[pos].emplace(aas[no], Color::distinct(no));
            }
        }
    }

} // AAAtPosDraw::prepare

// ----------------------------------------------------------------------

void AAAtPosDraw::collect_aa_per_pos()
{
    auto update = [this](size_t pos, char aa) {
        if (aa != 'X')
            ++this->aa_per_pos_[pos][aa];
    };
    auto collect = [&, this](const Node& node) {
        const auto sequence = node.data.amino_acids();
        if (this->positions_.empty()) {
            for (auto [pos, aa] : acmacs::enumerate(sequence))
                update(pos, aa);
        }
        else {
            for (auto pos : this->positions_) {
                if (pos < sequence.size())
                    update(pos, sequence[pos]);
            }
        }
    };
    tree::iterate_leaf(mTree, collect);

} // AAAtPosDraw::collect_aa_per_pos

// ----------------------------------------------------------------------

void AAAtPosDraw::find_most_diverse_positions()
{
    collect_aa_per_pos();

    // https://en.wikipedia.org/wiki/Diversity_index
    using all_pos_t = std::pair<size_t, ssize_t>; // position, shannon_index
    std::vector<all_pos_t> all_pos(aa_per_pos_.size());
    std::transform(aa_per_pos_.begin(), aa_per_pos_.end(), all_pos.begin(), [](const auto& src) -> all_pos_t {
        const auto sum = std::accumulate(src.second.begin(), src.second.end(), 0UL, [](auto accum, const auto& entry) { return accum + entry.second; });
        const auto shannon_index = -std::accumulate(src.second.begin(), src.second.end(), 0.0, [sum = double(sum)](auto accum, const auto& entry) {
            const double p = entry.second / sum;
            return accum + p * std::log(p);
        });
        return {src.first, std::lround(shannon_index * 100)};
                                                                            });
      // sort all_pos by shannon_index, more diverse first
    std::sort(std::begin(all_pos), std::end(all_pos), [](const auto& p1, const auto& p2) { return p1.second > p2.second; });

    positions_.resize(std::min(static_cast<size_t>(mSettings.most_diverse_positions), all_pos.size()));
    std::transform(all_pos.begin(), all_pos.begin() + static_cast<ssize_t>(positions_.size()), positions_.begin(), [](const all_pos_t& entry) { return entry.first; });

} // AAAtPosDraw::find_most_diverse_positions

// ----------------------------------------------------------------------

void AAAtPosDraw::draw()
{
    if (!positions_.empty()) {
        const auto surface_width = mSurface.viewport().size.width;
        const auto section_width = surface_width / positions_.size();
        const auto line_length = section_width * mSettings.line_length;

        auto draw_dash = [&, this](const Node& aNode) {
            const auto aas = aNode.data.amino_acids();
            for (size_t section_no = 0; section_no < positions_.size(); ++section_no) {
                if (const auto pos = this->positions_[section_no]; pos < aas.size()) {
                    const auto aa = aas[pos];
                    if (const auto found = this->colors_[pos].find(aa); found != colors_[pos].end()) {
                        const auto base_x = section_width * section_no + (section_width - line_length) / 2;
                        const std::string aa_s(1, aa);
                        const auto aa_width = mSurface.text_size(aa_s, Pixels{this->mSettings.line_width}).width * 2;
                        mSurface.text({base_x, aNode.draw.vertical_pos + this->mSettings.line_width / 2}, aa_s, 0 /* found->second */, Pixels{this->mSettings.line_width});
                        mSurface.line({base_x + aa_width, aNode.draw.vertical_pos}, {base_x + line_length - aa_width, aNode.draw.vertical_pos}, found->second, Pixels{this->mSettings.line_width},
                                      acmacs::surface::LineCap::Round);
                    }
                }
            }
        };
        tree::iterate_leaf(mTree, draw_dash);

        // const auto pos_text_height = mSurface.text_size("8", Pixels{}).height;
        for (size_t section_no = 0; section_no < positions_.size(); ++section_no) {
            const auto pos = positions_[section_no];
            mSurface.text({section_width * section_no + section_width / 4, mSurface.viewport().size.height + 10}, std::to_string(pos + 1), 0, Pixels{line_length}, acmacs::TextStyle{}, Rotation{M_PI_2});
        }
    }

} // AAAtPosDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
