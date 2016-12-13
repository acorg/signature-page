#pragma once

#include "surface.hh"
#include "cairo.hh"

// ----------------------------------------------------------------------

class SurfaceCairo : public Surface
{
 public:
    inline SurfaceCairo() : context(nullptr) {}
    virtual ~SurfaceCairo();

    virtual void line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt);
    virtual void rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt);
    virtual void rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);

    virtual void circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth);
    virtual void circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor);
    virtual void square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);
    virtual void triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);

    virtual void path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt);
    virtual void path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor);

    virtual void double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth);
    virtual void grid(double aStep, Color aLineColor, double aLineWidth);

    virtual void text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle = TextStyle(), double aRotation = 0);
    virtual Size text_size(std::string aText, double aSize, const TextStyle& aTextStyle, double* x_bearing);

 protected:
    cairo_t* context;

    class push_context
    {
     public:
        inline push_context(SurfaceCairo& aSurface) : context(aSurface.context) { cairo_save(context); }
        inline ~push_context() { cairo_restore(context); }

     private:
        cairo_t* context;
    };

    inline cairo_line_cap_t cairo_line_cap(LineCap aLineCap) const
        {
            switch (aLineCap) {
              case LineCap::Butt:
                  return CAIRO_LINE_CAP_BUTT;
              case LineCap::Round:
                  return CAIRO_LINE_CAP_ROUND;
              case LineCap::Square:
                  return CAIRO_LINE_CAP_SQUARE;
            }
        }

    inline void set_source_rgba(Color aColor) const
        {
            cairo_set_source_rgba(context, aColor.red(), aColor.green(), aColor.blue(), aColor.alpha());
        }

}; // class SurfaceCairo

// ----------------------------------------------------------------------

class PdfCairo : public SurfaceCairo
{
 public:
    PdfCairo(std::string aFilename, double aWidth, double aHeight);

}; // class PdfCairo

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
