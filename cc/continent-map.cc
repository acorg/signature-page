#include "continent-map.hh"
#include "continent-path.hh"

// ----------------------------------------------------------------------

void ColoringByContinentMapLegend::draw(Surface& aSurface) const
{
      // aSurface.border(0xA0FFA000, 10);
    // Color geographic_map_outline_color = 0;
    // double geographic_map_outline_width = 1;
    for (const auto& continent: ColoringByContinent::ContinentLabels) {
        const auto& path = continent_map_path(continent);
          // aSurface.path_outline(path.first, path.second, geographic_map_outline_color, geographic_map_outline_width);
        aSurface.path_fill(path.first, path.second, mColoring.color(continent));
    }

} // ColoringByContinentMapLegend::draw

// ----------------------------------------------------------------------

Size ColoringByContinentMapLegend::size() const
{
    return {continent_map_size[0], continent_map_size[1]};

} // ColoringByContinentMapLegend::size

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
