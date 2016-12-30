#include "chart.hh"
#include "antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

Chart::~Chart()
{

} // Chart::~Chart

// ----------------------------------------------------------------------

void Chart::prepare(const AntigenicMapsDrawSettings& aSettings)
{
    apply_transformation(aSettings.transformation);
    // mViewport = bounding_rectangle();
    // mViewport.square();
    // const Location offset(aSettings.map_x_offset, aSettings.map_y_offset);
    // mViewport.center(mViewport.center() + offset);
    // mViewport.zoom(aSettings.map_zoom);
    // mViewport.whole_width();

    //   // build point by name index
    // mPointByName.clear();
    // for (size_t point_no = 0; point_no < mPoints.size(); ++point_no) {
    //     mPointByName.emplace(mPoints[point_no].name, point_no);
    //       // mPointByName[mPoints[point_no].name] = point_no;
    // }

    // draw_points_reset(aSettings);

    // mPrefixName.clear();
    // for (const auto& p: mPoints) {
    //     mPrefixName.insert(p.name.substr(0, p.name.find(1, ' ')));
    // }

} // Chart::prepare

// ----------------------------------------------------------------------

void Chart::apply_transformation(const Transformation& aSettingsTransformation)
{
    Transformation t;
    t.multiplyBy(mTransformation);
    t.multiplyBy(aSettingsTransformation);
    std::cout << "transformation: " << t << std::endl;
    for (auto& p: mPoints) {
        if (!p.coordinates.isnan()) {
            p.coordinates.x = p.coordinates.x * t[0] + p.coordinates.y * t[2];
            p.coordinates.y = p.coordinates.x * t[1] + p.coordinates.y * t[3];
        }
    }

} // Chart::apply_transformation

// ----------------------------------------------------------------------

void Chart::make_point_by_name() const
{
    if (mPointByName.empty()) {
        for (size_t point_no = 0; point_no < mPoints.size(); ++point_no) {
            mPointByName.emplace(mPoints[point_no].name, point_no);
        }
    }

} // Chart::make_point_by_name

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
