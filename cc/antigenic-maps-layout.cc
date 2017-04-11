#include "antigenic-maps-layout.hh"
#include "chart-draw.hh"
#include "antigenic-maps-draw.hh"
#include "tree.hh"
//#include "tree-draw.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

AntigenicMapsLayout::~AntigenicMapsLayout()
{

} // AntigenicMapsLayout::~AntigenicMapsLayout

// ----------------------------------------------------------------------

void AntigenicMapsLayout::prepare()
{
    if (!mAntigenicMapsDraw.settings().viewport.empty()) {
        mMapViewport = mAntigenicMapsDraw.settings().viewport;
    }
    else {
        mMapViewport = mAntigenicMapsDraw.chart().viewport(&mAntigenicMapsDraw.settings().transformation);
    }
    std::cout << "Using viewport: " << mMapViewport << std::endl;

    find_sequenced_antigens();
    reset();

} // AntigenicMapsLayout::prepare

// ----------------------------------------------------------------------

void AntigenicMapsLayout::find_sequenced_antigens()
{
    size_t hz_section_index = NodeDrawData::HzSectionNoIndex;
    auto find_antigens = [this,&hz_section_index](const Node& aNode) {
        if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex)
            hz_section_index = aNode.draw.hz_section_index;
        if (aNode.draw.chart_antigen_index != ChartDrawBase::AntigenNotFound)
            mSequencedAntigens[aNode.draw.chart_antigen_index] = hz_section_index;
    };

    tree::iterate_leaf(mAntigenicMapsDraw.tree(), find_antigens);

} // AntigenicMapsLayout::find_sequenced_antigens

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
