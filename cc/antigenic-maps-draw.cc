#include "antigenic-maps-draw.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void AntigenicMapsDraw::init_settings(const HzSections& aHzSections)
{
    const size_t number_sections = aHzSections.sections.size();
    switch (number_sections) {
      case 0:
      case 1:
      case 2:
      case 3:
          mSettings.columns = 1;
          break;
      case 4:
      case 5:
      case 6:
          mSettings.columns = 2;
          break;
      case 7:
      case 8:
      case 9:
          mSettings.columns = 3;
          break;
      default:
          mSettings.columns = 3;
          break;
    }
    size_t maps_per_column = number_sections / mSettings.columns + ((number_sections % mSettings.columns) == 0 ? 0 : 1);
    const double map_width = 150; // height is not available at this moment mSurface.size().height / (maps_per_column + mSettings.gap * (maps_per_column - 1));
    mSettings.width = map_width * mSettings.columns + (mSettings.columns - 1) * mSettings.gap;

    std::cerr << "columns:" << mSettings.columns << " maps_per_column:" << maps_per_column << " map_width:" << map_width << " width:" << mSettings.width << std::endl;

    mChart.init_settings();

} // AntigenicMapsDraw::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::prepare()
{

} // AntigenicMapsDraw::prepare

// ----------------------------------------------------------------------

void AntigenicMapsDraw::draw()
{
    mSurface.border("blue", 2);

} // AntigenicMapsDraw::draw


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
