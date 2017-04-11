#include "sdb-antigenic-maps-draw.hh"
#include "tree-draw.hh"
#include "mapped-antigens-draw.hh"
#include "tree-iterate.hh"
#include "signature-page.hh"

// ----------------------------------------------------------------------

void sdb::AntigenicMapsDraw::init_settings()
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

} // sdb::AntigenicMapsDraw::init_settings

// ----------------------------------------------------------------------

void sdb::AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // sdb::AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void sdb::AntigenicMapsDraw::draw(Surface& aMappedAntigensDrawSurface)
{
    layout().draw(aMappedAntigensDrawSurface);

} // sdb::AntigenicMapsDraw::draw

// ----------------------------------------------------------------------

void sdb::DrawSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.square_filled(aPoint.coordinates, Pixels{aSettings.serum_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), NoRotation, outline_color(aPoint, aStyle, aSettings),
                               Pixels{outline_width(aPoint, aStyle, aSettings) * aSettings.point_scale}, "transparent");
    }

} // sdb::DrawSerum::draw

// ----------------------------------------------------------------------

void sdb::DrawReferenceAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.reference_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.reference_antigen_outline_color,
                               Pixels{aSettings.reference_antigen_outline_width * aSettings.point_scale}, "transparent");
    }

} // sdb::DrawReferenceAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTestAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.test_antigen_outline_color,
                               Pixels{aSettings.test_antigen_outline_width * aSettings.point_scale}, aSettings.test_antigen_fill_color);
    }

} // sdb::DrawTestAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawSequencedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.sequenced_antigen_outline_color,
                               Pixels{aSettings.sequenced_antigen_outline_width * aSettings.point_scale}, aSettings.sequenced_antigen_fill_color);
    }

} // sdb::DrawSequencedAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTrackedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.tracked_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.tracked_antigen_outline_color,
                               Pixels{aSettings.tracked_antigen_outline_width * aSettings.point_scale}, mColor);
    }

} // sdb::DrawTrackedAntigen::draw

// ----------------------------------------------------------------------

void sdb::DrawTrackedSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    DrawSerum::draw(aSurface, aPoint, aStyle, aSettings);
    // std::cout << "    Tracked serum " << aPoint.name << " radius:" << aPoint.attributes.serum_circle_radius << std::endl;
    aSurface.circle(aPoint.coordinates, Scaled{aPoint.attributes.serum_circle_radius * 2}, AspectNormal, NoRotation,
                    outline_color(aPoint, aStyle, aSettings), // aSettings.serum_circle_color,
                    Pixels{aSettings.serum_circle_thickness * aSettings.point_scale});

} // sdb::DrawTrackedSerum::draw

// ----------------------------------------------------------------------

void sdb::DrawVaccineAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
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

void sdb::DrawMarkedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{mData.scale * aSettings.point_scale},
                               Aspect{mData.aspect}, Rotation{mData.rotation}, mData.outline_color,
                               Pixels{mData.outline_width * aSettings.point_scale}, mData.fill_color);
        if (!mData.label.empty()) {
            const TextStyle style; //(mData.label_font);
            const auto text_size = aSurface.text_size(mData.label, Pixels{mData.label_size}, style);
            const Location text_origin = aPoint.coordinates + mData.label_offset;
            aSurface.text(text_origin, mData.label, mData.label_color, Pixels{mData.label_size}, style);
            const Location text_middle = text_origin + Size(text_size.width / 2, text_origin.y > aPoint.coordinates.y ? - text_size.height * 1.2 : text_size.height * 0.2);
            std::cerr << "    Mark antigen " << aPoint.name << " " << aPoint.coordinates << " " << text_middle << std::endl;
            aSurface.line(aPoint.coordinates, text_middle, mData.label_line_color, Pixels{mData.label_line_width});
        }
    }

} // sdb::DrawMarkedAntigen::draw

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::reset()
{
    mDrawTrackedAntigen.color(settings().tracked_antigen_color);
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
    const auto& mark_antigens = settings().mark_antigens;
    mDrawMarkedAntigens.clear();
    mDrawMarkedAntigens.reserve(mark_antigens.size()); // to avoid copying entries during emplace_back and loosing pointer for mDrawPoints

    for (const auto& to_mark: mark_antigens) {
        if (to_mark.show) {
            const size_t point_no = chart().find_antigen(to_mark.name);
            if (point_no != sdb::Chart::AntigenNotFound) {
                mDrawMarkedAntigens.emplace_back(to_mark);
                mDrawPoints[point_no] = &mDrawMarkedAntigens.back();
            }
        }
    }

} // sdb::AntigenicMapsLayoutDraw::mark_marked_antigens

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    std::cout << "\nMAP: " << aSectionIndex << std::endl;

    const AntigenicMapsDrawSettings& sett = settings();
    auto& chart = dynamic_cast<sdb::AntigenicMapsDraw&>(antigenic_maps_draw()).chart_sdb();

    aSurface.background(sett.background_color);
    aSurface.grid(Scaled{1}, sett.grid_line_color, Pixels{sett.grid_line_width});
    aSurface.viewport(viewport());

    mark_tracked_antigens(aSectionIndex);
    mark_tracked_sera(aSectionIndex);

    size_t drawn = 0;
    for (size_t level = 0; level < 10 && drawn < mDrawPoints.size(); ++level) {
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
           if (mDrawPoints[point_no]->level() == level) {
               mDrawPoints[point_no]->draw(aSurface, chart.points()[point_no], chart.plot_style().style(point_no), sett);
                ++drawn;
            }
        }
    }
    if (drawn != mDrawPoints.size())
        std::cerr << "AntigenicMapsLayoutDraw:0: warning: " << drawn << " points of " << mDrawPoints.size() << " were drawn" << std::endl;

    draw_map_title(aSurface, aSectionIndex);
    aSurface.border(sett.border_color, Pixels{sett.border_width});

} // sdb::AntigenicMapsLayoutDraw::draw_chart

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::mark_tracked_sera(size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& sett = settings();
    if (sett.show_tracked_sera) {
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
                    Color color = sett.tracked_serum_outline_color;
                    double outline_width = sett.tracked_serum_outline_width;
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

void sdb::AntigenicMapsLayoutDraw::draw_map_title(Surface& aSurface, size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& sett = settings();
    const HzSection& section = hz_sections().sections[aSectionIndex];
    std::string title = section.index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    if (!section.label.empty())
        title += " " + section.label;
    const Size wsize = aSurface.text_size("W", Pixels{sett.map_title_size});
    const Size tsize = aSurface.text_size(title, Pixels{sett.map_title_size});
    aSurface.text({sett.map_title_offset.width * wsize.width + aSurface.viewport().origin.x,
                    sett.map_title_offset.height * wsize.height + tsize.height + aSurface.viewport().origin.y},
        title, sett.map_title_color, Pixels{sett.map_title_size});

} // sdb::AntigenicMapsLayoutDraw::draw_map_title

// ----------------------------------------------------------------------

void sdb::AntigenicMapsLayoutDraw::mark_tracked_antigens(size_t aSectionIndex)
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
