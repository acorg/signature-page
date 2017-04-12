#include "sdb-antigenic-maps-draw.hh"
#include "tree-draw.hh"
#include "mapped-antigens-draw.hh"
#include "tree-iterate.hh"
#include "signature-page.hh"

// ----------------------------------------------------------------------

void sdb::AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // sdb::AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void sdb::DrawSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.square_filled(aPoint.coordinates, Pixels{aSettings.serum_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), NoRotation, outline_color(aPoint, aStyle, aSettings),
                               Pixels{outline_width(aPoint, aStyle, aSettings) * aSettings.point_scale}, "transparent");
    }

} // sdb::DrawSerum::draw

// ----------------------------------------------------------------------

void sdb::DrawReferenceAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.reference_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.reference_antigen_outline_color,
                               Pixels{aSettings.reference_antigen_outline_width * aSettings.point_scale}, "transparent");
    }

} // sdb::DrawReferenceAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTestAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.test_antigen_outline_color,
                               Pixels{aSettings.test_antigen_outline_width * aSettings.point_scale}, aSettings.test_antigen_fill_color);
    }

} // sdb::DrawTestAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawSequencedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.sequenced_antigen_outline_color,
                               Pixels{aSettings.sequenced_antigen_outline_width * aSettings.point_scale}, aSettings.sequenced_antigen_fill_color);
    }

} // sdb::DrawSequencedAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTrackedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.tracked_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.tracked_antigen_outline_color,
                               Pixels{aSettings.tracked_antigen_outline_width * aSettings.point_scale}, mColor);
    }

} // sdb::DrawTrackedAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTrackedSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    DrawSerum::draw(aSurface, aPoint, aStyle, aSettings);
    // std::cout << "    Tracked serum " << aPoint.name << " radius:" << aPoint.attributes.serum_circle_radius << std::endl;
    aSurface.circle(aPoint.coordinates, Scaled{aPoint.attributes.serum_circle_radius * 2}, AspectNormal, NoRotation,
                    outline_color(aPoint, aStyle, aSettings), // aSettings.serum_circle_color,
                    Pixels{aSettings.serum_circle_thickness * aSettings.point_scale});

} // sdb::DrawTrackedSerum::draw

// ----------------------------------------------------------------------

void sdb::DrawVaccineAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const DrawPointSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        std::cout << "    Vaccine " << aPoint.name << " " << fill_color(aPoint, aStyle, aSettings) << std::endl;
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.vaccine_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.vaccine_antigen_outline_color, // aPoint.attributes.vaccine.outline_color,
                               Pixels{aSettings.vaccine_antigen_outline_width * aSettings.point_scale},
                               fill_color(aPoint, aStyle, aSettings) /*aPoint.attributes.vaccine.fill_color*/);
    }

} // sdb::DrawVaccineAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawMarkedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& /*aStyle*/, const DrawPointSettings& aSettings) const
{
    // if (!aPoint.coordinates.isnan()) {
    //     aSurface.circle_filled(aPoint.coordinates, Pixels{mData.scale * aSettings.point_scale},
    //                            Aspect{mData.aspect}, Rotation{mData.rotation}, mData.outline_color,
    //                            Pixels{mData.outline_width * aSettings.point_scale}, mData.fill_color);
    //     if (!mData.label.empty()) {
    //         const TextStyle style; //(mData.label_font);
    //         const auto text_size = aSurface.text_size(mData.label, Pixels{mData.label_size}, style);
    //         const Location text_origin = aPoint.coordinates + mData.label_offset;
    //         aSurface.text(text_origin, mData.label, mData.label_color, Pixels{mData.label_size}, style);
    //         const Location text_middle = text_origin + Size(text_size.width / 2, text_origin.y > aPoint.coordinates.y ? - text_size.height * 1.2 : text_size.height * 0.2);
    //         std::cerr << "    Mark antigen " << aPoint.name << " " << aPoint.coordinates << " " << text_middle << std::endl;
    //         aSurface.line(aPoint.coordinates, text_middle, mData.label_line_color, Pixels{mData.label_line_width});
    //     }
    // }

} // sdb::DrawMarkedAntigen::draw

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::reset()
{
    mDrawTrackedAntigen.color("green3" /* settings().tracked_antigen_color */);
    auto& chart = dynamic_cast<sdb::AntigenicMapsDraw&>(antigenic_maps_draw()).chart_sdb();

    mDrawPoints.resize(chart.points().size(), nullptr);
    for (size_t point_no = 0; point_no < chart.points().size(); ++point_no) {
        const auto& p = chart.points()[point_no];
        if (p.attributes.antigen) {
            if (p.attributes.vaccine) {
                mDrawPoints[point_no] = &mDrawVaccineAntigen;
            }
            else if (sequenced_antigens().find(point_no) != sequenced_antigens().end()) {
                mDrawPoints[point_no] = &mDrawSequencedAntigen;
            }
            else if (p.attributes.reference) {
                mDrawPoints[point_no] = &mDrawReferenceAntigen;
            }
            else {
                mDrawPoints[point_no] = &mDrawTestAntigen;
            }
        }
        else {
            mDrawPoints[point_no] = &mDrawSerum;
        }
    }

    mark_marked_antigens();

} // sdb::AntigenicMapsLayoutDraw::reset

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::mark_marked_antigens()
{
    // const auto& mark_antigens = settings().mark_antigens;
    // mDrawMarkedAntigens.clear();
    // mDrawMarkedAntigens.reserve(mark_antigens.size()); // to avoid copying entries during emplace_back and loosing pointer for mDrawPoints

    // for (const auto& to_mark: mark_antigens) {
    //     if (to_mark.show) {
    //         const size_t point_no = chart().find_antigen(to_mark.name);
    //         if (point_no != sdb::Chart::AntigenNotFound) {
    //             mDrawMarkedAntigens.emplace_back(to_mark);
    //             mDrawPoints[point_no] = &mDrawMarkedAntigens.back();
    //         }
    //     }
    // }

} // sdb::AntigenicMapsLayoutDraw::mark_marked_antigens

// ----------------------------------------------------------------------

sdb::DrawPointSettings::DrawPointSettings()
    : point_scale(1), serum_scale(5), reference_antigen_scale(5), test_antigen_scale(3), vaccine_antigen_scale(15), tracked_antigen_scale(5),
      serum_outline_width(0.5), reference_antigen_outline_width(0.5), test_antigen_outline_width(0.5), vaccine_antigen_outline_width(0.5),
      sequenced_antigen_outline_width(0.5), tracked_antigen_outline_width(0.5),
      serum_outline_color("grey88"), reference_antigen_outline_color("grey88"), test_antigen_outline_color("grey88"),
      test_antigen_fill_color("grey88"), vaccine_antigen_outline_color("white"), sequenced_antigen_outline_color("white"), sequenced_antigen_fill_color("grey63"),
      tracked_antigen_outline_color("white"), tracked_antigen_colored_by_clade(false), tracked_antigen_color("green3"),
      reassortant_rotation(0.5 /* M_PI / 6.0 */), egg_antigen_aspect(0.75),
      show_tracked_sera(false), serum_circle_color("grey50"), tracked_serum_outline_color("black"), serum_circle_thickness(0.1), tracked_serum_outline_width(0.1),
      map_title_color("black"), map_title_offset{0.3, 0.4}, map_title_size(11)
{
}

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::apply_mods_to_settings(const std::vector<AntigenicMapMod>& aMods, DrawPointSettings& aSettings)
{
    for (const auto& mod: aMods) {
        const std::string name = mod.name();
        if (name == "point_scale") {
            aSettings.point_scale = mod.get("scale", 1.0);
        }
    }

} // sdb::AntigenicMapsLayoutDraw::apply_mods_to_settings

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    std::cout << "\nMAP: " << aSectionIndex << std::endl;

    apply_mods_before(settings().mods, aSurface);
    DrawPointSettings draw_point_settings;
    apply_mods_to_settings(settings().mods, draw_point_settings);

    auto& chart = dynamic_cast<sdb::AntigenicMapsDraw&>(antigenic_maps_draw()).chart_sdb();

    aSurface.viewport(viewport());

    mark_tracked_antigens(aSectionIndex, draw_point_settings);
    mark_tracked_sera(aSectionIndex, draw_point_settings);

    size_t drawn = 0;
    for (size_t level = 0; level < 10 && drawn < mDrawPoints.size(); ++level) {
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
           if (mDrawPoints[point_no]->level() == level) {
               mDrawPoints[point_no]->draw(aSurface, chart.points()[point_no], chart.plot_style().style(point_no), draw_point_settings);
                ++drawn;
            }
        }
    }
    if (drawn != mDrawPoints.size())
        std::cerr << "AntigenicMapsLayoutDraw:0: warning: " << drawn << " points of " << mDrawPoints.size() << " were drawn" << std::endl;

    draw_map_title(aSurface, aSectionIndex, draw_point_settings);
    apply_mods_after(settings().mods, aSurface);

} // sdb::AntigenicMapsLayoutDraw::draw_chart

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::mark_tracked_sera(size_t aSectionIndex, const DrawPointSettings& aSettings)
{
    if (aSettings.show_tracked_sera) {
        auto& chart = dynamic_cast<sdb::AntigenicMapsDraw&>(antigenic_maps_draw()).chart_sdb();
        const std::vector<sdb::Point>& points = chart.points();
        mDrawTrackedSera.clear();
        mDrawTrackedSera.reserve(mDrawPoints.size()); // to avoid copying entries during emplace_back and loosing pointer for mDrawPoints
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
            const auto& point = points[point_no];
            if (!point.attributes.antigen && point.attributes.homologous_antigen >= 0) {
                const auto sequenced = sequenced_antigens().find(static_cast<size_t>(point.attributes.homologous_antigen));
                if (sequenced != sequenced_antigens().end() && sequenced->second == aSectionIndex) {

                      // std::cout << "tracked serum " << point.name << std::endl;
                    Color color = aSettings.tracked_serum_outline_color;
                    double outline_width = aSettings.tracked_serum_outline_width;
                      // for (const auto& entry: infix_colors) {
                      //     if (mDrawPoints[point_no].name.find(entry.first) != std::string::npos) {
                      //         color = entry.second;
                      //         break;
                      //     }
                      // }
                    mDrawTrackedSera.emplace_back(color, outline_width);
                    mDrawPoints[point_no] = &mDrawTrackedSera.back();
                    std::cout << "    Tracked serum " << point.name << " radius:" << point.attributes.serum_circle_radius << std::endl;
                }
                else {
                    mDrawPoints[point_no] = &mDrawSerum;
                }
            }
        }
    }

} // sdb::AntigenicMapsLayoutDraw::mark_tracked_sera

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::draw_map_title(Surface& aSurface, size_t aSectionIndex, const DrawPointSettings& aSettings)
{
    const Pixels map_title_size{aSettings.map_title_size};
    const Size map_title_offset = aSettings.map_title_offset;
    const HzSection& section = hz_sections().sections[aSectionIndex];
    std::string title = section.index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    if (!section.label.empty())
        title += " " + section.label;
    const Size wsize = aSurface.text_size("W", map_title_size);
    const Size tsize = aSurface.text_size(title, map_title_size);
    aSurface.text({map_title_offset.width * wsize.width + aSurface.viewport().origin.x,
                    map_title_offset.height * wsize.height + tsize.height + aSurface.viewport().origin.y},
        title, aSettings.map_title_color, map_title_size);

} // sdb::AntigenicMapsLayoutDraw::draw_map_title

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::mark_tracked_antigens(size_t aSectionIndex, const DrawPointSettings& aSettings)
{
    auto& chart = dynamic_cast<sdb::AntigenicMapsDraw&>(antigenic_maps_draw()).chart_sdb();
    for (size_t point_no = 0; point_no < chart.points().size(); ++point_no) {
        if (mDrawPoints[point_no] == &mDrawTrackedAntigen || mDrawPoints[point_no] == &mDrawSequencedAntigen) {
            const auto sequenced = sequenced_antigens().find(point_no);
            if (sequenced != sequenced_antigens().end()) {
                if (sequenced->second == aSectionIndex)
                    mDrawPoints[point_no] = &mDrawTrackedAntigen;
                else
                    mDrawPoints[point_no] = &mDrawSequencedAntigen;
            }
        }
    }

} // sdb::AntigenicMapsLayoutDraw::mark_tracked_antigens

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
