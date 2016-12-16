#include "geographic-map.hh"
#include "geographic-path.inc"
#include "coloring.hh"
#include "surface.hh"

// ----------------------------------------------------------------------

void ColoringByGeographyMapLegend::draw(Surface& aSurface) const
{
    Color geographic_map_outline_color = 0;
    double geographic_map_outline_width = 1;
    // cairo_path_t* map_outline = outline(aSurface, geographic_map_path);
    // const double scale = aSurface.draw_path_scale(map_outline, aViewport);
    aSurface.path_outline(geographic_map_path.begin(), geographic_map_path.end(), geographic_map_outline_color, geographic_map_outline_width);
    // for (const auto& label: ColoringByContinentLegendLabels) {
    //     cairo_path_t* continent_outline = outline(aSurface, continent_path(label));
    //     aSurface.draw_path(continent_outline, aViewport, scale, mColoring.color(label), aSettings.geographic_map_outline_width);
    //     aSurface.destroy_path(continent_outline);
    // }

} // ColoringByGeographyMapLegend::draw

// ----------------------------------------------------------------------

// Size ColoringByGeographyMapLegend::size(Surface& aSurface, const SettingsLegend& aSettings) const
// {
//     const double map_height = aSurface.canvas_size().height * aSettings.geographic_map_fraction;
//     return Size(geographic_map_size[0] / geographic_map_size[1] * map_height, map_height);

// } // ColoringByGeographyMapLegend::size

// ----------------------------------------------------------------------

// cairo_path_t* ColoringByGeographyMapLegend::outline(Surface& aSurface, const std::vector<GeographicMapPathElement>& aPath) const
// {
//     aSurface.new_path();
//     for (const auto& element: aPath) {
//         if (element.x < 0) {
//               //aSurface.close_path();
//             aSurface.move_to(- element.x, element.y);
//         }
//         else {
//             aSurface.line_to(element.x, element.y);
//         }
//     }
//     auto path = aSurface.copy_path();
//     aSurface.new_path();
//     return path;

// } // ColoringByGeographyMapLegend::outline

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
