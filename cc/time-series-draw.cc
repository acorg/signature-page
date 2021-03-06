#include "acmacs-base/log.hh"
#include "signature-page/time-series-draw.hh"
#include "signature-page/tree.hh"
#include "signature-page/tree-draw.hh"
#include "signature-page/coloring.hh"

// ----------------------------------------------------------------------

void TimeSeriesDraw::prepare()
{
    std::map<date::year_month_day, size_t> sequences_per_month;
    mTree.sequences_per_month(sequences_per_month);
    if (!sequences_per_month.empty()) {
        std::cout << "INFO: Sequences per month:" << '\n';
        for (const auto& e: sequences_per_month) {
            std::cout << "  " << e.first << " " << e.second << '\n';
        }

        const auto today = date::today();
        auto end_of_ts = today;
        if (mSettings.end.empty()) {
            for (auto ms = sequences_per_month.crbegin(); ms != sequences_per_month.crend(); ++ms) {
                if (ms->second && ms->first <= today) {
                    end_of_ts = ms->first;
                    mSettings.end = date::display(date::end_of_month(ms->first));
                    break;
                }
            }
        }

        if (mSettings.begin.empty()) {
            for (const auto& entry : sequences_per_month) {
                if (entry.second && months_between_dates(entry.first, end_of_ts) < 25) {
                    mSettings.begin = date::display(entry.first);
                    break;
                }
            }
        }

        mNumberOfMonths = static_cast<size_t>(calendar_months_between_dates_inclusive(date::from_string(*mSettings.begin), date::from_string(*mSettings.end)));
        std::cout << "INFO: dates to show: " << mSettings.begin << " .. " << mSettings.end << "  months: " << mNumberOfMonths << '\n';
    }
    else {
        std::cout << "WARNING: no dates found for sequences" << '\n';
        mNumberOfMonths = 0;
    }

} // TimeSeriesDraw::prepare

// ----------------------------------------------------------------------

std::vector<std::string> TimeSeriesDraw::all_months() const
{
    std::vector<std::string> result;
    auto current_month{date::from_string(*mSettings.begin)};
    for (size_t month_no = 0; month_no < mNumberOfMonths; ++month_no, date::increment_month(current_month))
        result.push_back(date::year4_month2(current_month));
    return result;

} // TimeSeriesDraw::all_months

// ----------------------------------------------------------------------

void TimeSeriesDraw::init_settings(const SettingsInitializer& /*settings_initilizer*/)
{

} // TimeSeriesDraw::init_settings

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw()
{
    if (mNumberOfMonths) {
          // mSurface.border("green3", 1);
        const double month_width = mSurface.viewport().size.width / static_cast<double>(mNumberOfMonths);
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
    const double month_max_height = mSurface.text_size("May ", Pixels{*mSettings.label_size}, mSettings.label_style).width;
    double x_bearing;
    const auto big_label_size = mSurface.text_size("May 99", Pixels{*mSettings.label_size}, mSettings.label_style, &x_bearing);
    const auto text_up = (month_width - big_label_size.height) * 0.5;
    const double month_year_to_timeseries_gap = mSurface.convert(Pixels{*mSettings.month_year_to_timeseries_gap}).value();

    draw_labels_at_side({text_up, - big_label_size.width - x_bearing - 2 /*month_year_to_timeseries_gap*/}, month_width, month_max_height);
    draw_labels_at_side({text_up, surface_size.height + x_bearing + month_year_to_timeseries_gap}, month_width, month_max_height);

} // TimeSeriesDraw::draw_labels

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_labels_at_side(const acmacs::PointCoordinates& aOrigin, double month_width, double month_max_height)
{
    try {
        auto current_month{date::from_string(*mSettings.begin)};
        for (size_t month_no = 0; month_no < mNumberOfMonths; ++month_no, date::increment_month(current_month)) {
            const double left = aOrigin.x() + static_cast<double>(month_no) * month_width;
            mSurface.text({left, aOrigin.y()}, date::month_3(current_month), 0, Pixels{*mSettings.label_size}, mSettings.label_style, Rotation{M_PI_2});
            mSurface.text({left, aOrigin.y() + month_max_height}, date::year_2(current_month), 0, Pixels{*mSettings.label_size}, mSettings.label_style, Rotation{M_PI_2});
        }
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: " << err.what() << " (TimeSeriesDraw::draw_labels_at_side)\n";
    }

} // TimeSeriesDraw::draw_labels_at_side

// ----------------------------------------------------------------------

// for tracked antigens in antigenic maps colored by date
void TimeSeriesDraw::draw_color_scale(const std::map<std::string, Color, std::less<>>& aTrackedAntigenColorByMonth)
{
    const acmacs::Size& surface_size = mSurface.viewport().size;
    const double month_width = mSurface.viewport().size.width / static_cast<double>(mNumberOfMonths);
    const auto top = surface_size.height;
    auto current_month{date::from_string(*mSettings.begin)};
    for (size_t month_no = 0; month_no < mNumberOfMonths; ++month_no, date::increment_month(current_month)) {
        const auto left = static_cast<double>(month_no) * month_width;
        mSurface.rectangle_filled({left, top}, acmacs::Size{10, 10}, PINK, Pixels{0}, aTrackedAntigenColorByMonth.find(date::year4_month2(current_month))->second);
    }

} // TimeSeriesDraw::draw_color_scale

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_month_separators(double month_width)
{
    // const auto first_month_no = static_cast<unsigned>(date::from_string(*mSettings.begin).month());
    // const auto thick_separator_base  = (13 - first_month_no) % 12;

    const double bottom = mSurface.viewport().size.height;
    for (size_t month_no = 0; month_no <= mNumberOfMonths; ++month_no) {
        const double left = static_cast<double>(month_no) * month_width;
        const double thickness = 1.0; // (month_no % 12) == thick_separator_base ? 2.0 : 1.0;
        mSurface.line({left, 0}, {left, bottom}, mSettings.month_separator_color, Pixels{mSettings.month_separator_width * thickness});
    }

} // TimeSeriesDraw::draw_month_separators

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_dashes(double month_width)
{
    const double base_x = month_width * (1.0 - mSettings.dash_width) / 2;
    const Coloring& coloring = mTreeDraw.coloring();

    const auto begin{date::from_string(*mSettings.begin)}, end{date::from_string(*mSettings.end)};
    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown && !aNode.data.date().empty()) {
            if (const auto node_date_s = aNode.data.date(); node_date_s.size() > 3 && node_date_s.substr(node_date_s.size() - 3) != "-00") { // ignore incomplete dates
                try {
                    if (const auto node_date{date::from_string(node_date_s)}; node_date >= begin && node_date <= end) {
                        const int month_no = date::months_between_dates(begin, node_date);
                        const acmacs::PointCoordinates a(base_x + month_width * month_no, aNode.draw.vertical_pos);
                        mSurface.line(a, {a.x() + month_width * mSettings.dash_width, a.y()}, coloring.color(aNode), Pixels{*mSettings.dash_line_width}, acmacs::surface::LineCap::Round);
                    }
                }
                catch (std::exception& err) {
                    if (aNode.data.date().size() > 4)
                        std::cerr << "WARNING: " << err.what() << " (TimeSeriesDraw::draw_dashes) Date: " << aNode.data.date() << "\n";
                }
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
                if (section_settings->show_line) {
                    y = (previous_vertical_pos + aNode.draw.vertical_pos) / 2;
                    mSurface.line({0, y}, {mSurface.viewport().size.width, y}, mHzSections.line_color, Pixels{*mHzSections.line_width});
                }
                if ((!mTreeMode || mHzSections.show_labels_in_time_series_in_tree_mode) && section_settings->show_label_in_time_series) {
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
    if (section_settings->show && section_settings->show_map) {
        std::string label = mHzSections.node_refs[aSectionIndex].index; // (1, 'A' + static_cast<char>(aSectionNo));
        const acmacs::Size tsize = mSurface.text_size(label, Pixels{*mHzSections.ts_label_size}, mHzSections.ts_label_style);
        mSurface.text({mSurface.viewport().size.width - tsize.width * 1.2, aY + tsize.height * 1.2}, label, mHzSections.ts_label_color, Pixels{*mHzSections.ts_label_size}, mHzSections.ts_label_style);
    }

} // TimeSeriesDraw::draw_hz_section_label

// ----------------------------------------------------------------------

// void TimeSeriesDraw::hide_hz_section_labels_in_time_series()
// {
//     for (auto& section: mHzSections.sections)
//         section.show_label_in_time_series = false;

// } // TimeSeriesDraw::hide_hz_section_labels_in_time_series

// ----------------------------------------------------------------------

void TimeSeriesDrawSettings::remove_for_tree_settings()
{
    label_style.remove();

} // TimeSeriesDrawSettings::remove_for_tree_settings

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
