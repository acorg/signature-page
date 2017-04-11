#include "ace-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

void ChartDrawInterface::init_settings()
{

} // ChartDrawInterface::init_settings

// ----------------------------------------------------------------------

void ChartDrawInterface::prepare(const AntigenicMapsDrawSettings& aSettings)
{
    mChartDraw.prepare();

} // ChartDrawInterface::prepare

// ----------------------------------------------------------------------

const Viewport& ChartDrawInterface::viewport(const Transformation* aSettingsTransformation)
{

} // ChartDrawInterface::viewport

// ----------------------------------------------------------------------

void AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& sett = settings();
    aSurface.background(sett.background_color);
    aSurface.grid(Scaled{1}, sett.grid_line_color, Pixels{sett.grid_line_width});
      // aSurface.viewport(viewport());

    std::cout << "\nMAP: " << aSectionIndex << " " << aSurface.viewport() << std::endl;
    chart().draw(aSurface);

    aSurface.border(sett.border_color, Pixels{sett.border_width});

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::reset()
{

} // AntigenicMapsLayoutDrawAce::reset

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
