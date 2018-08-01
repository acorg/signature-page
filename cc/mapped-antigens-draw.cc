#include "acmacs-base/debug.hh"
#include "acmacs-base/timeit.hh"
#include "acmacs-base/stream.hh"

#include "mapped-antigens-draw.hh"
#include "tree.hh"
#include "chart-draw.hh"

// ----------------------------------------------------------------------

void MappedAntigensDraw::prepare()
{
    const auto matched_names = mTree.match(mChart.chart());
    if (matched_names)
        std::cout << "INFO: tree sequences found in the chart: " << matched_names << std::endl;
    // else
    //     std::cerr << "WARNING: No tree sequences found in the chart" << DEBUG_LINE_FUNC << '\n';

    // std::set<std::string> matched_names;
    // auto match_chart_antigens = [this,&matched_names](Node& aNode) {
    //     aNode.draw.chart_antigen_index.reset();
    //     if (aNode.draw.shown) {
    //         const std::vector<std::string>* hi_names = aNode.data.hi_names();
    //         if (hi_names && !hi_names->empty()) {
    //             for (const auto& name: *hi_names) {
    //                   // Timeit ti_name(name + ": ");
    //                 const auto antigen_index = mChart.find_antigen(name);
    //                 if (antigen_index) {
    //                     aNode.draw.chart_antigen_index = antigen_index;
    //                     matched_names.insert(name);
    //                       // ti_name.message_append(" #found: ");
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    // };

    // Timeit ti("matching tree leaf nodes and names in the chart: ");
    // tree::iterate_leaf(mTree, match_chart_antigens);
    // ti.report();

    // if (matched_names.size())
    //     std::cout << "Tree sequences found in the chart: " << matched_names.size() << std::endl;
    // else
    //     std::cerr << "WARNING: No tree sequences found in the chart" << DEBUG_LINE_FUNC << '\n';

    // // std::cerr << "DEBUG: Matched names:" << std::endl << "  ";
    // // std::copy(matched_names.begin(), matched_names.end(), polyfill::make_ostream_joiner(std::cerr, "\n  "));
    // // std::cerr << std::endl << std::endl;

    // // std::cerr << "DEBUG: Not matched names:" << std::endl;
    // // for (const auto& point: mChart.points()) {
    // //     if (point.attributes.antigen && matched_names.find(point.name) == matched_names.end())
    // //         std::cerr << "  " << point.name << std::endl;
    // // }

} // MappedAntigensDraw::prepare

// ----------------------------------------------------------------------

void MappedAntigensDraw::draw()
{
      // mSurface.border("brown", 1);

    const double surface_width = mSurface.viewport().size.width;
    const double line_length = surface_width * mSettings.line_length;
    const double base_x = (surface_width - line_length) / 2;

    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown && aNode.draw.chart_antigen_index) {
            mSurface.line({base_x, aNode.draw.vertical_pos}, {base_x + line_length, aNode.draw.vertical_pos}, mSettings.line_color, Pixels{mSettings.line_width}, acmacs::surface::LineCap::Round);
        }
    };
    tree::iterate_leaf(mTree, draw_dash);

} // MappedAntigensDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
