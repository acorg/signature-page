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
    apply_mods_before(sett.mods, aSurface);

      // aSurface.viewport(viewport());

    std::cout << "\nMAP: " << aSectionIndex << " " << aSurface.viewport() << std::endl;
    chart().draw(aSurface);

    apply_mods_after(sett.mods, aSurface);

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::reset()
{

} // AntigenicMapsLayoutDrawAce::reset

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
