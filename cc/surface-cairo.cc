#include "surface-cairo.hh"

// ----------------------------------------------------------------------

void SurfaceCairo::line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap)
{

} // SurfaceCairo::line

// ----------------------------------------------------------------------

void SurfaceCairo::rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap)
{

} // SurfaceCairo::rectangle

// ----------------------------------------------------------------------

void SurfaceCairo::rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap)
{

} // SurfaceCairo::rectangle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth)
{

} // SurfaceCairo::circle

// ----------------------------------------------------------------------

void SurfaceCairo::circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor)
{

} // SurfaceCairo::circle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap)
{

} // SurfaceCairo::square_filled

// ----------------------------------------------------------------------

void SurfaceCairo::triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap)
{

} // SurfaceCairo::triangle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose, LineCap aLineCap)
{

} // SurfaceCairo::path_outline

// ----------------------------------------------------------------------

void SurfaceCairo::path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor)
{

} // SurfaceCairo::path_fill

// ----------------------------------------------------------------------

void SurfaceCairo::double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth)
{

} // SurfaceCairo::double_arrow

// ----------------------------------------------------------------------

void SurfaceCairo::grid(double aStep, Color aLineColor, double aLineWidth)
{

} // SurfaceCairo::grid

// ----------------------------------------------------------------------

void SurfaceCairo::text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle, double aRotation)
{

} // SurfaceCairo::text

// ----------------------------------------------------------------------

Size SurfaceCairo::text_size(std::string aText, double aSize, const TextStyle& aTextStyle, double* x_bearing)
{

} // SurfaceCairo::text_size

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
