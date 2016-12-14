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

} // TreeDraw::prepare

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    mLineWidth = 1; // mSettings.force_line_width ? aSettings.line_width : std::min(aSettings.line_width, mVerticalStep * 0.5);
      // std::cout << "Tree line width: " << mLineWidth << "  Settings: " << aSettings.line_width << "  vertical_step/2: " << mVerticalStep * 0.5 << std::endl;
    // set_label_scale(aSurface, aTree, aViewport, aSettings);
    // set_horizontal_step(aSurface, aTree, aViewport, aSettings);

    // draw_grid(aSurface, aViewport, aSettings);
    // draw_node(aTree, aSurface, aViewport.origin, aSettings, aSettings.root_edge);

    // mark_nodes(aSurface);

} // TreeDraw::draw

// ----------------------------------------------------------------------

void TreeDraw::hide_leaves()
{
    auto hide_show_leaf = [this](Node& aNode) {
        aNode.data.shown = ! (aNode.data.date() < mSettings.hide_isolated_before || aNode.data.cumulative_edge_length > mSettings.hide_if_cumulative_edge_length_bigger_than);
    };

    auto hide_show_branch = [](Node& aNode) {
        aNode.data.shown = false;
        for (const auto& node: aNode.subtree) {
            if (node.data.shown) {
                aNode.data.shown = true;
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
        current_line += aNode.data.vertical_gap_before;
        if (aNode.data.shown) {
            aNode.data.line_no = current_line;
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
        if (aNode.data.shown) {
            aNode.data.top = -1;
            for (const auto& node: aNode.subtree) {
                if (node.data.shown) {
                    aNode.data.bottom = node.middle();
                    if (aNode.data.top < 0)
                        aNode.data.top = aNode.data.bottom;
                }
            }
        }
    };
    tree::iterate_post(mTree, set_top_bottom);

} // TreeDraw::set_top_bottom

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
