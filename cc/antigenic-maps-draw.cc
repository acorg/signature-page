#include "antigenic-maps-draw.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void AntigenicMapsDraw::init_settings()
{
    const size_t number_sections = mHzSections.sections.size();
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

void DrawSerum::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    const double size = aSettings.serum_scale * aObjectScale;
    if (!aPoint.coordinates.isnan()) {
        aSurface.rectangle_filled(aPoint.coordinates, {size * aspect(aPoint, aStyle, aSettings), size}, outline_color(aPoint, aStyle, aSettings),
                                  outline_width(aPoint, aStyle, aSettings) * aObjectScale, "transparent");
    }

} // DrawSerum::draw

// ----------------------------------------------------------------------

void DrawReferenceAntigen::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, aSettings.reference_antigen_scale * aObjectScale, aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.reference_antigen_outline_color,
                               aSettings.reference_antigen_outline_width * aObjectScale, "transparent");
    }

} // DrawReferenceAntigen::draw

// ----------------------------------------------------------------------

void DrawTestAntigen::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, aSettings.test_antigen_scale * aObjectScale, aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.test_antigen_outline_color,
                               aSettings.test_antigen_outline_width * aObjectScale, aSettings.test_antigen_fill_color);
    }

} // DrawTestAntigen::draw

// ----------------------------------------------------------------------

void DrawSequencedAntigen::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, aSettings.test_antigen_scale * aObjectScale, aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.sequenced_antigen_outline_color,
                               aSettings.sequenced_antigen_outline_width * aObjectScale, aSettings.sequenced_antigen_fill_color);
    }

} // DrawSequencedAntigen::draw

// ----------------------------------------------------------------------

void DrawTrackedAntigen::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        aSurface.circle_filled(aPoint.coordinates, aSettings.tracked_antigen_scale * aObjectScale, aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.tracked_antigen_outline_color,
                               aSettings.tracked_antigen_outline_width * aObjectScale, mColor);
    }

} // DrawTrackedAntigen::draw

// ----------------------------------------------------------------------

void DrawTrackedSerum::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    DrawSerum::draw(aSurface, aPoint, aStyle, aObjectScale, aSettings);
    std::cout << "    Tracked serum " << aPoint.name << " radius:" << aPoint.attributes.serum_circle_radius << std::endl;
    aSurface.circle(aPoint.coordinates, aPoint.attributes.serum_circle_radius * 2, 1, 0,
                    outline_color(aPoint, aStyle, aSettings), // aSettings.serum_circle_color,
                    aSettings.serum_circle_thickness * aObjectScale);

} // DrawTrackedSerum::draw

// ----------------------------------------------------------------------

void DrawVaccineAntigen::draw(Surface& aSurface, const Point& aPoint, const PointStyle& aStyle, double aObjectScale, const AntigenicMapsDrawSettings& aSettings) const
{
    if (!aPoint.coordinates.isnan()) {
        std::cout << "Vaccine " << aPoint.name << " " << fill_color(aPoint, aStyle, aSettings) << std::endl;
        aSurface.circle_filled(aPoint.coordinates, aSettings.vaccine_antigen_scale * aObjectScale, aspect(aPoint, aStyle, aSettings), rotation(aPoint, aStyle, aSettings),
                               aSettings.vaccine_antigen_outline_color, // aPoint.attributes.vaccine.outline_color,
                               aSettings.vaccine_antigen_outline_width * aObjectScale,
                               fill_color(aPoint, aStyle, aSettings) /*aPoint.attributes.vaccine.fill_color*/);
    }

} // DrawVaccineAntigen::draw

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

    draw_points_reset();

    // mPrefixName.clear();
    // for (const auto& p: mPoints) {
    //     mPrefixName.insert(p.name.substr(0, p.name.find(1, ' ')));
    // }


} // AntigenicMapsLayout::prepare

// ----------------------------------------------------------------------

void AntigenicMapsLayout::draw_points_reset()
{
    mDrawPoints.resize(mAntigenicMapsDraw.chart().points().size(), nullptr);
    for (size_t point_no = 0; point_no < mAntigenicMapsDraw.chart().points().size(); ++point_no) {
        const auto& p = mAntigenicMapsDraw.chart().points()[point_no];
        if (p.attributes.antigen) {
            if (p.attributes.vaccine) {
                mDrawPoints[point_no] = &mDrawVaccineAntigen;
            }
            // else if (mSequencedAntigens.find(point_no) != mSequencedAntigens.end()) {
            //     mDrawPoints[point_no] = &mDrawSequencedAntigen;
            // }
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

} // AntigenicMapsLayout::draw_points_reset

// ----------------------------------------------------------------------

void AntigenicMapsLayout::draw_chart(Surface& aSurface)
{
    const AntigenicMapsDrawSettings& settings = mAntigenicMapsDraw.settings();
    const Chart& chart = mAntigenicMapsDraw.chart();

    aSurface.background(settings.background_color);
    aSurface.grid(1, settings.grid_line_color, settings.grid_line_width);
    aSurface.border(settings.border_color, settings.border_width * 2);
    aSurface.viewport_offset(mMapViewport.offset());

    size_t drawn = 0;
    for (size_t level = 0; level < 10 && drawn < mDrawPoints.size(); ++level) {
        for (size_t point_no = 0; point_no < mDrawPoints.size(); ++point_no) {
           if (mDrawPoints[point_no]->level() == level) {
               mDrawPoints[point_no]->draw(aSurface, chart.points()[point_no], chart.plot_style().style(point_no), 0.1, settings);
                ++drawn;
            }
        }
    }
    if (drawn != mDrawPoints.size())
        std::cerr << "Warning: " << drawn << " points of " << mDrawPoints.size() << " were drawn" << std::endl;

} // AntigenicMapsLayout::draw_chart

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

    const double map_width = (surface.size().width - (settings.columns - 1) * settings.gap) / settings.columns;

    size_t row = 0, column = 0;
    for (const auto& section: mAntigenicMapsDraw.hz_sections().sections) {
        Surface& map_surface = surface.subsurface({column * (map_width + settings.gap), row * (map_width + settings.gap)},
                                                   {map_width, map_width}, mMapViewport.size.width, true);
        draw_chart(map_surface);
        ++column;
        if (column >= settings.columns) {
            ++row;
            column = 0;
        }
    }

} // LabelledGrid::draw

// ----------------------------------------------------------------------

void LabelledGrid::draw_chart(Surface& aSurface)
{
    AntigenicMapsLayout::draw_chart(aSurface);

} // LabelledGrid::draw_chart

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
