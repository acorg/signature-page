#include "acmacs-base/stream.hh"
#include "acmacs-base/enumerate.hh"
#include "aa-at-pos-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

void AAAtPosDraw::prepare()
{
    if (!mSettings.positions.empty()) {
        std::map<size_t, std::set<char>> aa_present;
        auto collect_aa = [&, this](const Node& aNode) {
            const auto aas = aNode.data.amino_acids();
            for (auto pos : this->mSettings.positions) {
                if ((pos - 1)  < aas.size() && aas[pos-1] != 'X')
                    aa_present[pos].insert(aas[pos-1]);
            }
        };
        tree::iterate_leaf(mTree, collect_aa);
        std::cout << "INFO: AAAtPosDraw: " << aa_present << '\n';
        for (auto pos : this->mSettings.positions) {
            for (auto [no, aa] : acmacs::enumerate(aa_present[pos]))
                mColors[pos].emplace(aa, Color::distinct(no));
        }
        // std::cout << "INFO: AAAtPosDraw: " << mColors << '\n';
    }

} // AAAtPosDraw::prepare

// ----------------------------------------------------------------------

void AAAtPosDraw::draw()
{
    if (!mSettings.positions.empty() && mSettings.width > 0) {
        const auto surface_width = mSurface.viewport().size.width;
        const auto section_width = surface_width / mSettings.positions.size();
        const auto line_length = section_width * mSettings.line_length;

        auto draw_dash = [&, this](const Node& aNode) {
            const auto aas = aNode.data.amino_acids();
            for (size_t section_no = 0; section_no < mSettings.positions.size(); ++section_no) {
                if (const auto pos = this->mSettings.positions[section_no]; (pos - 1) < aas.size()) {
                    const auto aa = aas[pos - 1];
                    if (const auto found = this->mColors[pos].find(aa); found != mColors[pos].end()) {
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
        for (size_t section_no = 0; section_no < mSettings.positions.size(); ++section_no) {
            const auto pos = mSettings.positions[section_no];
            mSurface.text({section_width * section_no + section_width / 4, mSurface.viewport().size.height + 10}, std::to_string(pos), 0, Pixels{line_length}, acmacs::TextStyle{}, Rotation{M_PI_2});
        }
    }

} // AAAtPosDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
