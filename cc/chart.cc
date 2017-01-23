#include "chart.hh"
#include "antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

Chart::~Chart()
{

} // Chart::~Chart

// ----------------------------------------------------------------------

void Chart::init_settings()
{
    calculate_viewport(nullptr);

} // Chart::init_settings

// ----------------------------------------------------------------------

void Chart::calculate_viewport(const Transformation* aSettingsTransformation)
{
    if (mViewport.empty()) {
        apply_transformation(aSettingsTransformation);
        bounding_rectangle(mViewport);
        mViewport.square();
        mViewport.whole_width();
        std::cout << "Calculated viewport: " << mViewport << std::endl;
    }

} // Chart::calculate_viewport

// ----------------------------------------------------------------------

void Chart::bounding_rectangle(Viewport& aViewport) const
{
    Location tl(1e10, 1e10), br(-1e10, -1e10);
    for (const auto& p: mPoints) {
        if (!p.coordinates.isnan()) {
            tl.min(p.coordinates);
            br.max(p.coordinates);
        }
    }
    aViewport.set(tl, br);

} // Chart::bounding_rectangle

// ----------------------------------------------------------------------

void Chart::prepare(const AntigenicMapsDrawSettings& aSettings)
{
    calculate_viewport(&aSettings.transformation);

} // Chart::prepare

// ----------------------------------------------------------------------

void Chart::apply_transformation(const Transformation* aSettingsTransformation)
{
    Transformation t;
    t.multiply_by(mTransformation);
    if (aSettingsTransformation)
        t.multiply_by(*aSettingsTransformation);
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
