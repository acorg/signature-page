#include "acmacs-map-draw/vaccines.hh"
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
                chart_draw().rotate(mod.get("angle", 0.0) * M_PI / 180.0);
            }
            else if (name == "rotate_radians") {
                chart_draw().rotate(mod.get("angle", 0.0));
            }
            else if (name == "flip") {
                const rjson::array& ar = mod.get_ref("value", rjson::array{0.0, 0.0});
                chart_draw().flip(ar[0], ar[1]);
            }
            else if (name == "viewport") {
                chart_draw().viewport(mod.get_viewport());
            }
            else if (name == "title") {
                chart_draw().title()
                        .text_size(mod.get("text_size", 12.0))
                        .text_color(mod.get_color("text_color", "black"))
                        .weight(mod.get("weight", "normal"))
                        .slant(mod.get("slant", "normal"))
                        .font_family(mod.get("font_family", "san serif"))
                        .background("transparent")
                        .border_width(0)
                        .padding(mod.get("padding", 3.0))
                        .offset(mod.offset())
                        .remove_all_lines();
            }
              // else if (name == "tracked_antigen") {
              //     mDrawTrackedAntigen.color(mod.get("color", "green3"));
              // }
        }
        catch (std::bad_variant_access&) {
            std::cerr << "MOD: " << mod.to_json() << DEBUG_LINE_FUNC << '\n';
            throw;
        }
    }
    chart_draw().calculate_viewport();

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections()
{
    chart_draw().prepare();
    chart_draw().mark_egg_antigens();
    chart_draw().mark_reassortant_antigens();
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "point_scale") {
            chart_draw().scale_points(mod.get("scale", 1.0), mod.get("outline_scale", 1.0));
        }
        if (name == "reference_antigens") {
            chart_draw().modify(chart().reference_antigen_indices(),
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 5.0)})
                              .fill(mod.get_color("fill", "transparent"))
                              .outline(mod.get_color("outline", "grey88"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
        else if (name == "test_antigens") {
            chart_draw().modify(chart().test_antigen_indices(),
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 3.0)})
                              .fill(mod.get_color("fill", "grey88"))
                              .outline(mod.get_color("outline", "grey88"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
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
            chart_draw().modify_all_sera(PointStyleDraw(PointStyle::Empty)
                                       .size(Pixels{mod.get("size", 5.0)})
                                       .outline(mod.get_color("outline", "grey88"))
                                       .outline_width(Pixels{mod.get("outline_width", 0.5)}), ChartDraw::Lower); // reset, lower sera
        }
        else if (name == "sequenced_antigens") {      // mark sequenced antigens (removes old tracked antigens marking)
            std::vector<size_t> sequenced_indices(sequenced_antigens().size());
            std::transform(sequenced_antigens().begin(), sequenced_antigens().end(), sequenced_indices.begin(), [](const auto& src) -> size_t { return src.first; });
            chart_draw().modify(sequenced_indices.begin(), sequenced_indices.end(),
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 3.0)})
                              .fill(mod.get_color("fill", "grey63"))
                              .outline(mod.get_color("outline", "white"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
        else if (name == "tracked_serum_circles") {
            chart_draw().remove_serum_circles();
        }
    }

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "tracked_antigens") {
            std::vector<size_t> tracked_indices;
            tracked_antigens(tracked_indices, aSectionIndex, report_antigens_in_hz_sections);
            chart_draw().modify(tracked_indices.begin(), tracked_indices.end(),
                                PointStyleDraw(PointStyle::Empty)
                                .size(Pixels{mod.get("size", 5.0)})
                                .fill(mod.get_color("fill", "grey63"))
                                .outline(mod.get_color("outline", "white"))
                                .outline_width(Pixels{mod.get("outline_width", 0.5)}), ChartDraw::Raise);
        }
        else if (name == "tracked_sera") {
            std::map<size_t, std::vector<size_t>> tracked_indices;
            tracked_sera(tracked_indices, aSectionIndex);
            std::cerr << "INFO: tracked_sera: " << tracked_indices << std::endl;
            for (auto iter = tracked_indices.begin(); iter != tracked_indices.end(); ++iter)
                chart_draw().modify_serum(
                    iter->first,
                    PointStyleDraw(PointStyle::Empty)
                    .size(Pixels{mod.get("size", 5.0)})
                    .outline(mod.get_color("outline", "black"))
                    .outline_width(Pixels{mod.get("outline_width", 0.5)}), ChartDraw::Raise);
        }
        else if (name == "tracked_serum_circles") {
            tracked_serum_circles(mod, aSectionIndex);
        }
        else if (name == "vaccines") {
            mark_vaccines(mod);
        }
        else if (name == "antigens") {
            const rjson::object& select = mod.get_ref("select", rjson::object{});
            try {
                const auto index = select.get_ref("index");
                  // std::cerr << "DEBUG: Antigens " << index << '\n';
                chart_draw().modify(index,
                                    PointStyleDraw(PointStyle::Empty)
                                    .size(Pixels{mod.get("size", 5.0)})
                                    .fill(mod.get_color("fill", "pink"))
                                    .outline(mod.get_color("outline", "white"))
                                    .outline_width(Pixels{mod.get("outline_width", 0.5)}),
                                    mod.get("raise_", true) ? ChartDraw::Raise : ChartDraw::NoOrderChange);
                try {
                    const rjson::object& label_data = const_cast<AntigenicMapMod&>(mod).get_ref_to_object("label");
                    // std::cerr << "DEBUG: label_data: " << label_data << '\n';
                    // std::cerr << "DEBUG: label size: " << label_data.get_ref_not_set("size", rjson::number{0}) << '\n';
                    auto& label = chart_draw().add_label(index);
                    label.size(label_data.get_ref_not_set("size", rjson::number{9}));
                    const auto offset_v = label_data.get_ref_not_set("offset", rjson::array{0, 1});
                    const auto& offset = static_cast<const rjson::array&>(offset_v);
                    // std::cerr << "DEBUG: label offset: " << offset << '\n';
                    label.offset(offset[0], offset[1]);
                    const std::string display_name = label_data.get_ref_not_set("display_name", rjson::string{});
                    if (!display_name.empty())
                        label.display_name(display_name);
                }
                catch (rjson::object::field_not_found&) { // no label
                }
            }
            catch (rjson::object::field_not_found&) {
                std::cerr << "WARNING: Antigens: no index in " << select << '\n';
            }
        }
    }

      // marked antigens
      // tracked_antigen_colored_by_clade

    std::string title = hz_sections().node_refs[aSectionIndex].index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    const HzSection& section = hz_sections().sections[aSectionIndex];
    if (!section.label.empty())
        title += " " + static_cast<std::string>(section.label);
    chart_draw().title().remove_all_lines().add_line(title);

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_antigens(std::vector<size_t>& tracked_indices, size_t aSectionIndex, bool report_antigens_in_hz_sections) const
{
    for (const auto& sequenced_section: sequenced_antigens()) {
        if (sequenced_section.second == aSectionIndex) {
            tracked_indices.push_back(sequenced_section.first);
            if (report_antigens_in_hz_sections) {
                std::cout << aSectionIndex << ' ' << sequenced_section.first << ' ' << chart().antigen(sequenced_section.first).full_name() << '\n';
            }
        }
    }

} // AntigenicMapsLayoutDrawAce::tracked_antigens

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_sera(std::map<size_t, std::vector<size_t>>& tracked_indices, size_t aSectionIndex) const
{
    const auto& chrt = chart();
    if (!mHomologousAntigenForSeraFound) {
        chrt.find_homologous_antigen_for_sera_const();
        mHomologousAntigenForSeraFound = true;
    }

    std::vector<size_t> tracked_antigen_indices;
    tracked_antigens(tracked_antigen_indices, aSectionIndex, false);

    for (size_t serum_no = 0; serum_no < chrt.number_of_sera(); ++serum_no) {
        const auto& homologous_antigens_for_serum = chrt.serum(serum_no).homologous();
        for (size_t antigen_no: homologous_antigens_for_serum) {
            if (std::find(tracked_antigen_indices.begin(), tracked_antigen_indices.end(), antigen_no) != tracked_antigen_indices.end()) {
                tracked_indices[serum_no] = homologous_antigens_for_serum;
                break;
            }
        }
    }

} // AntigenicMapsLayoutDrawAce::tracked_sera

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_serum_circles(const AntigenicMapMod& mod, size_t aSectionIndex)
{
    std::map<size_t, std::vector<size_t>> tracked_indices;
    tracked_sera(tracked_indices, aSectionIndex);
    for (auto serum_antigens: tracked_indices) {
        std::vector<double> radii(serum_antigens.second.size());
        std::transform(serum_antigens.second.begin(), serum_antigens.second.end(), radii.begin(), [&](size_t ag_no) -> double { return chart().serum_circle_radius(ag_no, serum_antigens.first, 0, false); });
        std::sort(radii.begin(), radii.end());
        const auto radius_p = std::find_if(radii.begin(), radii.end(), [](double r) -> bool { return r >= 0.0; });
        if (radius_p != radii.end()) {
            std::cerr << "INFO: serum_circle for " << serum_antigens.first << ' ' << chart().serum(serum_antigens.first).full_name() << " radius: " << *radius_p << std::endl;
            auto& serum_circle = chart_draw().serum_circle(serum_antigens.first, Scaled{*radius_p});
            serum_circle
                    .fill(mod.get_color("fill", "transparent"))
                    .outline(mod.get_color("outline", "black"), mod.get("outline_width", 1.0))
                    .radius_line(mod.get_color("radius_line", "transparent"), mod.get("radius_line_width", 1.0));
                      //.angles(mod.get["angle_degrees"][0] * math.pi / 180.0, mod.get["angle_degrees"][1] * math.pi / 180.0);
            const std::string radius_line_dash = mod.get("radius_line_dash", "");
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
        const auto& chart = static_cast<const Chart&>(chart_draw().chart());
        const auto& hidb = mHiDbSet.get(chart.chart_info().virus_type());
        Vaccines vaccs{chart, hidb};
        for (const auto& mod_v: vaccine_mod.mods()) {
            const rjson::object& mod = mod_v;
            const std::string type = mod.get_field("type", std::string{}), passage = mod.get_field("passage", std::string{}), name = mod.get_field("name", std::string{});
            std::unique_ptr<VaccineMatcher> matcher{vaccs.match(name, type, passage)};
              // std::cout << matcher->report(2) << '\n';
            for (const auto& [item_key, item_value]: mod) {
                const std::string field_name = item_key;
                if (field_name == "size")
                    matcher->size(item_value);
                else if (field_name == "fill")
                    matcher->fill(static_cast<std::string>(item_value));
                else if (field_name == "outline")
                    matcher->outline(static_cast<std::string>(item_value));
                else if (field_name == "outline_width")
                    matcher->outline_width(item_value);
                else if (field_name == "aspect")
                    matcher->aspect(item_value);
                else if (field_name == "rotation")
                    matcher->rotation(item_value);
                else if (field_name == "no")
                    matcher->no(item_value); // size_t
                else if (field_name == "show") {
                    const bool show = item_value;
                    matcher->show(show);
                    if (!show)
                        matcher->hide_label(chart_draw(), hidb.locdb());
                }
                else if (field_name == "label")
                    add_label(std::shared_ptr<VaccineMatcherLabel>{matcher->label(chart_draw(), hidb.locdb())}, static_cast<const rjson::object&>(item_value));
                else if (field_name != "type" && field_name != "passage" && field_name != "name" && (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?')))
                    std::cerr << "WARNING: mark_vaccines: unrecognized key \"" << field_name << '"' << std::endl;
            }
        }
          // std::cerr << "DEBUG: Vaccines:" << std::endl << vaccs.report(2) << std::endl;
        vaccs.plot(chart_draw());
    }
    catch (std::bad_variant_access&) {
        std::cerr << "WARNING: cannot mark vaccines: invalid vaccine settings: " << vaccine_mod.to_json() << std::endl;
    }
    catch (hidb::NoHiDb&) {
        std::cerr << "WARNING: cannot mark vaccines: no hidb for \"" << chart_draw().chart().chart_info().virus_type() << "\"" << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::mark_vaccines

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::add_label(std::shared_ptr<VaccineMatcherLabel> label, const rjson::object& data)
{
    for (const auto& [item_key, item_value]: data) {
        const std::string field_name = item_key;
        if (field_name == "size")
            label->size(item_value);
        else if (field_name == "color")
            label->color(static_cast<std::string>(item_value));
        else if (field_name == "font_family")
            label->font_family(item_value);
        else if (field_name == "name_type")
            label->name_type(item_value);
        else if (field_name == "slant")
            label->slant(item_value);
        else if (field_name == "weight")
            label->weight(item_value);
        else if (field_name == "offset") {
            const rjson::array& offset = item_value;
            label->offset(offset[0], offset[1]);
        }
        else if (field_name.empty() || (field_name.front() != '?' && field_name.back() != '?'))
            std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << field_name << '"' << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::add_label

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(Surface& aSurface, size_t /*aSectionIndex*/)
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
