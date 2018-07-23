#include "acmacs-base/debug.hh"

#include "time-series-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "tree-draw.hh"
#include "coloring.hh"

// ----------------------------------------------------------------------

void TimeSeriesDraw::prepare()
{
    std::map<Date, size_t> sequences_per_month;
    mTree.sequences_per_month(sequences_per_month);
    if (!sequences_per_month.empty()) {
        if (mSettings.begin.empty()) {
            for (Date d = sequences_per_month.crbegin()->first; sequences_per_month.find(d) != sequences_per_month.end(); d.decrement_month()) {
                mSettings.begin = d;
            }
        }
        if (mSettings.end.empty()) {
            for (auto ms = sequences_per_month.crbegin(); mSettings.end.empty() && ms != sequences_per_month.crend(); ++ms) {
                if (ms->second)
                    mSettings.end = ms->first;
            }
        }

        std::cout << "INFO: Sequences per month:" << '\n';
        for (const auto& e: sequences_per_month) {
            std::cout << "  " << e.first << " " << e.second << '\n';
        }

        mNumberOfMonths = static_cast<size_t>(months_between_dates(Date{mSettings.begin}, Date{mSettings.end})) + 2;
        std::cout << "INFO: dates to show: " << mSettings.begin << " .. " << mSettings.end << "  months: " << mNumberOfMonths << DEBUG_LINE_FUNC << '\n';
    }
    else {
        std::cout << "WARNING: no dates found for sequences" << DEBUG_LINE_FUNC << '\n';
        mNumberOfMonths = 0;
    }

} // TimeSeriesDraw::prepare

// ----------------------------------------------------------------------

void TimeSeriesDraw::init_settings()
{

} // TimeSeriesDraw::init_settings

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw()
{
    if (mNumberOfMonths) {
          // mSurface.border("green3", 1);
        const double month_width = mSurface.viewport().size.width / mNumberOfMonths;
        draw_labels(month_width);
        draw_month_separators(month_width);
        draw_dashes(month_width);
        draw_hz_section_lines();
    }

} // TimeSeriesDraw::draw

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_labels(double month_width)
{
    const acmacs::Size& surface_size = mSurface.viewport().size;
    const double month_max_height = mSurface.text_size("May ", Pixels{mSettings.label_size}, mSettings.label_style).width;
    double x_bearing;
    const auto big_label_size = mSurface.text_size("May 99", Pixels{mSettings.label_size}, mSettings.label_style, &x_bearing);
    const auto text_up = (month_width - big_label_size.height) * 0.5;
    const double month_year_to_timeseries_gap = mSurface.convert(Pixels{mSettings.month_year_to_timeseries_gap}).value();

    draw_labels_at_side({text_up, - big_label_size.width - x_bearing - month_year_to_timeseries_gap}, month_width, month_max_height);
    draw_labels_at_side({text_up, surface_size.height + x_bearing + month_year_to_timeseries_gap}, month_width, month_max_height);

} // TimeSeriesDraw::draw_labels

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_labels_at_side(acmacs::Location2D aOrigin, double month_width, double month_max_height)
{
    try {
        Date current_month{mSettings.begin};
        for (size_t month_no = 0; month_no < mNumberOfMonths; ++month_no, current_month.increment_month()) {
            const double left = aOrigin.x() + month_no * month_width;
            mSurface.text({left, aOrigin.y()}, current_month.month_3(), 0, Pixels{mSettings.label_size}, mSettings.label_style, Rotation{M_PI_2});
            mSurface.text({left, aOrigin.y() + month_max_height}, current_month.year_2(), 0, Pixels{mSettings.label_size}, mSettings.label_style, Rotation{M_PI_2});
        }
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: " << err.what() << " (TimeSeriesDraw::draw_labels_at_side)\n";
    }

} // TimeSeriesDraw::draw_labels_at_side

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_month_separators(double month_width)
{
    const double bottom = mSurface.viewport().size.height;
    for (size_t month_no = 0; month_no <= mNumberOfMonths; ++month_no) {
        const double left = month_no * month_width;
        mSurface.line({left, 0}, {left, bottom}, mSettings.month_separator_color, Pixels{mSettings.month_separator_width});
    }

} // TimeSeriesDraw::draw_month_separators

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_dashes(double month_width)
{
    const double base_x = month_width * (1.0 - mSettings.dash_width) / 2;
    const Coloring& coloring = mTreeDraw.coloring();

    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown && !aNode.data.date().empty()) {
            try {
                const int month_no = months_between_dates(Date{mSettings.begin}, Date{aNode.data.date()});
                if (month_no >= 0) {
                    const acmacs::Location2D a{base_x + month_width * month_no, aNode.draw.vertical_pos};
                    mSurface.line(a, {a.x() + month_width * mSettings.dash_width, a.y()}, coloring.color(aNode), Pixels{mSettings.dash_line_width}, acmacs::surface::LineCap::Round);
                }
            }
            catch (std::exception& err) {
                std::cerr << "WARNING: " << err.what() << " (TimeSeriesDraw::draw_dashes) Date: " << aNode.data.date() << "\n";
            }
        }
    };

    try {
        tree::iterate_leaf(mTree, draw_dash);
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: " << err.what() << " (TimeSeriesDraw::draw_dashes)\n";
    }

} // TimeSeriesDraw::draw_dashes

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_hz_section_lines()
{
    double previous_vertical_pos = -1e-8;
    auto draw = [&](const Node& aNode) {
        if (aNode.draw.shown) {
            if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex) {
                const auto section_settings = mHzSections.sections[aNode.draw.hz_section_index];
                double y = aNode.draw.vertical_pos;
                if (section_settings.show_line) {
                    y = (previous_vertical_pos + aNode.draw.vertical_pos) / 2;
                    mSurface.line({0, y}, {mSurface.viewport().size.width, y}, mHzSections.line_color, Pixels{mHzSections.line_width});
                }
                if ((!mTreeMode || mHzSections.show_labels_in_time_series_in_tree_mode) && section_settings.show_label_in_time_series) {
                    draw_hz_section_label(aNode.draw.hz_section_index, y);
                }
            }
            previous_vertical_pos = aNode.draw.vertical_pos;
        }
    };
    tree::iterate_leaf(mTree, draw);

} // TimeSeriesDraw::draw_hz_section_lines

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_hz_section_label(size_t aSectionIndex, double aY)
{
    const auto section_settings = mHzSections.sections[aSectionIndex];
    if (section_settings.show && section_settings.show_map) {
        std::string label = mHzSections.node_refs[aSectionIndex].index; // (1, 'A' + static_cast<char>(aSectionNo));
        const acmacs::Size tsize = mSurface.text_size(label, Pixels{mHzSections.ts_label_size}, mHzSections.ts_label_style);
        mSurface.text({mSurface.viewport().size.width - tsize.width * 1.2, aY + tsize.height * 1.2}, label, mHzSections.ts_label_color, Pixels{mHzSections.ts_label_size}, mHzSections.ts_label_style);
    }

} // TimeSeriesDraw::draw_hz_section_label

// ----------------------------------------------------------------------

// void TimeSeriesDraw::hide_hz_section_labels_in_time_series()
// {
//     for (auto& section: mHzSections.sections)
//         section.show_label_in_time_series = false;

// } // TimeSeriesDraw::hide_hz_section_labels_in_time_series

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
