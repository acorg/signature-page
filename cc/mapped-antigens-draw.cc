#include "mapped-antigens-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

void MappedAntigensDraw::prepare()
{
    size_t matched = 0;
    auto match_chart_antigens = [this,&matched](Node& aNode) {
        aNode.draw.chart_antigen_index = Chart::AntigenNotFound;
        if (aNode.draw.shown) {
            const std::vector<std::string>* hi_names = aNode.data.hi_names();
            if (hi_names) {
                for (const auto& name: *hi_names) {
                    const size_t antigen_index = mChart.find_antigen(name);
                    if (antigen_index != Chart::AntigenNotFound) {
                        aNode.draw.chart_antigen_index = antigen_index;
                        ++matched;
                        break;
                    }
                }
            }
        }
    };
    tree::iterate_leaf(mTree, match_chart_antigens);

    std::cerr << "Tree sequences found in the chart: " << matched << std::endl;

} // MappedAntigensDraw::prepare

// ----------------------------------------------------------------------

void MappedAntigensDraw::draw()
{
      // mSurface.border("brown", 1);

    const double surface_width = mSurface.size().width;
    const double line_length = surface_width * mSettings.line_length;
    const double base_x = (surface_width - line_length) / 2;

    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown && aNode.draw.chart_antigen_index != Chart::AntigenNotFound) {
            mSurface.line({base_x, aNode.draw.vertical_pos}, {base_x + line_length, aNode.draw.vertical_pos}, mSettings.line_color, mSettings.line_width, Surface::LineCap::Round);
        }
    };
    tree::iterate_leaf(mTree, draw_dash);

} // MappedAntigensDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
