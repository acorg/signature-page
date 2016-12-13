#include "surface-cairo.hh"

// ----------------------------------------------------------------------

SurfaceCairo::~SurfaceCairo()
{
    if (context)
        cairo_destroy(context);

} // SurfaceCairo::~SurfaceCairo

// ----------------------------------------------------------------------

void SurfaceCairo::line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap)
{
    push_context pc(*this);
    cairo_set_line_width(context, aWidth);
    set_source_rgba(aColor);
    cairo_set_line_cap(context, cairo_line_cap(aLineCap));
    cairo_move_to(context, a.x, a.y);
    cairo_line_to(context, b.x, b.y);
    cairo_stroke(context);

} // SurfaceCairo::line

// ----------------------------------------------------------------------

void SurfaceCairo::rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap)
{
    push_context pc(*this);
    cairo_set_line_width(context, aWidth);
    cairo_set_line_cap(context, cairo_line_cap(aLineCap));
    cairo_rectangle(context, a.x, a.y, s.width, s.height);
    set_source_rgba(aColor);
    cairo_stroke(context);

} // SurfaceCairo::rectangle

// ----------------------------------------------------------------------

void SurfaceCairo::rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap)
{
    push_context pc(*this);
    cairo_set_line_width(context, aWidth);
    cairo_set_line_cap(context, cairo_line_cap(aLineCap));
    cairo_rectangle(context, a.x, a.y, s.width, s.height);
    set_source_rgba(aFillColor);
    cairo_fill_preserve(context);
    set_source_rgba(aOutlineColor);
    cairo_stroke(context);

} // SurfaceCairo::rectangle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth)
{
    push_context pc(*this);
    cairo_set_line_width(context, aOutlineWidth);
    cairo_translate(context, aCenter.x, aCenter.y);
    cairo_rotate(context, aAngle);
    cairo_scale(context, aAspect, 1.0);
    cairo_arc(context, 0, 0, aDiameter / 2, 0.0, 2.0 * M_PI);
    set_source_rgba(aOutlineColor);
    cairo_stroke(context);

} // SurfaceCairo::circle

// ----------------------------------------------------------------------

void SurfaceCairo::circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor)
{
    push_context pc(*this);
    cairo_set_line_width(context, aOutlineWidth);
    cairo_translate(context, aCenter.x, aCenter.y);
    cairo_rotate(context, aAngle);
    cairo_scale(context, aAspect, 1.0);
    cairo_arc(context, 0, 0, aDiameter / 2, 0.0, 2.0 * M_PI);
    set_source_rgba(aFillColor);
    cairo_fill_preserve(context);
    set_source_rgba(aOutlineColor);
    cairo_stroke(context);

} // SurfaceCairo::circle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap)
{
    push_context pc(*this);

} // SurfaceCairo::square_filled

// ----------------------------------------------------------------------

void SurfaceCairo::triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap)
{
    push_context pc(*this);

} // SurfaceCairo::triangle_filled

// ----------------------------------------------------------------------

void SurfaceCairo::path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose, LineCap aLineCap)
{
    push_context pc(*this);

} // SurfaceCairo::path_outline

// ----------------------------------------------------------------------

void SurfaceCairo::path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor)
{
    push_context pc(*this);

} // SurfaceCairo::path_fill

// ----------------------------------------------------------------------

void SurfaceCairo::double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth)
{
    push_context pc(*this);

} // SurfaceCairo::double_arrow

// ----------------------------------------------------------------------

void SurfaceCairo::grid(double aStep, Color aLineColor, double aLineWidth)
{
    push_context pc(*this);

} // SurfaceCairo::grid

// ----------------------------------------------------------------------

void SurfaceCairo::text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle, double aRotation)
{
    push_context pc(*this);

} // SurfaceCairo::text

// ----------------------------------------------------------------------

Size SurfaceCairo::text_size(std::string aText, double aSize, const TextStyle& aTextStyle, double* x_bearing)
{

} // SurfaceCairo::text_size

// ----------------------------------------------------------------------

PdfCairo::PdfCairo(std::string aFilename, double aWidth, double aHeight)
{
    auto surface = cairo_pdf_surface_create(aFilename.c_str(), aWidth, aHeight);
    context = cairo_create(surface);
    cairo_surface_destroy(surface);

} // PdfCairo::PdfCairo

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
