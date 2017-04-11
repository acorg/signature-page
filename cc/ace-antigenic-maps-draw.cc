#include "ace-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

void ChartDraw::init_settings()
{

} // ChartDraw::init_settings

// ----------------------------------------------------------------------

void ChartDraw::prepare(const AntigenicMapsDrawSettings& aSettings)
{

} // ChartDraw::prepare

// ----------------------------------------------------------------------

const Viewport& ChartDraw::viewport(const Transformation* aSettingsTransformation)
{

} // ChartDraw::viewport

// ----------------------------------------------------------------------

void AntigenicMapsDraw::init_settings()
{

} // AntigenicMapsDraw::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(Surface& aSurface, size_t aSectionIndex)
{

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::reset()
{

} // AntigenicMapsLayoutDrawAce::reset

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
