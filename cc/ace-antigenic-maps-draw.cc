#include <set>

#include "acmacs-base/enumerate.hh"
#include "acmacs-map-draw/vaccine-matcher.hh"
#include "acmacs-map-draw/mod-applicator.hh"
#include "ace-antigenic-maps-draw.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"

// ----------------------------------------------------------------------

void ChartDrawInterface::init_settings()
{

} // ChartDrawInterface::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::make_layout()
{
    if (static_cast<std::string>(settings().layout) == "labelled_grid") {
        layout(new LabelledGrid(*this));
    }
    else {
        throw std::runtime_error("Unrecognized antigenic maps layout: " + static_cast<std::string>(settings().layout));
    }

} // AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_apply_mods()
{
      // std::cerr << "DEBUG: AntigenicMapsLayoutDrawAce::prepare_apply_mods\n";
    settings().mods.for_each([this](const auto& mod, size_t /*mod_no*/) {
        try {
            if (!mod.name.is_set_or_has_default()) {
                if (!mod.name_commented.is_set_or_has_default())
                    throw std::runtime_error("mod N not set, and there is no ?N");
            }
            else if (mod.name == "rotate_degrees") {
                chart_draw().rotate(mod.angle.get_or(0.0) * M_PI / 180.0);
            }
            else if (mod.name == "rotate_radians") {
                chart_draw().rotate(mod.angle.get_or(0.0));
            }
            else if (mod.name == "rotate") {
                if (const double degrees = mod.degrees.get_or(0.0); !float_zero(degrees))
                    chart_draw().rotate(degrees * M_PI / 180.0);
                else if (const double radians = mod.radians.get_or(0.0); !float_zero(radians))
                    chart_draw().rotate(radians);
            }
            else if (mod.name == "flip") {
                if (mod.direction.is_set()) {
                    if (mod.direction == "ns")
                        chart_draw().flip(1, 0);
                    else if (mod.direction == "ew")
                        chart_draw().flip(0, 1);
                    else
                        std::cerr << "ERROR: unrecognized flip value: " << mod << DEBUG_LINE_FUNC << '\n';
                }
                else if (mod.value.is_set()) {
                    chart_draw().flip(mod.value[0], mod.value[1]);
                }
                else {
                    std::cerr << "ERROR: unrecognized flip mod: " << mod << DEBUG_LINE_FUNC << '\n';
                }
            }
            else if (mod.name == "viewport") {
                chart_draw().viewport(mod.get_viewport(chart_draw().calculate_viewport(false)));
            }
            else if (mod.name == "title") {
                chart_draw()
                    .title()
                    .text_size(mod.text_size.get_or(12.0))
                    .text_color(mod.text_color.get_or(BLACK))
                    .weight(std::string(mod.weight.get_or("normal")))
                    .slant(std::string(mod.slant.get_or("normal")))
                    .font_family(std::string(mod.font_family.get_or("san serif")))
                    .background("transparent")
                    .border_width(0)
                    .padding(mod.padding.get_or(3.0))
                    .offset(mod.offset)
                    .remove_all_lines();
            }
            // else if (mod.name == "tracked_antigen") {
            //     mDrawTrackedAntigen.color(mod.get("color", "green3"));
            // }
        }
        catch (std::exception&) {
            std::cerr << "WARNING: MOD: " << mod << DEBUG_LINE_FUNC << '\n';
            throw;
        }
    });
    chart_draw().calculate_viewport();

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections()
{
    chart_draw().mark_egg_antigens();
    chart_draw().mark_reassortant_antigens();
    settings().mods.for_each([this](const auto& mod, size_t /*mod_no*/) {
        if (mod.name.is_set_or_has_default()) {
            if (mod.name == "point_scale") {
                chart_draw().scale_points(mod.scale.get_or(1.0), mod.outline_scale.get_or(1.0));
            }
            if (mod.name == "reference_antigens") {
                acmacs::PointStyle ref_antigen_style;
                ref_antigen_style.size = Pixels{mod.size.get_or(5.0)};
                ref_antigen_style.fill = mod.fill.get_or(TRANSPARENT);
                ref_antigen_style.outline = mod.outline.get_or("grey88");
                ref_antigen_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
                chart_draw().modify(chart().antigens()->reference_indexes(), ref_antigen_style);
            }
            else if (mod.name == "test_antigens") {
                acmacs::PointStyle test_antigen_style;
                test_antigen_style.size = Pixels{mod.size.get_or(3.0)};
                test_antigen_style.fill = mod.fill.get_or("grey88");
                test_antigen_style.outline = mod.outline.get_or("grey88");
                test_antigen_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
                chart_draw().modify(chart().antigens()->test_indexes(), test_antigen_style);
            }
        }
    });

} // AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_drawing_chart(size_t aSectionIndex, std::string map_letter, bool report_antigens_in_hz_sections)
{
    // reset tracked antigens and sera shown on the previous map
    chart_draw().remove_serum_circles();
    for (size_t serum_index = chart().number_of_antigens(); serum_index < (chart().number_of_antigens() + chart().number_of_sera()); ++serum_index)
        chart_draw().remove_label(serum_index);

    settings().mods.for_each([this](const auto& mod, size_t /*mod_no*/) {
        if (mod.name.is_set_or_has_default()) {
            if (mod.name == "sera") {
                acmacs::PointStyle serum_style;
                serum_style.size = Pixels{mod.size.get_or(5.0)};
                serum_style.fill = mod.fill.get_or(TRANSPARENT);
                serum_style.outline = mod.outline.get_or("grey88");
                serum_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
                chart_draw().modify_all_sera(serum_style, PointDrawingOrder::Lower);
            }
            else if (mod.name == "sequenced_antigens") { // mark sequenced antigens (removes old tracked antigens marking)
                std::vector<size_t> sequenced_indices(sequenced_antigens().size());
                std::transform(sequenced_antigens().begin(), sequenced_antigens().end(), sequenced_indices.begin(), [](const auto& src) -> size_t { return src.first; });
                acmacs::PointStyle sequenced_antigen_style;
                sequenced_antigen_style.size = Pixels{mod.size.get_or(3.0)};
                sequenced_antigen_style.fill = mod.fill.get_or("grey88");
                sequenced_antigen_style.outline = mod.outline.get_or(WHITE);
                sequenced_antigen_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
                chart_draw().modify(acmacs::chart::Indexes{sequenced_indices}, sequenced_antigen_style);
            }
        }
    });

    settings().mods.for_each([this, aSectionIndex, report_antigens_in_hz_sections, map_letter](const AntigenicMapMod& mod, size_t /*mod_no*/) {
        if (mod.name.is_set_or_has_default()) {
            if (mod.name == "tracked_antigens") {
                acmacs::PointStyle tracked_antigen_style;
                tracked_antigen_style.size = Pixels{mod.size.get_or(5.0)};
                tracked_antigen_style.outline = mod.outline.get_or(WHITE);
                tracked_antigen_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
                if (const auto& fill_raw = mod.fill.get(); fill_raw == "by_date") {
                    for (const auto& [month, tracked] : tracked_antigens_per_month(aSectionIndex)) {
                        // std::cerr << "DEBUG: tracked_antigens by month " << month <<  ' ' << tracked << '\n';
                        if (!tracked->empty()) {
                            tracked_antigen_style.fill = tracked_antigen_color_by_month(month);
                            chart_draw().modify(tracked, tracked_antigen_style, PointDrawingOrder::Raise);
                            if (mod.report.get_or(false)) {
                                std::cout << "tracked_antigens by month " << month << ' ' << tracked_antigen_style.fill << '\n';
                                for (auto ag_no : tracked) {
                                    auto antigen = chart().antigen(ag_no);
                                    std::cout << "    " << ag_no << ' ' << antigen->full_name() << ' ' << *antigen->date() << '\n';
                                }
                            }
                        }
                    }
                }
                else {
                    tracked_antigen_style.fill = mod.fill.get_or("grey63");
                    chart_draw().modify(tracked_antigens(aSectionIndex, report_antigens_in_hz_sections), tracked_antigen_style, PointDrawingOrder::Raise);
                }
            }
            else if (mod.name == "tracked_sera") {
                const auto tracked_indices = tracked_sera(aSectionIndex);
                std::cout << "INFO: tracked_sera: " << tracked_indices << '\n';
                for (auto [serum_index, ignored] : tracked_indices)
                    make_tracked_serum(serum_index, Pixels{mod.size.get_or(5.0)}, mod.outline.get_or("black"), Pixels{mod.outline_width.get_or(0.5)}, *mod.label);
            }
            else if (mod.name == "tracked_serum_circles") {
                tracked_serum_circles(mod, aSectionIndex);
            }
            else if (mod.name == "serum_circle") {
                serum_circle(mod, map_letter, aSectionIndex);
            }
            else if (mod.name == "vaccines") {
                throw std::runtime_error("obsolete mod \"vaccines\" (use {\"N\":\"antigens\", \"select\": {\"vaccine\": }}): " + mod.to_json());
                // mark_vaccines(mod);
            }
            else if (mod.name == "antigens") {
                mark_antigens(mod);
            }
            else if (mod.name == "antigens_old") {
                throw std::runtime_error("obsolete mod \"antigens_old\" (use {\"N\":\"antigens\"}): " + mod.to_json());
                // mark_antigens_old(mod);
            }
        }
    });

    // marked antigens
    // tracked_antigen_colored_by_clade

    std::string title = hz_sections().node_refs[aSectionIndex].index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    auto section = hz_sections().sections[aSectionIndex];
    if (!section->label.empty())
        title += " " + static_cast<std::string>(section->label);
    // std::cerr << "DEBUG: map title: " << title << '\n';
    chart_draw().title().remove_all_lines().show(true).add_line(title);

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

// ----------------------------------------------------------------------

Color AntigenicMapsLayoutDrawAce::tracked_antigen_color_by_month(std::string_view month) const
{
    if (mTrackedAntigenColorByMonth.empty()) {
        const auto& ts = antigenic_maps_draw().time_series();
        const auto months = ts.all_months();
        const auto total_colors = months.size() - 3; // [-3 for four last months in yellow]
        mTooOldTrackedAntigenColor = Color::perceptually_uniform_heatmap(total_colors, 0);
        mTooRecentTrackedAntigenColor = Color::perceptually_uniform_heatmap(total_colors, total_colors - 1);
        for (const auto [no, month] : acmacs::enumerate(months))
            mTrackedAntigenColorByMonth[month] = Color::perceptually_uniform_heatmap(total_colors, no /* + 1 */);
    }
    if (const auto found = mTrackedAntigenColorByMonth.find(month); found != mTrackedAntigenColorByMonth.end())
        return found->second;
    else if (mTrackedAntigenColorByMonth.empty() || month > mTrackedAntigenColorByMonth.rbegin()->first)
        return mTooRecentTrackedAntigenColor;
    else
        return mTooOldTrackedAntigenColor;

} // AntigenicMapsLayoutDrawAce::tracked_antigen_color_by_month

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::make_tracked_serum(size_t serum_index, Pixels size, Color outline, Pixels outline_width, const LabelSettings& label_data)
{
    acmacs::PointStyle tracked_serum_style;
    tracked_serum_style.size = size;
    tracked_serum_style.outline = outline;
    tracked_serum_style.outline_width = outline_width;
    chart_draw().modify_serum(serum_index, tracked_serum_style, PointDrawingOrder::Raise);
    if (label_data.offset.is_set() && label_data.show.get_or(true)) {
        auto& label = chart_draw().add_label(chart().number_of_antigens() + serum_index);
        label.offset(label_data.offset);

        rjson::for_each(label_data.get(), [&label, &label_data, this, serum_index](const std::string& field_name, const rjson::value& item_value) {
            if (field_name == "size")
                label.size(static_cast<double>(item_value));
            else if (field_name == "color")
                label.color(Color(item_value.to_string_view()));
            else if (field_name == "font_family")
                label.font_family(item_value.to_string_view());
            else if (field_name == "name_type") {
                auto serum = chart().serum(serum_index);
                const std::string_view name_type{item_value.to_string_view()};
                if (name_type == "abbreviated")
                    label.display_name(serum->abbreviated_name());
                else if (name_type == "abbreviated_name_with_serum_id")
                    label.display_name(serum->abbreviated_name_with_serum_id());
                else {
                    if (name_type != "full")
                        std::cerr << "WARNING: unrecognized \"name_type\" for label for serum: " << label_data << '\n';
                    label.display_name(serum->full_name());
                }
            }
            else if (field_name == "display_name")
                label.display_name(item_value.to_string_view());
            else if (field_name == "slant")
                label.slant(item_value.to_string_view());
            else if (field_name == "weight")
                label.weight(item_value.to_string_view());
            else if (field_name == "offset") {
                label.offset({static_cast<double>(item_value[0]), static_cast<double>(item_value[1])});
            }
            else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
                std::cerr << "WARNING: make_tracked_serum label: unrecognized key \"" << field_name << '"' << '\n';
        });
    }

} // AntigenicMapsLayoutDrawAce::make_tracked_serum

// ----------------------------------------------------------------------

acmacs::chart::PointIndexList AntigenicMapsLayoutDrawAce::tracked_antigens(size_t aSectionIndex, bool report_antigens_in_hz_sections) const
{
    acmacs::chart::PointIndexList tracked_indices;
    for (const auto& sequenced_section: sequenced_antigens()) {
        if (sequenced_section.second == aSectionIndex) {
            tracked_indices.insert(sequenced_section.first);
            if (report_antigens_in_hz_sections) {
                std::cout << aSectionIndex << ' ' << sequenced_section.first << ' ' << chart().antigen(sequenced_section.first)->full_name() << '\n';
            }
        }
    }
    return tracked_indices;

} // AntigenicMapsLayoutDrawAce::tracked_antigens

// ----------------------------------------------------------------------

std::map<std::string, acmacs::chart::PointIndexList> AntigenicMapsLayoutDrawAce::tracked_antigens_per_month(size_t aSectionIndex) const
{
    std::map<std::string, acmacs::chart::PointIndexList> tracked_indices;
    for (const auto& sequenced_section: sequenced_antigens()) {
        if (sequenced_section.second == aSectionIndex) {
            auto antigen = chart().antigen(sequenced_section.first);
            if (const auto date = antigen->date(); !date.empty() && date.size() >= 7) {
                const auto month = date->substr(0, 7);
                tracked_indices[month].insert(sequenced_section.first);
                // std::cerr << "DEBUG: " << aSectionIndex << ' ' << month << '\n';
            }
        }
    }
    return tracked_indices;

} // AntigenicMapsLayoutDrawAce::tracked_antigens_per_month

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::find_homologous_antigens_for_sera() const
{
    if (!mHomologousAntigenForSeraFound) {
        chart().set_homologous(acmacs::chart::find_homologous::relaxed_strict);
        mHomologousAntigenForSeraFound = true;
    }

} // AntigenicMapsLayoutDrawAce::find_homologous_antigens_for_sera

// ----------------------------------------------------------------------

std::map<size_t, acmacs::chart::PointIndexList> AntigenicMapsLayoutDrawAce::tracked_sera(size_t aSectionIndex) const
{
    find_homologous_antigens_for_sera();

    const auto tracked_antigen_indices = tracked_antigens(aSectionIndex, false);
    std::set<std::string> tracked_antigen_names;
    for (auto ag_no : tracked_antigen_indices)
        tracked_antigen_names.insert(*chart().antigen(ag_no)->name());

    // std::cerr << "DEBUG: section " << aSectionIndex << " tracked antigen names " << tracked_antigen_names.size() << '\n';
    // for (const auto& name : tracked_antigen_names)
    //     std::cerr << "    " << name << '\n';

    // std::cerr << "DEBUG: section " << aSectionIndex << " tracked antigens " << tracked_antigen_indices << '\n';
    // std::cerr << "DEBUG: section " << aSectionIndex << " tracked antigens " << tracked_antigen_indices.size() << '\n';
    // for (auto ag_no : tracked_antigen_indices)
    //     std::cerr << "    " << ag_no << ' ' << chart().antigen(ag_no)->full_name() << '\n';

    std::map<size_t, acmacs::chart::PointIndexList> tracked_indices;
    for (size_t serum_no = 0; serum_no < chart().number_of_sera(); ++serum_no) {
        if (tracked_antigen_names.find(*chart().serum(serum_no)->name()) != tracked_antigen_names.end()) {
            if (const auto homologous_antigens_for_serum = chart().serum(serum_no)->homologous_antigens(); !homologous_antigens_for_serum->empty())
                tracked_indices[serum_no] = homologous_antigens_for_serum;
        }
    }
    return tracked_indices;

} // AntigenicMapsLayoutDrawAce::tracked_sera

// ----------------------------------------------------------------------

// BEFORE 2019-09-23
// std::map<size_t, acmacs::chart::PointIndexList> AntigenicMapsLayoutDrawAce::tracked_sera(size_t aSectionIndex) const
// {
//     find_homologous_antigens_for_sera();

//     const auto tracked_antigen_indices = tracked_antigens(aSectionIndex, false);
//     std::cerr << "DEBUG: section " << aSectionIndex << " tracked antigens " << tracked_antigen_indices << '\n';
//     // std::cerr << "DEBUG: section " << aSectionIndex << " tracked antigens " << tracked_antigen_indices.size() << '\n';
//     // for (auto ag_no : tracked_antigen_indices)
//     //     std::cerr << "    " << ag_no << ' ' << chart().antigen(ag_no)->full_name() << '\n';

//     std::map<size_t, acmacs::chart::PointIndexList> tracked_indices;
//     for (size_t serum_no = 0; serum_no < chart().number_of_sera(); ++serum_no) {
//         const auto homologous_antigens_for_serum = chart().serum(serum_no)->homologous_antigens();

//         if (homologous_antigens_for_serum.empty())
//             std::cerr << "DEBUG: no homologous antigens for serum " << serum_no <<  ' ' << chart().serum(serum_no)->full_name_with_passage() << " for section " << aSectionIndex << '\n';
//         else
//             std::cerr << "DEBUG: section " << aSectionIndex << " serum: " << serum_no <<  ' ' << chart().serum(serum_no)->full_name_with_passage()
//                       << "\n    homologous antigens: " << homologous_antigens_for_serum
//                       // << "\n    tracked_antigen_indices: " << tracked_antigen_indices
//                       << '\n';

//         for (size_t antigen_no: homologous_antigens_for_serum) {
//             if (tracked_antigen_indices.contains(antigen_no)) {
//                 tracked_indices[serum_no] = homologous_antigens_for_serum;
//                 break;
//             }
//         }
//     }
//     return tracked_indices;

// } // AntigenicMapsLayoutDrawAce::tracked_sera

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_serum_circles(const AntigenicMapMod& mod, size_t aSectionIndex)
{
    find_homologous_antigens_for_sera();
    for (auto serum_antigens: tracked_sera(aSectionIndex)) {
        if (!mod.ignore_sera.is_set() || !mod.ignore_sera.contains(serum_antigens.first))
            make_serum_circle(mod, serum_antigens.first, serum_antigens.second);
    }

} // AntigenicMapsLayoutDrawAce::tracked_serum_circles

// ----------------------------------------------------------------------

Color AntigenicMapsLayoutDrawAce::serum_circle_outline(const AntigenicMapMod& mod, bool egg, bool forced_radius) const
{
    Color outline{BLACK};
    if (const auto& outline_raw = mod.outline.get(); outline_raw == "passage") {
        if (egg)
            outline = forced_radius ? Color{0xFFA0A0} : RED;
        else
            outline = forced_radius ? Color{0xA0A0FF} : BLUE;
    }
    else
        outline = mod.outline.get_or(BLACK);
    return outline;

} // AntigenicMapsLayoutDrawAce::serum_circle_outline

// ----------------------------------------------------------------------

// returns if circle shown
bool AntigenicMapsLayoutDrawAce::make_serum_circle(const AntigenicMapMod& mod, size_t serum_no, const acmacs::chart::PointIndexList& homologous_antigens)
{
    const auto report_homologous_antigens = [serum_no,&homologous_antigens](std::ostream& out, const auto& chart) {
        auto titers = chart.titers();
        for (auto ag_no : homologous_antigens)
            out << " [" << ag_no << ' ' << chart.antigen(ag_no)->full_name() << " : " << *titers->titer(ag_no, serum_no) << ']';
    };

    std::vector<double> radii(homologous_antigens->size());
    std::transform(homologous_antigens.begin(), homologous_antigens.end(), radii.begin(), [&](size_t ag_no) -> double {
        const auto circle_data = chart().serum_circle_radius_empirical(ag_no, serum_no, 0);
        return circle_data.valid() ? circle_data.radius() : -1.0;
    });
    std::sort(radii.begin(), radii.end());

    auto serum = chart().serum(serum_no);
    double radius{-1.0};
    Color outline_color{PINK};
    if (const auto radius_p = std::find_if(radii.begin(), radii.end(), [](double r) -> bool { return r >= 0.0; }); radius_p != radii.end()) {
        radius = *radius_p;
        outline_color = serum_circle_outline(mod, serum->is_egg(), false);
    }
    else if (mod.serum_circle_radius_if_not_calculated.is_set()) {
        radius = mod.serum_circle_radius_if_not_calculated.get_or(-1);
        std::cerr << "INFO: forced radius: " << radius << " (cannot be calculated)\n";
        outline_color = serum_circle_outline(mod, serum->is_egg(), true);
    }
    if (radius > 0) {
        std::cout << "INFO: serum circle for " << serum_no << ' ' << serum->full_name() << " passage:" << *serum->passage() << " passage-type:" << serum->passage_type() << " radius: " << radius << " antigens:";
        report_homologous_antigens(std::cout, chart());
        std::cout << '\n';
        auto& serum_circle = chart_draw().serum_circle(serum_no, Scaled{radius});
        serum_circle.fill(mod.fill.get_or(TRANSPARENT)).outline(outline_color, mod.outline_width.get_or(1.0)).radius_line(mod.radius_line.get_or(TRANSPARENT), mod.radius_line_width.get_or(1.0));
        //.angles(mod.get["angle_degrees"][0] * math.pi / 180.0, mod.get["angle_degrees"][1] * math.pi / 180.0);
        const auto radius_line_dash = mod.radius_line_dash.get_or("");
        if (radius_line_dash.empty() || radius_line_dash == "nodash")
            serum_circle.radius_line_no_dash();
        else if (radius_line_dash == "dash1")
            serum_circle.radius_line_dash1();
        else if (radius_line_dash == "dash2")
            serum_circle.radius_line_dash2();
    }
    else {
        std::cerr << "WARNING: no serum circle for " << serum_no << ' ' << serum->full_name();
        if (homologous_antigens->empty()) {
            std::cerr << " no homologous antigens\n";
        }
        else {
            std::cerr << " antigens and titers:";
            report_homologous_antigens(std::cerr, chart());
            std::cerr << '\n';
        }
    }
    return radius > 0;

} // AntigenicMapsLayoutDrawAce::make_serum_circle

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::serum_circle(const AntigenicMapMod& mod, std::string map_letter, size_t /*aSectionIndex*/)
{
    if (const auto map = mod.map.get_or("A"); map == map_letter) {
        find_homologous_antigens_for_sera();
        if (mod.serum.empty())
            throw std::runtime_error("invalid mod (\"serum\" not found): " + mod.to_json());
        auto serum_index = chart().sera()->find_by_full_name(*mod.serum);
        if (!serum_index)
            throw std::runtime_error("serum not found: " + mod.to_json());
        const auto homologous_antigens_for_serum = chart().serum(*serum_index)->homologous_antigens();
        if (make_serum_circle(mod, *serum_index, homologous_antigens_for_serum)) {
            // const auto serum_outline = mod.serum_outline.get_or(serum_circle_outline(mod, chart().serum(*serum_index)->passage().is_egg(), false));
            const auto serum_outline = serum_circle_outline(mod, chart().serum(*serum_index)->is_egg(), false);
            // std::cerr << "DEBUG: serum_outline " << *serum_index << " " << chart().serum(*serum_index)->full_name() << " : " << serum_outline << '\n';
            make_tracked_serum(*serum_index, Pixels{mod.serum_size.get_or(5.0)}, serum_outline, Pixels{mod.serum_outline_width.get_or(0.5)}, *mod.label);
        }
    }

} // AntigenicMapsLayoutDrawAce::serum_circle

// ----------------------------------------------------------------------

// Obsolete: use "N":"antigens", "select": {"vaccine": }
// void AntigenicMapsLayoutDrawAce::mark_vaccines(const AntigenicMapMod& vaccine_mod)
// {
//     try {
//         const auto& chart = chart_draw().chart();
//         Vaccines vaccs{chart};
//         rjson::for_each(vaccine_mod.mods(), [&vaccs, this](const rjson::value& mod) {
//             const std::string type(rjson::get_or(mod, "type", "")), passage(rjson::get_or(mod, "passage", "")), name(rjson::get_or(mod, "name", ""));
//             VaccineMatcher matcher(vaccs, VaccineMatchData{}.name(name).type(type).passage(passage));
//             // std::cerr << matcher.report(2) << '\n';
//             rjson::for_each(mod, [&matcher, this](const std::string& field_name, const rjson::value& item_value) {
//                 if (field_name == "size")
//                     matcher.size(item_value);
//                 else if (field_name == "fill")
//                     matcher.fill(item_value);
//                 else if (field_name == "outline")
//                     matcher.outline(item_value);
//                 else if (field_name == "outline_width")
//                     matcher.outline_width(item_value);
//                 else if (field_name == "aspect")
//                     matcher.aspect(item_value);
//                 else if (field_name == "rotation")
//                     matcher.rotation(item_value);
//                 else if (field_name == "no")
//                     matcher.no(item_value); // size_t
//                 else if (field_name == "show") {
//                     const bool show = static_cast<bool>(item_value);
//                     matcher.show(show);
//                     if (!show)
//                         matcher.hide_label(chart_draw());
//                 }
//                 else if (field_name == "label")
//                     add_label(std::shared_ptr<VaccineMatcherLabel>{matcher.label(chart_draw())}, item_value);
//                 else if (field_name != "type" && field_name != "passage" && field_name != "name" && (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?')))
//                     std::cerr << "WARNING: mark_vaccines: unrecognized key \"" << field_name << '"' << '\n';
//             });
//         });
//         // std::cerr << "DEBUG: Vaccines:" << '\n' << vaccs.report(2) << '\n';
//         vaccs.plot(chart_draw());
//     }
//     catch (std::exception&) {
//         std::cerr << "WARNING: cannot mark vaccines: invalid vaccine settings: " << vaccine_mod << '\n';
//     }

// } // AntigenicMapsLayoutDrawAce::mark_vaccines

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_antigens(const AntigenicMapMod& mod)
{
      // std::cerr << "DEBUG: mark_antigens " << mod.get() << DEBUG_LINE_FUNC << '\n';
    ModAntigens applicator(mod.get());
    applicator.apply(chart_draw(), rjson::value{});

} // AntigenicMapsLayoutDrawAce::mark_antigens

// ----------------------------------------------------------------------

// void AntigenicMapsLayoutDrawAce::mark_antigens_old(const AntigenicMapMod& mod)
// {
//     const auto& select = mod.get_or_empty_object("select");
//     try {
//         if (const auto& index = select["index"]; !index.is_null()) {
//             acmacs::PointStyle antigen_style;
//             antigen_style.size = Pixels{mod.size.get_or(5.0)};
//             antigen_style.fill = mod.fill.get_or(PINK);
//             antigen_style.outline = mod.outline.get_or(WHITE);
//             antigen_style.outline_width = Pixels{mod.outline_width.get_or(0.5)};
//             chart_draw().modify(static_cast<size_t>(index), antigen_style, mod.raise_.get_or(true) ? PointDrawingOrder::Raise : PointDrawingOrder::NoChange);
//             if (const auto& label_data = mod.label; label_data.is_set()) {
//                 auto& label = chart_draw().add_label(index);
//                 label.size(label_data->size.get_or(9.0));
//                 label.offset = label_data->offset.get_or({0, 1});
//                 if (const auto& display_name = label_data->display_name; display_name.is_set())
//                     label.display_name(display_name);
//             }
//         }
//         else
//             std::cerr << "WARNING: Antigens: no index in " << select << '\n';
//     }
//     catch (std::exception& err) {
//         std::cerr << "WARNING: Antigens: error: " << err.what() << " in " << select << '\n';
//     }

// } // AntigenicMapsLayoutDrawAce::mark_antigens_old

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::add_label(std::shared_ptr<VaccineMatcherLabel> label, const rjson::value& data)
{
    rjson::for_each(data, [&label](const std::string& field_name, const rjson::value& item_value) {
        if (field_name == "size")
            label->size(static_cast<double>(item_value));
        else if (field_name == "color")
            label->color(item_value.to_string_view());
        else if (field_name == "font_family")
            label->font_family(item_value.to_string_view());
        else if (field_name == "name_type")
            label->name_type(static_cast<std::string>(item_value));
        else if (field_name == "slant")
            label->slant(item_value.to_string_view());
        else if (field_name == "weight")
            label->weight(item_value.to_string_view());
        else if (field_name == "offset")
            label->offset({static_cast<double>(item_value[0]), static_cast<double>(item_value[1])});
        else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
            std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << field_name << '"' << '\n';
    });

} // AntigenicMapsLayoutDrawAce::add_label

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(acmacs::surface::Surface& aSurface, size_t /*aSectionIndex*/)
{
    if (!mTrackedAntigenColorByMonth.empty() && !mColorScalePanelDrawn)
        antigenic_maps_draw().time_series().draw_color_scale(mTrackedAntigenColorByMonth);

    apply_mods_before(aSurface);

      // aSurface.viewport(viewport());

    chart_draw().draw(aSurface);

    apply_mods_after(aSurface);

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
