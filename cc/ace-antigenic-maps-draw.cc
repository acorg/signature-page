#include "ace-antigenic-maps-draw.hh"

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
      // ref antigens grey and lower
      // test antigens grey

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
    }
      // mark sequenced antigens (removes old tracked antigens marking)

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "tracked_sera") {
              // chart_draw.modify_all_sera(PointStyleDraw(PointStyle::Empty).outline(mod.get("outline", "black")).outline_width(Pixels{mod.get("outline_width", 0.5)}), false, true); // reset, lower sera
        }
    }
      // mark tracked antigens
      // vaccines
      // marked antigens

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

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
