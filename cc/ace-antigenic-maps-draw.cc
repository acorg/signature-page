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
    std::cerr << "INFO: [ace] AntigenicMapsLayoutDrawAce::prepare_apply_mods" << std::endl;
    auto& chart_draw = dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart()).chart_draw();
      // std::cerr << "INFO: [sdb] prepare_apply_mods 1 transformation " << chart.transformation() << std::endl;
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "rotate_degrees") {
            chart_draw.rotate(mod.get("angle", 0.0) * M_PI / 180.0);
        }
        else if (name == "rotate_radians") {
            chart_draw.rotate(mod.get("angle", 0.0));
        }
        else if (name == "viewport") {
            chart_draw.viewport(mod.get_viewport());
        }
        else if (name == "title") {
            chart_draw.title()
                    .text_size(mod.get("text_size", 12.0))
                    .text_color(mod.get("text_color", "black"))
                    .weight(mod.get("weight", "normal"))
                    .slant(mod.get("slant", "normal"))
                    .font_family(mod.get("font_family", "san serif"))
                    .background("transparent")
                    .border_width(0)
                    .padding(mod.get("padding", 3.0))
                    .offset(mod.get("offset", Location{}))
                    .remove_all_lines();
        }
        // else if (name == "tracked_antigen") {
        //     mDrawTrackedAntigen.color(mod.get("color", "green3"));
        // }
    }
    chart_draw.calculate_viewport();

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections()
{
    auto& chart_draw_interface = dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart());
    auto& chart_draw = chart_draw_interface.chart_draw();
    const auto& chart = chart_draw_interface.chart();
    chart_draw.prepare();
    chart_draw.mark_egg_antigens();
    chart_draw.mark_reassortant_antigens();
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "point_scale") {
            chart_draw.scale_points(mod.get("scale", 1.0), mod.get("outline_scale", 1.0));
        }
        // else if (name == "sera") {
        //     chart_draw.modify_all_sera(PointStyleDraw(PointStyle::Empty).outline(mod.get("outline", "black")).outline_width(Pixels{mod.get("outline_width", 1)}), false, true); // lower sera
        // }
        if (name == "reference_antigens") {
            chart_draw.modify(chart.reference_antigen_indices(), PointStyleDraw(PointStyle::Empty).fill(mod.get("fill", "transparent")).outline(mod.get("outline", "grey88")).outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
        else if (name == "test_antigens") {
            chart_draw.modify(chart.test_antigen_indices(), PointStyleDraw(PointStyle::Empty).fill(mod.get("fill", "grey88")).outline(mod.get("outline", "grey88")).outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
    }

} // AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_drawing_chart(size_t aSectionIndex)
{
    auto& chart_draw_interface = dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart());
    auto& chart_draw = chart_draw_interface.chart_draw();
    // const auto& chart = chart_draw_interface.chart();
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "sera") {
            chart_draw.modify_all_sera(PointStyleDraw(PointStyle::Empty).outline(mod.get("outline", "grey88")).outline_width(Pixels{mod.get("outline_width", 0.5)}), false, true); // reset, lower sera
        }
        else if (name == "sequenced_antigens") {      // mark sequenced antigens (removes old tracked antigens marking)
            std::vector<size_t> sequenced_indices(sequenced_antigens().size());
            std::transform(sequenced_antigens().begin(), sequenced_antigens().end(), sequenced_indices.begin(), [](const auto& src) -> size_t { return src.first; });
            chart_draw.modify(sequenced_indices, PointStyleDraw(PointStyle::Empty).fill(mod.get("fill", "grey63")).outline(mod.get("outline", "white")).outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
    }

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "tracked_sera") {
              // chart_draw.modify_all_sera(PointStyleDraw(PointStyle::Empty).outline(mod.get("outline", "black")).outline_width(Pixels{mod.get("outline_width", 0.5)}), false, true); // reset, lower sera
        }
        else if (name == "tracked_antigens") {
            std::vector<size_t> tracked_indices;
            for (const auto& sequenced_section: sequenced_antigens()) {
                if (sequenced_section.second == aSectionIndex)
                    tracked_indices.push_back(sequenced_section.first);
            }
            chart_draw.modify(tracked_indices, PointStyleDraw(PointStyle::Empty).fill(mod.get("fill", "grey63")).outline(mod.get("outline", "white")).outline_width(Pixels{mod.get("outline_width", 0.5)}), true);
        }
        else if (name == "vaccines") {
            mark_vaccines(chart_draw, mod);
        }
    }
      // vaccines
      // marked sera
      // marked antigens
      // serum circles
      // tracked_antigen_colored_by_clade

    const HzSection& section = hz_sections().sections[aSectionIndex];
    std::string title = section.index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    if (!section.label.empty())
        title += " " + section.label;
    chart_draw.title().remove_all_lines().add_line(title);

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_vaccines(ChartDraw& chart_draw, const AntigenicMapMod& vaccine_mod)
{
    std::cerr << "DEBUG: mark_vaccines" << std::endl;
    try {
        Vaccines vaccs{chart_draw.chart(), mHiDbSet.get(chart_draw.chart().chart_info().virus_type())};
        for (const SettingValue& mod_v: vaccine_mod.get_mods()) {
            std::cerr << "DEBUG: mark_vaccines mod" << std::endl;
            const SettingDict& mod = boost::get<SettingDict>(mod_v);
            const std::string type = mod.get("type", ""), passage = mod.get("passage", ""), name = mod.get("name", "");
            std::unique_ptr<VaccineMatcher> matcher{vaccs.match(name, type, passage)};
            for (const auto& item: mod) {
                if (item.first == "size")
                    matcher->size(boost::get<double>(item.second));
            }
        }
        std::cerr << "DEBUG: Vaccines:" << std::endl << vaccs.report(2) << std::endl;
        vaccs.plot(chart_draw);
    }
    catch (hidb::NoHiDb&) {
        std::cerr << "WARNING: cannot mark vaccines: no hidb for \"" << chart_draw.chart().chart_info().virus_type() << "\"" << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::mark_vaccines

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    apply_mods_before(aSurface);

      // aSurface.viewport(viewport());

    std::cout << "\nMAP: " << aSectionIndex << " " << aSurface.viewport() << std::endl;
    chart().draw(aSurface);

    apply_mods_after(aSurface);

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
