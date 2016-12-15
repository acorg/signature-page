#include "tree-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "surface.hh"

// ----------------------------------------------------------------------

void TreeDraw::prepare()
{
    mTree.compute_cumulative_edge_length();
    hide_leaves();
    set_line_no();
    set_top_bottom();
    const auto canvas_size = mSurface.size();
    mHorizontalStep = canvas_size.width / mTree.width(); // * 0.9;
    mVerticalStep = canvas_size.height / mTree.height(); // + 2); // +2 to add space at the top and bottom

} // TreeDraw::prepare

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    mLineWidth = mSettings.force_line_width ? mSettings.line_width : std::min(mSettings.line_width, mVerticalStep * 0.5);
    std::cout << "Tree line width: " << mLineWidth << "  Settings: " << mSettings.line_width << "  vertical_step/2: " << mVerticalStep * 0.5 << std::endl;
    set_label_scale();

    draw_node(mTree, Location{mLineWidth / 2, (mLabelHeight + mLineWidth) / 2}, mSettings.root_edge);

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
    size_t current_line = 0;
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

void TreeDraw::set_label_scale()
{
    // const double width = mSurface.size().width;
    // mLabelScale = 1.0;
    mFontSize = mVerticalStep; // * mLabelScale;

      // $$$
    // auto adjust_font_size = [this](Node& node) {
    // };
    // tree::iterate_leaf(mTree, adjust_font_size);

    // mWidth = tree_width(aSurface, aTree, aSettings);
    // for (int i = 0; (mLabelScale * mVerticalStep) > 1.0 && mWidth > aViewport.size.width; ++i) {
    //     mLabelScale *= 0.95;
    //     mWidth = tree_width(aSurface, aTree, aSettings, aSettings.root_edge);
    //       // std::cerr << i << " label scale: " << mLabelScale << "  width:" << mWidth << " right:" << tree_right_margin << std::endl;
    // }

    const auto tsize = mSurface.text_size("W", mFontSize, mSettings.label_style);
    mNameOffset  = mSettings.name_offset * tsize.width;
    mLabelHeight = tsize.height;

} // TreeDraw::set_label_scale

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
            mSurface.text(text_origin, text, 0 /*mColoring->color(aNode)*/, mFontSize, mSettings.label_style);
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
            mSurface.line({right, aOrigin.y + mVerticalStep * aNode.draw.top}, {right, aOrigin.y + mVerticalStep * aNode.draw.bottom}, mSettings.line_color, mLineWidth);
              // draw_aa_transition(aNode, aSurface, viewport, aSettings.aa_transition);
            for (auto& node: aNode.subtree) {
                draw_node(node, {right, aOrigin.y});
            }
        }
    }

} // TreeDraw::draw_node

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
