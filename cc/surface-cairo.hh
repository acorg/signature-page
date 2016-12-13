#pragma once

#include "surface.hh"

// ----------------------------------------------------------------------

class SurfaceCairo : public Surface
{
 public:
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

}; // class SurfaceCairo

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
