#include <typeinfo>

#include "acmacs-chart/ace.hh"
#include "tree-draw.hh"
#include "signature-page.hh"
#include "antigenic-maps-layout.hh"
#include "ace-antigenic-maps-draw.hh"
#include "sdb-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::AntigenicMapsDrawBase(Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
    : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections),
      mSignaturePageDrawSettings(aSignaturePageDrawSettings), mSettings(aSettings)
{

} // AntigenicMapsDrawBase::AntigenicMapsDrawBase

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::~AntigenicMapsDrawBase()
{
}

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::layout(AntigenicMapsLayout* aLayout)
{
    mLayout.reset(aLayout);

} // AntigenicMapsDrawBase::layout

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::init_settings()
{
    const size_t number_sections = hz_sections().shown_maps();
    switch (number_sections) {
      case 0:
      case 1:
      case 2:
      case 3:
          settings().columns = 1;
          break;
      case 4:
      case 5:
      case 6:
          settings().columns = 2;
          break;
      case 7:
      case 8:
      case 9:
          settings().columns = 3;
          break;
      default:
          settings().columns = 3;
          break;
    }
    size_t maps_per_column = number_sections / settings().columns + ((number_sections % settings().columns) == 0 ? 0 : 1);
    const double map_width = 150; // height is not available at this moment mSurface.viewport().size.height / (maps_per_column + settings().gap * (maps_per_column - 1));
    signature_page_settings().antigenic_maps_width = map_width * settings().columns + (settings().columns - 1) * settings().gap;

    std::cerr << "columns:" << settings().columns << " maps_per_column:" << maps_per_column << " map_width:" << map_width << " width:" << signature_page_settings().antigenic_maps_width << std::endl;

    chart().init_settings();

} // AntigenicMapsDrawBase::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::draw(Surface& aMappedAntigensDrawSurface)
{
    layout().draw(aMappedAntigensDrawSurface);

} // AntigenicMapsDrawBase::draw

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::prepare()
{
    chart().prepare(settings());
    if (settings().layout == "labelled_grid") {
        make_layout();
    }
    else {
        throw std::runtime_error("Unrecognized antigenic maps layout: " + settings().layout);
    }
    layout().prepare();

} // AntigenicMapsDrawBase::prepare

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
{
    std::string error;
    try {
        return new AntigenicMapsDraw(aSurface, aTree, import_chart(aChartFilename), aHzSections, aSignaturePageDrawSettings, aSettings);
    }
    catch (AceChartReadError& err) {
        error = std::string("[Not ACE] ") + err.what();
    }
    try {
        return new sdb::AntigenicMapsDraw(aSurface, aTree, sdb::read_chart_from_sdb(aChartFilename), aHzSections, aSignaturePageDrawSettings, aSettings);
    }
    catch (ChartReadError& err) {
        error += std::string("\n[Not SDB] ") + err.what();
    }
    throw ChartReadError(error);

} // make_antigenic_maps_draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
