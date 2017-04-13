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
    auto& chart_draw = dynamic_cast<ChartDrawInterface&>(antigenic_maps_draw().chart());
      // std::cerr << "INFO: [sdb] prepare_apply_mods 1 transformation " << chart.transformation() << std::endl;
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "rotate_degrees") {
            chart_draw.chart_draw().rotate(mod.get("angle", 0.0) * M_PI / 180.0);
        }
        else if (name == "rotate_radians") {
            chart_draw.chart_draw().rotate(mod.get("angle", 0.0));
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

} // AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_drawing_chart(size_t /*aSectionIndex*/)
{

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
