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
    std::cerr << "INFO: AntigenicMapsLayoutDrawAce::prepare_apply_mods" << std::endl;
    dynamic_cast<ChartDrawInterface&>(chart()).calculate_viewport();

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "point_scale") {
              // aSettings.point_scale = mod.get("scale", 1.0);
        }
    }

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

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

void AntigenicMapsLayoutDrawAce::prepare_drawing_char()
{

} // AntigenicMapsLayoutDrawAce::prepare_drawing_char

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
