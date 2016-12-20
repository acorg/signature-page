#include <algorithm>

#include "tree-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "surface.hh"
#include "coloring.hh"

// ----------------------------------------------------------------------

TreeDraw::TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings)
    : mSurface(aSurface), mTree(aTree), mSettings(aSettings)
{
    make_coloring();
}

// ----------------------------------------------------------------------

// explicit destructor made here to allow proper mColoring destruction
TreeDraw::~TreeDraw()
{

} // TreeDraw::~TreeDraw

// ----------------------------------------------------------------------

void TreeDraw::make_coloring()
{
    if (mSettings.color_nodes == "black")
        mColoring = std::unique_ptr<Coloring>(new ColoringBlack());
    else if (mSettings.color_nodes == "continent")
        mColoring = std::unique_ptr<Coloring>(new ColoringByContinent());
    else {
        try {
            mColoring = std::unique_ptr<Coloring>(new ColoringByPos(std::stoul(mSettings.color_nodes)));
        }
        catch (std::exception&) {
            throw std::runtime_error("Unrecognized TreeDrawSettings.color_nodes: " + mSettings.color_nodes);
        }
    }

} // TreeDraw::make_coloring

// ----------------------------------------------------------------------

void TreeDraw::prepare()
{
    mTree.compute_cumulative_edge_length();
    hide_leaves();
    set_line_no();
    set_top_bottom();
    const auto canvas_size = mSurface.size();
    mHorizontalStep = canvas_size.width / mTree.width(mSettings.hide_if_cumulative_edge_length_bigger_than);
    mVerticalStep = canvas_size.height / (mTree.height() + 2); // +2 to add space at the top and bottom

} // TreeDraw::prepare

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    mLineWidth = mSettings.force_line_width ? mSettings.line_width : std::min(mSettings.line_width, mVerticalStep * 0.5);
    std::cout << "Tree line width: " << mLineWidth << "  Settings: " << mSettings.line_width << "  vertical_step/2: " << mVerticalStep * 0.5 << std::endl;
    fit_labels_into_viewport();

    draw_node(mTree, Location{mLineWidth / 2, 0}, mSettings.root_edge);
    mColoring->report();

    // mark_nodes(aSurface);

} // TreeDraw::draw

// ----------------------------------------------------------------------

void TreeDraw::hide_leaves()
{
    auto hide_show_leaf = [this](Node& aNode) {
        aNode.draw.shown = ! (aNode.data.date() < mSettings.hide_isolated_before || aNode.data.cumulative_edge_length > mSettings.hide_if_cumulative_edge_length_bigger_than);
    };

    auto hide_show_branch = [](Node& aNode) {
        aNode.draw.shown = false;
        for (const auto& node: aNode.subtree) {
            if (node.draw.shown) {
                aNode.draw.shown = true;
                break;
            }
        }
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_show_branch);

} // TreeDraw::hide_leaves

// ----------------------------------------------------------------------

void TreeDraw::set_line_no()
{
    size_t current_line = 1;    // line of the first node is 1, we have 1 line space at the top and bottom of the tree
    auto set_line_no = [&current_line](Node& aNode) {
        current_line += aNode.draw.vertical_gap_before;
        if (aNode.draw.shown) {
            aNode.draw.line_no = current_line;
            ++current_line;
        }
    };
    tree::iterate_leaf(mTree, set_line_no);
    std::cout << "Lines in the tree: " << current_line << std::endl;

} // TreeDraw::set_line_no

// ----------------------------------------------------------------------

void TreeDraw::set_top_bottom()
{
    auto set_top_bottom = [](Node& aNode) {
        if (aNode.draw.shown) {
            aNode.draw.top = -1;
            for (const auto& node: aNode.subtree) {
                if (node.draw.shown) {
                    aNode.draw.bottom = node.middle();
                    if (aNode.draw.top < 0)
                        aNode.draw.top = aNode.draw.bottom;
                }
            }
        }
    };
    tree::iterate_post(mTree, set_top_bottom);

} // TreeDraw::set_top_bottom

// ----------------------------------------------------------------------

void TreeDraw::calculate_name_offset()
{
    const auto tsize = mSurface.text_size("W", mFontSize, mSettings.label_style);
    mNameOffset  = mSettings.name_offset * tsize.width;
      // mLabelHeight = tsize.height;

} // TreeDraw::calculate_name_offset

// ----------------------------------------------------------------------

void TreeDraw::fit_labels_into_viewport()
{
    mFontSize = mVerticalStep;

    const double canvas_width = mSurface.size().width;

    for (double label_offset = max_label_offset(); label_offset > canvas_width; label_offset = max_label_offset()) {
        const double scale = std::min(canvas_width / label_offset, 0.99); // to avoid too much looping
          // std::cerr << "Canvas:" << canvas_width << " label_right:" << label_offset << " scale: " << scale << std::endl;
        mHorizontalStep *= scale;
        mFontSize *= scale;
    }

} // TreeDraw::fit_labels_into_viewport

// ----------------------------------------------------------------------

double TreeDraw::max_label_offset()
{
    calculate_name_offset();

    double max_label_origin = 0, max_label_right = 0;
    auto label_offset = [&](Node& node) {
        if (node.draw.shown) {
            const double label_origin = node.data.cumulative_edge_length * mHorizontalStep + mNameOffset;
            max_label_origin = std::max(max_label_origin, label_origin);
            max_label_right = std::max(max_label_right, label_origin + this->text_width(node.display_name()));
        }
    };
    tree::iterate_leaf(mTree, label_offset);
    return max_label_right; // std::make_pair(max_label_origin, max_label_right);

} // TreeDraw::max_label_offset

// ----------------------------------------------------------------------

void TreeDraw::draw_node(const Node& aNode, const Location& aOrigin, double aEdgeLength)
{
    if (aNode.draw.shown) {
        Location origin{aOrigin.x, aOrigin.y + mVerticalStep * aNode.middle()};
        Size size{(aEdgeLength < 0.0 ? aNode.edge_length : aEdgeLength) * mHorizontalStep, 0};
        const double right = aOrigin.x + size.width;

        mSurface.line(origin, {right, origin.y}, mSettings.line_color, mLineWidth);
          // draw_aa_transition(aNode, aSurface, viewport, aSettings.aa_transition);
        if (aNode.is_leaf()) {
            const std::string text = aNode.display_name();
            const auto tsize = mSurface.text_size(text, mFontSize, mSettings.label_style);
            const Location text_origin{right + mNameOffset, origin.y + tsize.height / 2};
            mSurface.text(text_origin, text, mColoring->color(aNode), mFontSize, mSettings.label_style);
              //         auto mark_node = mNodesToMark.find(aNode.name);
              //         if (mark_node != mNodesToMark.end())
              //             mark_node->second.set(text_origin, aNode);
        }
        else {
              // if (aShowBranchIds && !aNode.branch_id.empty()) {
              //     show_branch_id(aSurface, aNode.branch_id, aLeft, y);
              // }
              // if (!aNode.name.empty() && aNode.number_strains > aNumberStrainsThreshold) {
              //     show_branch_annotation(aSurface, aNode.branch_id, aNode.name, aLeft, right, y);
              // }
            mSurface.line({right, aOrigin.y + mVerticalStep * aNode.draw.top}, {right, aOrigin.y + mVerticalStep * aNode.draw.bottom}, mSettings.line_color, mLineWidth, Surface::LineCap::Square);
              // draw_aa_transition(aNode, aSurface, viewport, aSettings.aa_transition);
            for (auto& node: aNode.subtree) {
                draw_node(node, {right, aOrigin.y});
            }
        }
    }

} // TreeDraw::draw_node

// ----------------------------------------------------------------------

Legend* TreeDraw::coloring_legend() const
{
    return mColoring ? mColoring->legend() : nullptr;

} // TreeDraw::coloring_legend

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
