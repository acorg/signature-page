#include "sdb-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::~AntigenicMapsDrawBase()
{
}

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
{
    try {
        return new sdb::AntigenicMapsDraw(aSurface, aTree, sdb::read_chart_from_sdb(aChartFilename), aHzSections, aSignaturePageDrawSettings, aSettings);
    }
    catch (ChartReadError& err) {
        std::cerr << "Cannot read sdb chart" << std::endl;
        throw;
    }

} // make_antigenic_maps_draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
