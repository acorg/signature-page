#include <typeinfo>

#include "acmacs-chart/ace.hh"
#include "ace-antigenic-maps-draw.hh"
#include "sdb-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::~AntigenicMapsDrawBase()
{
}

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
