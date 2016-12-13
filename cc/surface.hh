#pragma once

#include "acmacs-base/color.hh"
#include "size.hh"

// ----------------------------------------------------------------------

class TextStyle
{
 public:
    enum class Slant { Normal, Italic };
    enum class Weight { Normal, Bold };

    inline TextStyle() : mSlant(Slant::Normal), mWeight(Weight::Normal) {}
    inline TextStyle(std::string aFontFamily, Slant aSlant = Slant::Normal, Weight aWeight = Weight::Normal) : mFontFamily(aFontFamily), mSlant(aSlant), mWeight(aWeight) {}

    inline std::string font_family() const { return mFontFamily; }
    inline Slant slant() const { return mSlant; }
    inline Weight weight() const { return mWeight; }

 private:
    std::string mFontFamily;
    Slant mSlant;
    Weight mWeight;

    // static std::string slant_to_string(Slant a);
    // static std::string weight_to_string(Weight a);
    // static Slant slant_from_string(std::string source);
    // static Weight weight_from_string(weight, std::string source);

}; // class TextStyle

// ----------------------------------------------------------------------

class Surface
{
 public:
    enum class LineCap { Butt, Round, Square };

    inline Surface() = default;
    virtual ~Surface() {}

    virtual void line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;

    virtual void circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth) = 0;
    virtual void circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor) = 0;
    virtual void square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;

    virtual void path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor) = 0;

    virtual void double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth) = 0;
    virtual void grid(double aStep, Color aLineColor, double aLineWidth) = 0;

    virtual void text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle = TextStyle(), double aRotation = 0) = 0;
    virtual Size text_size(std::string aText, double aSize, const TextStyle& aTextStyle, double* x_bearing) = 0;
    inline Size text_size(std::string aText, double aSize, const TextStyle& aTextStyle) { return text_size(aText, aSize, aTextStyle, nullptr); }

}; // class Surface

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
