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
    for (const auto& mod: settings().mods) {
        try {
            const std::string name = mod.name();
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
                if (std::string direction = mod.get_or_default("direction", ""); !direction.empty()) {
                    if (direction == "ns")
                        chart_draw().flip(1, 0);
                    else if (direction == "ew")
                        chart_draw().flip(0, 1);
                    else
                        std::cerr << "ERROR: unrecognized flip value: " << mod << DEBUG_LINE_FUNC << '\n';
                }
                else {
                    try {
                        const rjson::array& ar = mod["value"];
                        chart_draw().flip(ar[0], ar[1]);
                    }
                    catch (rjson::field_not_found&) {
                    }
                }
            }
            else if (name == "viewport") {
                chart_draw().viewport(mod.get_viewport(chart_draw().calculate_viewport(false)));
            }
            else if (name == "title") {
                chart_draw().title()
                        .text_size(mod.get_or_default("text_size", 12.0))
                        .text_color(mod.get_color("text_color", "black"))
                        .weight(mod.get_or_default("weight", "normal"))
                        .slant(mod.get_or_default("slant", "normal"))
                        .font_family(mod.get_or_default("font_family", "san serif"))
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
            std::cerr << "MOD: " << mod << DEBUG_LINE_FUNC << '\n';
            throw;
        }
    }
    chart_draw().calculate_viewport();

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections()
{
    chart_draw().mark_egg_antigens();
    chart_draw().mark_reassortant_antigens();
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
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
    }

} // AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_drawing_chart(size_t aSectionIndex, bool report_antigens_in_hz_sections)
{
    // reset tracked antigens and sera shown on the previous map
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
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
        else if (name == "tracked_serum_circles") {
            chart_draw().remove_serum_circles();
        }
    }

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
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
            std::cerr << "INFO: tracked_sera: " << tracked_indices << std::endl;
            acmacs::PointStyle tracked_serum_style;
            tracked_serum_style.size = Pixels{mod.get_or_default("size", 5.0)};
            tracked_serum_style.outline = mod.get_color("outline", "black");
            tracked_serum_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
            // for (auto iter = tracked_indices.begin(); iter != tracked_indices.end(); ++iter)
            //     chart_draw().modify_serum(iter->first, tracked_serum_style, PointDrawingOrder::Raise);
            for (auto [serum_index, ignored]: tracked_indices)
                chart_draw().modify_serum(serum_index, tracked_serum_style, PointDrawingOrder::Raise);
        }
        else if (name == "tracked_serum_circles") {
            tracked_serum_circles(mod, aSectionIndex);
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
    }

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

std::map<size_t, acmacs::chart::PointIndexList> AntigenicMapsLayoutDrawAce::tracked_sera(size_t aSectionIndex) const
{
    const auto& chrt = chart();
    if (!mHomologousAntigenForSeraFound) {
        chrt.set_homologous(acmacs::chart::Chart::find_homologous_for_big_chart::yes);
        mHomologousAntigenForSeraFound = true;
    }

    const auto tracked_antigen_indices = tracked_antigens(aSectionIndex, false);
    std::map<size_t, acmacs::chart::PointIndexList> tracked_indices;
    for (size_t serum_no = 0; serum_no < chrt.number_of_sera(); ++serum_no) {
        const auto homologous_antigens_for_serum = chrt.serum(serum_no)->homologous_antigens();
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
    for (auto serum_antigens: tracked_sera(aSectionIndex)) {
        std::vector<double> radii(serum_antigens.second.size());
        std::transform(serum_antigens.second.begin(), serum_antigens.second.end(), radii.begin(), [&](size_t ag_no) -> double { return chart().serum_circle_radius_empirical(ag_no, serum_antigens.first, 0, false); });
        std::sort(radii.begin(), radii.end());
        const auto radius_p = std::find_if(radii.begin(), radii.end(), [](double r) -> bool { return r >= 0.0; });
        if (radius_p != radii.end()) {
            std::cerr << "INFO: serum_circle for " << serum_antigens.first << ' ' << chart().serum(serum_antigens.first)->full_name() << " radius: " << *radius_p << std::endl;
            auto& serum_circle = chart_draw().serum_circle(serum_antigens.first, Scaled{*radius_p});
            serum_circle
                    .fill(mod.get_color("fill", "transparent"))
                    .outline(mod.get_color("outline", "black"), mod.get_or_default("outline_width", 1.0))
                    .radius_line(mod.get_color("radius_line", "transparent"), mod.get_or_default("radius_line_width", 1.0));
                      //.angles(mod.get["angle_degrees"][0] * math.pi / 180.0, mod.get["angle_degrees"][1] * math.pi / 180.0);
            const std::string radius_line_dash = mod.get_or_default("radius_line_dash", "");
            if (radius_line_dash.empty() || radius_line_dash == "nodash")
                serum_circle.radius_line_no_dash();
            else if (radius_line_dash == "dash1")
                serum_circle.radius_line_dash1();
            else if (radius_line_dash == "dash2")
                serum_circle.radius_line_dash2();
        }
    }

} // AntigenicMapsLayoutDrawAce::tracked_serum_circles

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_vaccines(const AntigenicMapMod& vaccine_mod)
{
    try {
        const auto& chart = chart_draw().chart();
        Vaccines vaccs{chart};
        for (const auto& mod_v: vaccine_mod.mods()) {
            const rjson::object& mod = mod_v;
            const std::string type = mod.get_or_default("type", std::string{}), passage = mod.get_or_default("passage", std::string{}), name = mod.get_or_default("name", std::string{});
            VaccineMatcher matcher(vaccs, VaccineMatchData{}.name(name).type(type).passage(passage));
              // std::cerr << matcher.report(2) << '\n';
            for (const auto& [item_key, item_value]: mod) {
                const auto field_name = item_key.strv();
                if (field_name == "size")
                    matcher.size(item_value);
                else if (field_name == "fill")
                    matcher.fill(Color(item_value));
                else if (field_name == "outline")
                    matcher.outline(Color(item_value));
                else if (field_name == "outline_width")
                    matcher.outline_width(item_value);
                else if (field_name == "aspect")
                    matcher.aspect(item_value);
                else if (field_name == "rotation")
                    matcher.rotation(item_value);
                else if (field_name == "no")
                    matcher.no(item_value); // size_t
                else if (field_name == "show") {
                    const bool show = item_value;
                    matcher.show(show);
                    if (!show)
                        matcher.hide_label(chart_draw());
                }
                else if (field_name == "label")
                    add_label(std::shared_ptr<VaccineMatcherLabel>{matcher.label(chart_draw())}, static_cast<const rjson::object&>(item_value));
                else if (field_name != "type" && field_name != "passage" && field_name != "name" && (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?')))
                    std::cerr << "WARNING: mark_vaccines: unrecognized key \"" << field_name << '"' << std::endl;
            }
        }
        // std::cerr << "DEBUG: Vaccines:" << std::endl << vaccs.report(2) << std::endl;
        vaccs.plot(chart_draw());
    }
    catch (std::bad_variant_access&) {
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
        const size_t index = select["index"];
        acmacs::PointStyle antigen_style;
        antigen_style.size = Pixels{mod.get_or_default("size", 5.0)};
        antigen_style.fill = mod.get_color("fill", "pink");
        antigen_style.outline = mod.get_color("outline", "white");
        antigen_style.outline_width = Pixels{mod.get_or_default("outline_width", 0.5)};
        chart_draw().modify(index, antigen_style, mod.get_or_default("raise_", true) ? PointDrawingOrder::Raise : PointDrawingOrder::NoChange);
        try {
            const rjson::object& label_data = mod["label"];
            auto& label = chart_draw().add_label(index);
            label.size(label_data.get_or_default("size", 9.0));
            try {
                const rjson::array& offset = label_data["offset"];
                label.offset(offset[0], offset[1]);
            }
            catch (rjson::field_not_found&) {
                label.offset(0, 1);
            }
            if (const auto display_name = label_data.get<std::string>("display_name"); display_name)
                label.display_name(*display_name);
        }
        catch (rjson::field_not_found&) { // no label
        }
    }
    catch (rjson::field_not_found&) {
        std::cerr << "WARNING: Antigens: no index in " << select << '\n';
    }
    catch (std::out_of_range&) {
        std::cerr << "WARNING: Antigens: invalid index in " << select << '\n';
    }
    catch (std::exception& err) {
        std::cerr << "WARNING: Antigens: error: " << err.what() << " in " << select << '\n';
    }

} // AntigenicMapsLayoutDrawAce::mark_antigens_old

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::add_label(std::shared_ptr<VaccineMatcherLabel> label, const rjson::object& data)
{
    for (const auto& [item_key, item_value]: data) {
        const auto field_name = item_key.strv();
        if (field_name == "size")
            label->size(item_value);
        else if (field_name == "color")
            label->color(Color(item_value));
        else if (field_name == "font_family")
            label->font_family(item_value.str());
        else if (field_name == "name_type")
            label->name_type(item_value.str());
        else if (field_name == "slant")
            label->slant(item_value.str());
        else if (field_name == "weight")
            label->weight(item_value.str());
        else if (field_name == "offset") {
            const rjson::array& offset = item_value;
            label->offset(offset[0], offset[1]);
        }
        else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
            std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << field_name << '"' << std::endl;
    }

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
