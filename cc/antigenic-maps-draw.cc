#include "antigenic-maps-draw.hh"
#include "tree-draw.hh"
#include "mapped-antigens-draw.hh"
#include "tree-iterate.hh"
#include "signature-page.hh"

// ----------------------------------------------------------------------

void AntigenicMapsDraw::init_settings()
{
    const size_t number_sections = mHzSections.shown_maps();
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
    const double map_width = 150; // height is not available at this moment mSurface.viewport().size.height / (maps_per_column + mSettings.gap * (maps_per_column - 1));
    mSignaturePageDrawSettings.antigenic_maps_width = map_width * mSettings.columns + (mSettings.columns - 1) * mSettings.gap;

    std::cerr << "columns:" << mSettings.columns << " maps_per_column:" << maps_per_column << " map_width:" << map_width << " width:" << mSignaturePageDrawSettings.antigenic_maps_width << std::endl;

    mChart.init_settings();

} // AntigenicMapsDraw::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::prepare()
{
    if (mSettings.layout == "labelled_grid") {
        mLayout = std::make_unique<LabelledGrid>(*this);
    }
    else {
        throw std::runtime_error("Unrecognized antigenic maps layout: " + mSettings.layout);
    }
    mLayout->prepare();

} // AntigenicMapsDraw::prepare

// ----------------------------------------------------------------------

void AntigenicMapsDraw::draw()
{
    mLayout->draw();

} // AntigenicMapsDraw::draw

// ----------------------------------------------------------------------

void DrawSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.square_filled(aPoint.coordinates, Pixels{aSettings.serum_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), NoRotation, outline_color(aPoint, aStyle, aSettings),
                               Pixels{outline_width(aPoint, aStyle, aSettings) * aSettings.point_scale}, "transparent");
    }

} // DrawSerum::draw

// ----------------------------------------------------------------------

void DrawReferenceAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.reference_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.reference_antigen_outline_color,
                               Pixels{aSettings.reference_antigen_outline_width * aSettings.point_scale}, "transparent");
    }

} // DrawReferenceAntigen::draw

// ----------------------------------------------------------------------

void DrawTestAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.test_antigen_outline_color,
                               Pixels{aSettings.test_antigen_outline_width * aSettings.point_scale}, aSettings.test_antigen_fill_color);
    }

} // DrawTestAntigen::draw

// ----------------------------------------------------------------------

void DrawSequencedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.test_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.sequenced_antigen_outline_color,
                               Pixels{aSettings.sequenced_antigen_outline_width * aSettings.point_scale}, aSettings.sequenced_antigen_fill_color);
    }

} // DrawSequencedAntigen::draw

// ----------------------------------------------------------------------

void DrawTrackedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.tracked_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.tracked_antigen_outline_color,
                               Pixels{aSettings.tracked_antigen_outline_width * aSettings.point_scale}, mColor);
    }

} // DrawTrackedAntigen::draw

// ----------------------------------------------------------------------

void DrawTrackedSerum::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    DrawSerum::draw(aSurface, aPoint, aStyle, aSettings);
    // std::cout << "    Tracked serum " << aPoint.name << " radius:" << aPoint.attributes.serum_circle_radius << std::endl;
    aSurface.circle(aPoint.coordinates, Scaled{aPoint.attributes.serum_circle_radius * 2}, AspectNormal, NoRotation,
                    outline_color(aPoint, aStyle, aSettings), // aSettings.serum_circle_color,
                    Pixels{aSettings.serum_circle_thickness * aSettings.point_scale});

} // DrawTrackedSerum::draw

// ----------------------------------------------------------------------

void DrawVaccineAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& aStyle, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        std::cout << "    Vaccine " << aPoint.name << " " << fill_color(aPoint, aStyle, aSettings) << std::endl;
        aSurface.circle_filled(aPoint.coordinates, Pixels{aSettings.vaccine_antigen_scale * aSettings.point_scale},
                               aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.vaccine_antigen_outline_color, // aPoint.attributes.vaccine.outline_color,
                               Pixels{aSettings.vaccine_antigen_outline_width * aSettings.point_scale},
                               fill_color(aPoint, aStyle, aSettings) /*aPoint.attributes.vaccine.fill_color*/);
    }

} // DrawVaccineAntigen::draw

// ----------------------------------------------------------------------

void DrawMarkedAntigen::draw(Surface& aSurface, const sdb::Point& aPoint, const sdb::PointStyle& /*aStyle*/, const AntigenicMapsDrawSettings& aSettings) const
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

} // DrawMarkedAntigen::draw

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
    draw_points_reset();

    // mPrefixName.clear();
    // for (const auto& p: mPoints) {
    //     mPrefixName.insert(p.name.substr(0, p.name.find(1, ' ')));
    // }


} // AntigenicMapsLayout::prepare

// ----------------------------------------------------------------------

void AntigenicMapsLayout::find_sequenced_antigens()
{
    size_t hz_section_index = NodeDrawData::HzSectionNoIndex;
    auto find_antigens = [this,&hz_section_index](const Node& aNode) {
        if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex)
            hz_section_index = aNode.draw.hz_section_index;
        if (aNode.draw.chart_antigen_index != sdb::Chart::AntigenNotFound)
            mSequencedAntigens[aNode.draw.chart_antigen_index] = hz_section_index;
    };

    tree::iterate_leaf(mAntigenicMapsDraw.tree(), find_antigens);

} // AntigenicMapsLayout::find_sequenced_antigens

// ----------------------------------------------------------------------

void AntigenicMapsLayout::draw_points_reset()
{
    mDrawTrackedAntigen.color(mAntigenicMapsDraw.settings().tracked_antigen_color);

    mDrawPoints.resize(mAntigenicMapsDraw.chart().points().size(), nullptr);
    for (size_t point_no = 0; point_no < mAntigenicMapsDraw.chart().points().size(); ++point_no) {
        const auto& p = mAntigenicMapsDraw.chart().points()[point_no];
        if (p.attributes.antigen) {
            if (p.attributes.vaccine) {
                mDrawPoints[point_no] = &mDrawVaccineAntigen;
            }
            else if (mSequencedAntigens.find(point_no) != mSequencedAntigens.end()) {
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

} // AntigenicMapsLayout::draw_points_reset

// ----------------------------------------------------------------------

void AntigenicMapsLayout::mark_marked_antigens()
{
    const auto& mark_antigens = mAntigenicMapsDraw.settings().mark_antigens;
    mDrawMarkedAntigens.clear();
    mDrawMarkedAntigens.reserve(mark_antigens.size()); // to avoid copying entries during emplace_back and loosing pointer for mDrawPoints

    for (const auto& to_mark: mark_antigens) {
        if (to_mark.show) {
            const size_t point_no = mAntigenicMapsDraw.chart().find_antigen(to_mark.name);
            if (point_no != sdb::Chart::AntigenNotFound) {
                mDrawMarkedAntigens.emplace_back(to_mark);
                mDrawPoints[point_no] = &mDrawMarkedAntigens.back();
            }
        }
    }

} // AntigenicMapsLayout::mark_marked_antigens

// ----------------------------------------------------------------------

void AntigenicMapsLayout::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    std::cout << "\nMAP: " << aSectionIndex << std::endl;

    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    const sdb::Chart& chart = mAntigenicMapsDraw.chart();

    aSurface.background(settings.background_color);
    aSurface.grid(Scaled{1}, settings.grid_line_color, Pixels{settings.grid_line_width});
    aSurface.viewport(mMapViewport);

    mark_tracked_antigens(aSectionIndex);
    mark_tracked_sera(aSectionIndex);

    size_t drawn = 0;
    for (size_t level = 0; level < 10 && drawn < mDrawPoints.size(); ++level) {
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
           if (mDrawPoints[point_no]->level() == level) {
               mDrawPoints[point_no]->draw(aSurface, chart.points()[point_no], chart.plot_style().style(point_no), settings);
                ++drawn;
            }
        }
    }
    if (drawn != mDrawPoints.size())
        std::cerr << "AntigenicMapsLayout:0: warning: " << drawn << " points of " << mDrawPoints.size() << " were drawn" << std::endl;

    draw_map_title(aSurface, aSectionIndex);
    aSurface.border(settings.border_color, Pixels{settings.border_width});

} // AntigenicMapsLayout::draw_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayout::mark_tracked_sera(size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    if (settings.show_tracked_sera) {
        const std::vector<sdb::Point>& points = mAntigenicMapsDraw.chart().points();
        mDrawTrackedSera.clear();
        mDrawTrackedSera.reserve(mDrawPoints.size()); // to avoid copying entries during emplace_back and loosing pointer for mDrawPoints
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
            const auto& point = points[point_no];
            if (!point.attributes.antigen && point.attributes.homologous_antigen >= 0) {
                const auto sequenced = mSequencedAntigens.find(static_cast<size_t>(point.attributes.homologous_antigen));
                if (sequenced != mSequencedAntigens.end() && sequenced->second == aSectionIndex) {

                      // std::cout << "tracked serum " << point.name << std::endl;
                    Color color = settings.tracked_serum_outline_color;
                    double outline_width = settings.tracked_serum_outline_width;
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

} // AntigenicMapsLayout::mark_tracked_sera

// ----------------------------------------------------------------------

void AntigenicMapsLayout::draw_map_title(Surface& aSurface, size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    const HzSection& section = mAntigenicMapsDraw.hz_sections().sections[aSectionIndex];
    std::string title = section.index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    if (!section.label.empty())
        title += " " + section.label;
    const Size wsize = aSurface.text_size("W", Pixels{settings.map_title_size});
    const Size tsize = aSurface.text_size(title, Pixels{settings.map_title_size});
    aSurface.text({settings.map_title_offset.width * wsize.width + aSurface.viewport().origin.x,
                    settings.map_title_offset.height * wsize.height + tsize.height + aSurface.viewport().origin.y},
        title, settings.map_title_color, Pixels{settings.map_title_size});

} // AntigenicMapsLayout::draw_map_title

// ----------------------------------------------------------------------

void AntigenicMapsLayout::mark_tracked_antigens(size_t aSectionIndex)
{
    for (size_t point_no = 0; point_no < mAntigenicMapsDraw.chart().points().size(); ++point_no) {
        if (mDrawPoints[point_no] == &mDrawTrackedAntigen || mDrawPoints[point_no] == &mDrawSequencedAntigen) {
            const auto sequenced = mSequencedAntigens.find(point_no);
            if (sequenced != mSequencedAntigens.end()) {
                if (sequenced->second == aSectionIndex)
                    mDrawPoints[point_no] = &mDrawTrackedAntigen;
                else
                    mDrawPoints[point_no] = &mDrawSequencedAntigen;
            }
        }
    }

} // AntigenicMapsLayout::mark_tracked_antigens

// ----------------------------------------------------------------------

void LabelledGrid::prepare()
{
    AntigenicMapsLayout::prepare();

} // LabelledGrid::prepare

// ----------------------------------------------------------------------

void LabelledGrid::draw()
{
    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    Surface& surface = mAntigenicMapsDraw.surface();
    // std::cerr << "Maps " << surface << std::endl;

    const double map_width = (surface.viewport().size.width - (settings.columns - 1) * settings.gap) / settings.columns;

    size_t row = 0, column = 0, section_index = 0;
    for (const auto& section: mAntigenicMapsDraw.hz_sections().sections) {
        if (section.show && section.show_map) {
            Surface& map_surface = surface.subsurface({column * (map_width + settings.gap), row * (map_width + settings.gap)},
                                                      Scaled{map_width}, mMapViewport, true);
            // std::cerr << "Map " << map_surface << std::endl;
            // std::cerr << "origin_offset: " << map_surface.origin_offset() << "  scale: " << map_surface.scale() << std::endl;
            draw_chart(map_surface, section_index);
            draw_mapped_antigens_section(section_index);
            ++column;
            if (column >= settings.columns) {
                ++row;
                column = 0;
            }
        }
        ++section_index;
    }

    const double antigenic_maps_width = mAntigenicMapsDraw.signature_page_settings().antigenic_maps_width;
    const size_t rows = row + (column ? 1 : 0);
    const double maps_height = map_width * rows + (rows - 1) * settings.gap;
    const double suggested_surface_width = antigenic_maps_width * surface.viewport().size.height / maps_height;
    std::cout << "Map area height: " << maps_height << std::endl;
    if (std::abs((antigenic_maps_width - suggested_surface_width) / antigenic_maps_width) > 0.01)
        std::cout << "Change antigenic_maps.width from " << antigenic_maps_width << " to " << suggested_surface_width << std::endl;
    else
        std::cout << "antigenic_maps.width is OK: " << antigenic_maps_width << " vs. suggested " << suggested_surface_width << std::endl;

} // LabelledGrid::draw

// ----------------------------------------------------------------------

void LabelledGrid::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    AntigenicMapsLayout::draw_chart(aSurface, aSectionIndex);

} // LabelledGrid::draw_chart

// ----------------------------------------------------------------------

void LabelledGrid::draw_mapped_antigens_section(size_t aSectionIndex)
{
    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    Surface& surface = mAntigenicMapsDraw.mapped_antigens_draw().surface();
    const auto& section = mAntigenicMapsDraw.hz_sections().sections[aSectionIndex];

    const double left = 0, right = surface.viewport().size.width;
    const double top = section.first->draw.vertical_pos, bottom = section.last->draw.vertical_pos;
    surface.line({left, top}, {right, top}, settings.mapped_antigens_section_line_color, Pixels{settings.mapped_antigens_section_line_width}, Surface::LineCap::Square);
    surface.line({left, bottom}, {right, bottom}, settings.mapped_antigens_section_line_color, Pixels{settings.mapped_antigens_section_line_width}, Surface::LineCap::Square);
    surface.line({right, top}, {right, bottom}, settings.mapped_antigens_section_line_color, Pixels{settings.mapped_antigens_section_line_width}, Surface::LineCap::Square);

} // LabelledGrid::draw_mapped_antigens_section

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
