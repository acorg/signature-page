#include "acmacs-map-draw/vaccine-matcher.hh"
#include "acmacs-map-draw/mod-applicator.hh"
#include "ace-antigenic-maps-draw.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void ChartDrawInterface::init_settings()
{

} // ChartDrawInterface::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_apply_mods()
{
    // std::cerr << "DEBUG: [ace] AntigenicMapsLayoutDrawAce::prepare_apply_mods" << std::endl;
    settings().mods.for_each([this](const auto& mod) {
        try {
            const std::string name(mod.name());
            if (name == "rotate_degrees") {
                chart_draw().rotate(mod.get_or_default("angle", 0.0) * M_PI / 180.0);
            }
            else if (name == "rotate_radians") {
                chart_draw().rotate(mod.get_or_default("angle", 0.0));
            }
            else if (name == "rotate") {
                if (const double degrees = mod.get_or_default("degrees", 0.0); !float_zero(degrees))
                    chart_draw().rotate(degrees * M_PI / 180.0);
                else if (const double radians = mod.get_or_default("radians", 0.0); !float_zero(radians))
                    chart_draw().rotate(radians);
            }
            else if (name == "flip") {
                if (auto direction = mod.get_or_default("direction", ""); !direction.empty()) {
                    if (direction == "ns")
                        chart_draw().flip(1, 0);
                    else if (direction == "ew")
                        chart_draw().flip(0, 1);
                    else
                        std::cerr << "ERROR: unrecognized flip value: " << mod << DEBUG_LINE_FUNC << '\n';
                }
                else {
                    if (const auto& ar = mod["value"]; !ar.is_null())
                        chart_draw().flip(ar[0], ar[1]);
                }
            }
            else if (name == "viewport") {
                chart_draw().viewport(mod.get_viewport(chart_draw().calculate_viewport(false)));
            }
            else if (name == "title") {
                chart_draw()
                    .title()
                    .text_size(mod.get_or_default("text_size", 12.0))
                    .text_color(mod.get_color("text_color", "black"))
                    .weight(std::string(mod.get_or_default("weight", "normal")))
                    .slant(std::string(mod.get_or_default("slant", "normal")))
                    .font_family(std::string(mod.get_or_default("font_family", "san serif")))
                    .background("transparent")
                    .border_width(0)
                    .padding(mod.get_or_default("padding", 3.0))
                    .offset(mod.offset())
                    .remove_all_lines();
            }
            // else if (name == "tracked_antigen") {
            //     mDrawTrackedAntigen.color(mod.get("color", "green3"));
            // }
        }
        catch (std::bad_variant_access&) {
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
    settings().mods.for_each([this](const auto& mod) {
        const std::string name(mod.name());
        if (name == "point_scale") {
            chart_draw().scale_points(mod.get_or_default("scale", 1.0), mod.get_or_default("outline_scale", 1.0));
        }
        if (name == "reference_antigens") {
            acmacs::PointStyle ref_antigen_style;
            ref_antigen_style.size = Pixels{mod.get_or_default("size", 5.0)};
            ref_antigen_style.fill = mod.get_color("fill", "transparent");
            ref_antigen_style.outline = mod.get_color("outline", "grey88");
            ref_antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify(chart().antigens()->reference_indexes(), ref_antigen_style);
        }
        else if (name == "test_antigens") {
            acmacs::PointStyle test_antigen_style;
            test_antigen_style.size = Pixels{mod.get_or_default("size", 3.0)};
            test_antigen_style.fill = mod.get_color("fill", "grey88");
            test_antigen_style.outline = mod.get_color("outline", "grey88");
            test_antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify(chart().antigens()->test_indexes(), test_antigen_style);
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

    settings().mods.for_each([this](const auto& mod) {
        const std::string name(mod.name());
        if (name == "sera") {
            acmacs::PointStyle serum_style;
            serum_style.size = Pixels{mod.get_or_default("size", 5.0)};
            serum_style.fill = mod.get_color("fill", "transparent");
            serum_style.outline = mod.get_color("outline", "grey88");
            serum_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify_all_sera(serum_style, PointDrawingOrder::Lower);
        }
        else if (name == "sequenced_antigens") {      // mark sequenced antigens (removes old tracked antigens marking)
            std::vector<size_t> sequenced_indices(sequenced_antigens().size());
            std::transform(sequenced_antigens().begin(), sequenced_antigens().end(), sequenced_indices.begin(), [](const auto& src) -> size_t { return src.first; });
            acmacs::PointStyle sequenced_antigen_style;
            sequenced_antigen_style.size = Pixels{mod.get_or_default("size", 3.0)};
            sequenced_antigen_style.fill = mod.get_color("fill", "grey88");
            sequenced_antigen_style.outline = mod.get_color("outline", "white");
            sequenced_antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify(sequenced_indices, sequenced_antigen_style);
        }
    });

    settings().mods.for_each([this,aSectionIndex,report_antigens_in_hz_sections,map_letter](const auto& mod) {
        const std::string name(mod.name());
        if (name == "tracked_antigens") {
            const auto tracked_indices = tracked_antigens(aSectionIndex, report_antigens_in_hz_sections);
            acmacs::PointStyle tracked_antigen_style;
            tracked_antigen_style.size = Pixels{mod.get_or_default("size", 5.0)};
            tracked_antigen_style.fill = mod.get_color("fill", "grey63");
            tracked_antigen_style.outline = mod.get_color("outline", "white");
            tracked_antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify(tracked_indices, tracked_antigen_style, PointDrawingOrder::Raise);
        }
        else if (name == "tracked_sera") {
            const auto tracked_indices = tracked_sera(aSectionIndex);
            std::cout << "INFO: tracked_sera: " << tracked_indices << std::endl;
            for (auto [serum_index, ignored]: tracked_indices)
                make_tracked_serum(serum_index, Pixels{mod.get_or_default("size", 5.0)}, mod.get_color("outline", "black"), Pixels{mod.get_or_default("outline_width", 0.5)});
        }
        else if (name == "tracked_serum_circles") {
            tracked_serum_circles(mod, aSectionIndex);
        }
        else if (name == "serum_circle") {
            serum_circle(mod, map_letter, aSectionIndex);
        }
        else if (name == "vaccines") {
            mark_vaccines(mod);
        }
        else if (name == "antigens") {
            mark_antigens(mod);
        }
        else if (name == "antigens_old") {
            mark_antigens_old(mod);
        }
    });

      // marked antigens
      // tracked_antigen_colored_by_clade

    std::string title = hz_sections().node_refs[aSectionIndex].index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    const HzSection& section = hz_sections().sections[aSectionIndex];
    if (!section.label.empty())
        title += " " + static_cast<std::string>(section.label);
    // std::cerr << "DEBUG: map title: " << title << '\n';
    chart_draw().title().remove_all_lines().show(true).add_line(title);

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::make_tracked_serum(size_t serum_index, Pixels size, Color outline, Pixels outline_width, const rjson::value& label_data)
{
    acmacs::PointStyle tracked_serum_style;
    tracked_serum_style.size = size;
    tracked_serum_style.outline = outline;
    tracked_serum_style.outline_width = outline_width;
    chart_draw().modify_serum(serum_index, tracked_serum_style, PointDrawingOrder::Raise);
    if (!label_data.empty() && rjson::get_or(label_data, "show", true)) {
        auto& label = chart_draw().add_label(chart().number_of_antigens() + serum_index);
        rjson::for_each(label_data, [&label,&label_data,this,serum_index](const std::string& field_name, const rjson::value& item_value) {
            if (field_name == "size")
                label.size(item_value);
            else if (field_name == "color")
                label.color(Color(static_cast<std::string_view>(item_value)));
            else if (field_name == "font_family")
                label.font_family(item_value);
            else if (field_name == "name_type") {
                auto serum = chart().serum(serum_index);
                const std::string_view name_type = item_value;
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
                label.display_name(item_value);
            else if (field_name == "slant")
                label.slant(item_value);
            else if (field_name == "weight")
                label.weight(item_value);
            else if (field_name == "offset") {
                label.offset({item_value[0], item_value[1]});
            }
            else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
                std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << field_name << '"' << std::endl;
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

void AntigenicMapsLayoutDrawAce::find_homologous_antigens_for_sera() const
{
    if (!mHomologousAntigenForSeraFound) {
        chart().set_homologous(acmacs::chart::Chart::find_homologous_for_big_chart::yes);
        mHomologousAntigenForSeraFound = true;
    }

} // AntigenicMapsLayoutDrawAce::find_homologous_antigens_for_sera

// ----------------------------------------------------------------------

std::map<size_t, acmacs::chart::PointIndexList> AntigenicMapsLayoutDrawAce::tracked_sera(size_t aSectionIndex) const
{
    find_homologous_antigens_for_sera();
    const auto& chrt = chart();
    const auto tracked_antigen_indices = tracked_antigens(aSectionIndex, false);
    std::map<size_t, acmacs::chart::PointIndexList> tracked_indices;
    for (size_t serum_no = 0; serum_no < chrt.number_of_sera(); ++serum_no) {
        const auto homologous_antigens_for_serum = chrt.serum(serum_no)->homologous_antigens();
        // if (homologous_antigens_for_serum.empty())
        //     std::cerr << "DEBUG: no homologous antigens for serum " << serum_no <<  ' ' << chrt.serum(serum_no)->full_name() << " for section " << aSectionIndex << '\n';
        // else
        //     std::cerr << "DEBUG: section " << aSectionIndex << " serum: " << serum_no <<  ' ' << chrt.serum(serum_no)->full_name()
        //               << "\n    homologous antigens: " << homologous_antigens_for_serum
        //               << "\n    tracked_antigen_indices: " << tracked_antigen_indices
        //               << '\n';
        for (size_t antigen_no: homologous_antigens_for_serum) {
            if (tracked_antigen_indices.contains(antigen_no)) {
                tracked_indices[serum_no] = homologous_antigens_for_serum;
                break;
            }
        }
    }
    return tracked_indices;

} // AntigenicMapsLayoutDrawAce::tracked_sera

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_serum_circles(const AntigenicMapMod& mod, size_t aSectionIndex)
{
    find_homologous_antigens_for_sera();
    for (auto serum_antigens: tracked_sera(aSectionIndex))
        make_serum_circle(mod, serum_antigens.first, serum_antigens.second);

} // AntigenicMapsLayoutDrawAce::tracked_serum_circles

// ----------------------------------------------------------------------

// returns if circle shown
bool AntigenicMapsLayoutDrawAce::make_serum_circle(const AntigenicMapMod& mod, size_t serum_no, const acmacs::chart::PointIndexList& homologous_antigens)
{
    std::vector<double> radii(homologous_antigens.size());
    std::transform(homologous_antigens.begin(), homologous_antigens.end(), radii.begin(),
                   [&](size_t ag_no) -> double { return chart().serum_circle_radius_empirical(ag_no, serum_no, 0, false); });
    std::sort(radii.begin(), radii.end());
    const auto radius_p = std::find_if(radii.begin(), radii.end(), [](double r) -> bool { return r >= 0.0; });
    const bool shown = radius_p != radii.end();
    if (shown) {
        std::cout << "INFO: serum circle for " << serum_no << ' ' << chart().serum(serum_no)->full_name() << " radius: " << *radius_p << " antigens: " << homologous_antigens << '\n';
        auto& serum_circle = chart_draw().serum_circle(serum_no, Scaled{*radius_p});
        serum_circle.fill(mod.get_color("fill", "transparent"))
            .outline(mod.get_color("outline", "black"), mod.get_or_default("outline_width", 1.0))
            .radius_line(mod.get_color("radius_line", "transparent"), mod.get_or_default("radius_line_width", 1.0));
        //.angles(mod.get["angle_degrees"][0] * math.pi / 180.0, mod.get["angle_degrees"][1] * math.pi / 180.0);
        const auto radius_line_dash = mod.get_or_default("radius_line_dash", "");
        if (radius_line_dash.empty() || radius_line_dash == "nodash")
            serum_circle.radius_line_no_dash();
        else if (radius_line_dash == "dash1")
            serum_circle.radius_line_dash1();
        else if (radius_line_dash == "dash2")
            serum_circle.radius_line_dash2();
    }
    else {
        std::cerr << "WARNING: no serum circle for " << serum_no << ' ' << chart().serum(serum_no)->full_name();
        if (homologous_antigens.empty())
            std::cerr << " no homologous antigens\n";
        else
            std::cerr << " antigens: " << homologous_antigens << ": no radius\n";
    }
    return shown;

} // AntigenicMapsLayoutDrawAce::make_serum_circle

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::serum_circle(const AntigenicMapMod& mod, std::string map_letter, size_t /*aSectionIndex*/)
{
    if (const auto map = mod.get_or_default("map", "A"); map == map_letter) {
        find_homologous_antigens_for_sera();
        std::string serum_name(mod.get_or_default("serum", ""));
        if (serum_name.empty())
            throw std::runtime_error("invalid mod (\"serum\" not found): " + mod.to_json());
        const auto& chrt = chart();
        auto serum_index = chrt.sera()->find_by_full_name(serum_name);
        if (!serum_index)
            throw std::runtime_error("serum not found: " + mod.to_json());
        const auto homologous_antigens_for_serum = chrt.serum(*serum_index)->homologous_antigens();
        if (!homologous_antigens_for_serum.empty()) {
            // std::cout << "INFO: forced serum circle for serum: " << *serum_index << ' ' << serum_name << '\n';
            if (make_serum_circle(mod, *serum_index, homologous_antigens_for_serum))
                make_tracked_serum(*serum_index, Pixels{mod.get_or_default("serum_size", 5.0)}, mod.get_color("serum_outline", mod.get_color("outline", "black")), Pixels{mod.get_or_default("serum_outline_width", 0.5)}, mod.get_or_empty_object("label"));
        }
        else
            std::cerr << "WARNING: no homologous antigens for serum (for forced serum circle): " << *serum_index << ' ' << serum_name << '\n';
    }

} // AntigenicMapsLayoutDrawAce::serum_circle

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_vaccines(const AntigenicMapMod& vaccine_mod)
{
    try {
        const auto& chart = chart_draw().chart();
        Vaccines vaccs{chart};
        rjson::for_each(vaccine_mod.mods(), [&vaccs, this](const rjson::value& mod) {
            const std::string type(rjson::get_or(mod, "type", "")), passage(rjson::get_or(mod, "passage", "")), name(rjson::get_or(mod, "name", ""));
            VaccineMatcher matcher(vaccs, VaccineMatchData{}.name(name).type(type).passage(passage));
            // std::cerr << matcher.report(2) << '\n';
            rjson::for_each(mod, [&matcher, this](const std::string& field_name, const rjson::value& item_value) {
                if (field_name == "size")
                    matcher.size(item_value);
                else if (field_name == "fill")
                    matcher.fill(item_value);
                else if (field_name == "outline")
                    matcher.outline(item_value);
                else if (field_name == "outline_width")
                    matcher.outline_width(item_value);
                else if (field_name == "aspect")
                    matcher.aspect(item_value);
                else if (field_name == "rotation")
                    matcher.rotation(item_value);
                else if (field_name == "no")
                    matcher.no(item_value); // size_t
                else if (field_name == "show") {
                    const bool show = item_value.get_bool();
                    matcher.show(show);
                    if (!show)
                        matcher.hide_label(chart_draw());
                }
                else if (field_name == "label")
                    add_label(std::shared_ptr<VaccineMatcherLabel>{matcher.label(chart_draw())}, item_value);
                else if (field_name != "type" && field_name != "passage" && field_name != "name" && (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?')))
                    std::cerr << "WARNING: mark_vaccines: unrecognized key \"" << field_name << '"' << std::endl;
            });
        });
        // std::cerr << "DEBUG: Vaccines:" << std::endl << vaccs.report(2) << std::endl;
        vaccs.plot(chart_draw());
    }
    catch (std::exception&) {
        std::cerr << "WARNING: cannot mark vaccines: invalid vaccine settings: " << vaccine_mod << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::mark_vaccines

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_antigens(const AntigenicMapMod& mod)
{
    ModAntigens applicator(mod.data());
    applicator.apply(chart_draw(), rjson::value{});

} // AntigenicMapsLayoutDrawAce::mark_antigens

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_antigens_old(const AntigenicMapMod& mod)
{
    const auto& select = mod.get_or_empty_object("select");
    try {
        if (const auto& index = select["index"]; !index.is_null()) {
            acmacs::PointStyle antigen_style;
            antigen_style.size = Pixels{mod.get_or_default("size", 5.0)};
            antigen_style.fill = mod.get_color("fill", "pink");
            antigen_style.outline = mod.get_color("outline", "white");
            antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            chart_draw().modify(static_cast<size_t>(index), antigen_style, mod.get_or_default("raise_", true) ? PointDrawingOrder::Raise : PointDrawingOrder::NoChange);
            if (const rjson::value& label_data = mod["label"]; !label_data.is_null()) {
                auto& label = chart_draw().add_label(index);
                label.size(rjson::get_or(label_data, "size", 9.0));
                if (const auto& offset = label_data["offset"]; !offset.is_null())
                    label.offset({offset[0], offset[1]});
                else
                    label.offset({0, 1});
                if (const auto& display_name = label_data["display_name"]; !display_name.is_null())
                    label.display_name(std::string(display_name));
            }
        }
        else
            std::cerr << "WARNING: Antigens: no index in " << select << '\n';
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: Antigens: error: " << err.what() << " in " << select << '\n';
    }

} // AntigenicMapsLayoutDrawAce::mark_antigens_old

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::add_label(std::shared_ptr<VaccineMatcherLabel> label, const rjson::value& data)
{
    rjson::for_each(data, [&label](const std::string& field_name, const rjson::value& item_value) {
        if (field_name == "size")
            label->size(item_value);
        else if (field_name == "color")
            label->color(static_cast<std::string_view>(item_value));
        else if (field_name == "font_family")
            label->font_family(static_cast<std::string_view>(item_value));
        else if (field_name == "name_type")
            label->name_type(static_cast<std::string>(item_value));
        else if (field_name == "slant")
            label->slant(static_cast<std::string_view>(item_value));
        else if (field_name == "weight")
            label->weight(static_cast<std::string_view>(item_value));
        else if (field_name == "offset")
            label->offset({item_value[0], item_value[1]});
        else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
            std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << field_name << '"' << std::endl;
    });

} // AntigenicMapsLayoutDrawAce::add_label

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(acmacs::surface::Surface& aSurface, size_t /*aSectionIndex*/)
{
    apply_mods_before(aSurface);

      // aSurface.viewport(viewport());

    chart_draw().draw(aSurface);

    apply_mods_after(aSurface);

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
