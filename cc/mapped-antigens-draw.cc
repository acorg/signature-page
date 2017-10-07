#include "acmacs-base/debug.hh"

#include "mapped-antigens-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "chart-draw.hh"

// ----------------------------------------------------------------------

void MappedAntigensDraw::prepare()
{
    std::set<std::string> matched_names;
    auto match_chart_antigens = [this,&matched_names](Node& aNode) {
        aNode.draw.chart_antigen_index = ChartDrawBase::AntigenNotFound;
        if (aNode.draw.shown) {
            const std::vector<std::string>* hi_names = aNode.data.hi_names();
            if (hi_names) {
                for (const auto& name: *hi_names) {
                    const size_t antigen_index = mChart.find_antigen(name);
                    if (antigen_index != ChartDrawBase::AntigenNotFound) {
                        aNode.draw.chart_antigen_index = antigen_index;
                        matched_names.insert(name);
                        break;
                    }
                }
            }
        }
    };
    tree::iterate_leaf(mTree, match_chart_antigens);

    if (matched_names.size())
        std::cout << "Tree sequences found in the chart: " << matched_names.size() << std::endl;
    else
        std::cerr << "WARNING: No tree sequences found in the chart" << DEBUG_LINE_FUNC << '\n';

    // std::cerr << "Matched names:" << std::endl << "  ";
    // std::copy(matched_names.begin(), matched_names.end(), polyfill::make_ostream_joiner(std::cerr, "\n  "));
    // std::cerr << std::endl << std::endl;

    // std::cerr << "Not matched names:" << std::endl;
    // for (const auto& point: mChart.points()) {
    //     if (point.attributes.antigen && matched_names.find(point.name) == matched_names.end())
    //         std::cerr << "  " << point.name << std::endl;
    // }

} // MappedAntigensDraw::prepare

// ----------------------------------------------------------------------

void MappedAntigensDraw::draw()
{
      // mSurface.border("brown", 1);

    const double surface_width = mSurface.viewport().size.width;
    const double line_length = surface_width * mSettings.line_length;
    const double base_x = (surface_width - line_length) / 2;

    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown && aNode.draw.chart_antigen_index != ChartDrawBase::AntigenNotFound) {
            mSurface.line({base_x, aNode.draw.vertical_pos}, {base_x + line_length, aNode.draw.vertical_pos}, mSettings.line_color, Pixels{mSettings.line_width}, Surface::LineCap::Round);
        }
    };
    tree::iterate_leaf(mTree, draw_dash);

} // MappedAntigensDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
