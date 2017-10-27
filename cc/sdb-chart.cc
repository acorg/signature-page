#error Not supported since 2017-10-27

#include "sdb-chart.hh"
#include "antigenic-maps-draw.hh"
#include "sdb-antigenic-maps-draw.hh"

using namespace sdb;

// ----------------------------------------------------------------------

void Chart::init_settings()
{
    calculate_viewport();

} // Chart::init_settings

// ----------------------------------------------------------------------

void Chart::calculate_viewport()
{
    if (mViewport.empty()) {
        apply_transformation();
        bounding_rectangle(mViewport);
        mViewport.square();
        mViewport.whole_width();
        std::cout << "INFO: [sdb] Calculated viewport: " << mViewport << std::endl;
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

// void Chart::prepare(const DrawPointSettings& aSettings)
// {
//     std::cerr << "INFO: [sdb] sdb::Chart prepare" << std::endl;
//       // calculate_viewport(&aSettings.transformation);

// } // Chart::prepare

// ----------------------------------------------------------------------

void Chart::apply_transformation()
{
      // std::cout << "INFO: [sdb] Chart::apply_transformation transformation: " << mTransformation << std::endl;
    for (auto& p: mPoints) {
        if (!p.coordinates.isnan()) {
            const auto x = p.coordinates.x * mTransformation.a + p.coordinates.y * mTransformation.c;
            p.coordinates.y = p.coordinates.x * mTransformation.b + p.coordinates.y * mTransformation.d;
            p.coordinates.x = x;
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
