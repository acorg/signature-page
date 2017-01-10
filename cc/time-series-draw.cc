#include "time-series-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "tree-draw.hh"
#include "coloring.hh"

// ----------------------------------------------------------------------

void TimeSeriesDraw::prepare()
{
    mBegin = mSettings.begin;
    mEnd = mSettings.end;
    std::map<Date, size_t> sequences_per_month;
    mTree.sequences_per_month(sequences_per_month);
    if (mBegin.empty()) {
        for (Date d = sequences_per_month.crbegin()->first; sequences_per_month.find(d) != sequences_per_month.end(); d.decrement_month()) {
            mBegin = d;
        }
    }
    if (mEnd.empty()) {
        for (auto ms = sequences_per_month.crbegin(); mEnd.empty() && ms != sequences_per_month.crend(); ++ms) {
            if (ms->second)
                mEnd = ms->first;
        }
    }

    std::cout << "Sequences per month:" << std::endl;
    for (const auto& e: sequences_per_month) {
        std::cout << "  " << e.first << " " << e.second << std::endl;
    }

    mNumberOfMonths = static_cast<size_t>(months_between_dates(mBegin, mEnd)) + 1;
    std::cout << "dates to show: " << mBegin << " .. " << mEnd << "  months: " << mNumberOfMonths << std::endl;

} // TimeSeriesDraw::prepare

// ----------------------------------------------------------------------

void TimeSeriesDraw::init_settings()
{

} // TimeSeriesDraw::init_settings

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw()
{
    // mSurface.border("green3", 1);

    const double month_width = mSurface.size().width / mNumberOfMonths;
    draw_labels(month_width);
    draw_month_separators(month_width);
    draw_dashes(month_width);
    draw_hz_section_lines();

} // TimeSeriesDraw::draw

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_labels(double month_width)
{
    const Size surface_size = mSurface.size();
    const double month_max_height = mSurface.text_size("May ", mSettings.label_size, mSettings.label_style).width;
    double x_bearing;
    const auto big_label_size = mSurface.text_size("May 99", mSettings.label_size, mSettings.label_style, &x_bearing);
    const auto text_up = (month_width - big_label_size.height) * 0.5;

    draw_labels_at_side({text_up, - big_label_size.width - x_bearing - mSettings.month_year_to_timeseries_gap}, month_width, month_max_height);
    draw_labels_at_side({text_up, surface_size.height + x_bearing + mSettings.month_year_to_timeseries_gap}, month_width, month_max_height);

} // TimeSeriesDraw::draw_labels

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_labels_at_side(const Location& aOrigin, double month_width, double month_max_height)
{
    Date current_month = mBegin;
    for (size_t month_no = 0; month_no < mNumberOfMonths; ++month_no, current_month.increment_month()) {
        const double left = aOrigin.x + month_no * month_width;
        mSurface.text({left, aOrigin.y}, current_month.month_3(), 0, mSettings.label_size, mSettings.label_style, M_PI_2);
        mSurface.text({left, aOrigin.y + month_max_height}, current_month.year_2(), 0, mSettings.label_size, mSettings.label_style, M_PI_2);
    }

} // TimeSeriesDraw::draw_labels_at_side

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_month_separators(double month_width)
{
    const double bottom = mSurface.size().height;
    for (size_t month_no = 0; month_no <= mNumberOfMonths; ++month_no) {
        const double left = month_no * month_width;
        mSurface.line({left, 0}, {left, bottom}, mSettings.month_separator_color, mSettings.month_separator_width);
    }

} // TimeSeriesDraw::draw_month_separators

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_dashes(double month_width)
{
    const double base_x = month_width * (1.0 - mSettings.dash_width) / 2;
    const Coloring& coloring = mTreeDraw.coloring();

    auto draw_dash = [&](const Node& aNode) {
        if (aNode.draw.shown) {
            const int month_no = months_between_dates(mBegin, Date(aNode.data.date()));
            if (month_no >= 0) {
                const Location a{base_x + month_width * month_no, aNode.draw.vertical_pos};
                mSurface.line(a, {a.x + month_width * mSettings.dash_width, a.y}, coloring.color(aNode), mSettings.dash_line_width, Surface::LineCap::Round);
            }
        }
    };
    tree::iterate_leaf(mTree, draw_dash);

} // TimeSeriesDraw::draw_dashes

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_hz_section_lines()
{
    double previous_vertical_pos = -1e-8;
    auto draw = [&](const Node& aNode) {
        if (aNode.draw.shown) {
            const auto& section_settings = mHzSections.sections[aNode.draw.hz_section_index];
            if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex) {
                double y = aNode.draw.vertical_pos;
                if (section_settings.show_line) {
                    y = (previous_vertical_pos + aNode.draw.vertical_pos) / 2;
                    mSurface.line({0, y}, {mSurface.size().width, y}, mHzSections.line_color, mHzSections.line_width);
                }
                if (section_settings.show_label_in_time_series) {
                    draw_hz_section_label(aNode.draw.hz_section_index, section_settings, y);
                }
            }
            previous_vertical_pos = aNode.draw.vertical_pos;
        }
    };
    tree::iterate_leaf(mTree, draw);

} // TimeSeriesDraw::draw_hz_section_lines

// ----------------------------------------------------------------------

void TimeSeriesDraw::draw_hz_section_label(size_t aSectionNo, const HzSection& aSection, double aY)
{
    if (aSection.show_map) {
        std::string label(1, 'A' + static_cast<char>(aSectionNo));
        const Size tsize = mSurface.text_size(label, mHzSections.ts_label_size, mHzSections.ts_label_style);
        mSurface.text({mSurface.size().width - tsize.width * 1.2, aY + tsize.height * 1.2}, label, mHzSections.ts_label_color, mHzSections.ts_label_size, mHzSections.ts_label_style);
    }

} // TimeSeriesDraw::draw_hz_section_label

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
