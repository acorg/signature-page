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
        std::cout << "INFO: AAAtPosDraw: " << mColors << '\n';
    }

} // AAAtPosDraw::prepare

// ----------------------------------------------------------------------

void AAAtPosDraw::draw()
{
    if (!mSettings.positions.empty()) {
        const auto surface_width = mSurface.viewport().size.width;
        const auto section_width = surface_width / mSettings.positions.size();
        const auto line_length = section_width * mSettings.line_length;

        auto draw_dash = [&,this](const Node& aNode) {
            for (size_t section_no = 0; section_no < mSettings.positions.size(); ++section_no) {
                const auto pos = this->mSettings.positions[section_no];
                const auto base_x = section_width * section_no + (section_width - line_length) / 2;
                if (const auto aas = aNode.data.amino_acids(); pos < aas.size()) {
                    if (const auto found = this->mColors[pos].find(aas[pos-1]); found != mColors[pos].end())
                        mSurface.line({base_x, aNode.draw.vertical_pos}, {base_x + line_length, aNode.draw.vertical_pos}, found->second, Pixels{this->mSettings.line_width}, acmacs::surface::LineCap::Round);
                }
            }
        };
        tree::iterate_leaf(mTree, draw_dash);

        for (size_t section_no = 0; section_no < mSettings.positions.size(); ++section_no) {
            const auto pos = mSettings.positions[section_no];
            mSurface.text({section_width * section_no, mSurface.viewport().size.height + 10}, std::to_string(pos), 0, Pixels{8}, acmacs::TextStyle{}, Rotation{M_PI_2});
        }
    }

} // AAAtPosDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
