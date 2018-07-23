#include "acmacs-base/stream.hh"
#include "acmacs-base/enumerate.hh"
#include "aa-at-pos-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

void AAAtPosDraw::prepare()
{
    if (!mSettings.positions.empty()) {
        std::set<char> aa_present;
        auto collect_aa = [&, this](const Node& aNode) {
            const auto aas = aNode.data.amino_acids();
            if (const size_t pos = this->mSettings.positions[0] - 1; pos  < aas.size() && aas[pos] != 'X')
                aa_present.insert(aas[pos]);
        };
        tree::iterate_leaf(mTree, collect_aa);
        for (auto [no, aa] : acmacs::enumerate(aa_present))
            mColors.emplace(aa, Color::distinct(no));
        std::cout << "INFO: AAAtPosDraw: " << mColors << '\n';
    }

} // AAAtPosDraw::prepare

// ----------------------------------------------------------------------

void AAAtPosDraw::draw()
{
    const double surface_width = mSurface.viewport().size.width;
    const double line_length = surface_width * mSettings.line_length;
    const double base_x = (surface_width - line_length) / 2;

    auto draw_dash = [&](const Node& aNode) {
        const auto aas = aNode.data.amino_acids();
        if (const size_t pos = this->mSettings.positions[0] - 1; pos < aas.size()) {
            if (const auto found = mColors.find(aas[pos]); found != mColors.end())
                mSurface.line({base_x, aNode.draw.vertical_pos}, {base_x + line_length, aNode.draw.vertical_pos}, found->second, Pixels{mSettings.line_width}, acmacs::surface::LineCap::Round);
        }
    };
    tree::iterate_leaf(mTree, draw_dash);

    mSurface.text({0, mSurface.viewport().size.height + 5}, std::to_string(mSettings.positions[0]), 0, Pixels{8}, acmacs::TextStyle{}, Rotation{M_PI_2});

} // AAAtPosDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
