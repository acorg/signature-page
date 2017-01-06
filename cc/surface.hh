#pragma once

#include <vector>

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

    inline std::string slant_as_stirng() const
        {
            switch (mSlant) {
              case Slant::Normal:
                  return "normal";
              case Slant::Italic:
                  return "italic";
            }
            return "normal";
        }

    inline std::string weight_as_stirng() const
        {
            switch (mWeight) {
              case Weight::Normal:
                  return "normal";
              case Weight::Bold:
                  return "bold";
            }
            return "normal";
        }

    inline std::string& font_family() { return mFontFamily; }
    inline void font_family(std::string s) { mFontFamily = s; }
    inline void font_family(const char* s, size_t len) { mFontFamily.assign(s, len); }

    inline void slant(Slant aSlant) { mSlant = aSlant; }
    inline void slant(std::string aSlant)
        {
            if (aSlant == "normal")
                mSlant = Slant::Normal;
            else if (aSlant == "italic")
                mSlant = Slant::Italic;
            else
                throw std::runtime_error("Unrecognized TextStyle slant: " + aSlant);
        }
    inline void slant(const char* s, size_t len) { slant(std::string(s, len)); }

    inline void weight(Weight aWeight) { mWeight = aWeight; }
    inline void weight(std::string aWeight)
        {
            if (aWeight == "normal")
                mWeight = Weight::Normal;
            else if (aWeight == "bold")
                mWeight = Weight::Bold;
            else
                throw std::runtime_error("Unrecognized TextStyle weight: " + aWeight);
        }
    inline void weight(const char* s, size_t len) { weight(std::string(s, len)); }

 private:
    std::string mFontFamily;
    Slant mSlant;
    Weight mWeight;

}; // class TextStyle

// ----------------------------------------------------------------------

using Path = std::vector<Location>;

// ----------------------------------------------------------------------

class Surface
{
 public:
    enum class LineCap { Butt, Round, Square };
    enum class LineJoin { Miter, Round, Bevel };

    [[noreturn]] inline Surface(const Surface&) { throw std::runtime_error("Surface copying forbidden!"); } // cannto make it private due to using vector<SurfaceCairoChild>
    virtual ~Surface() {}

    constexpr static const double default_canvas_width = 1000.0;

    virtual const Size& size() const = 0;
    virtual const Size& offset() const = 0;
    virtual const Size& viewport_offset() const = 0;
    inline double aspect() const { return size().aspect(); }
      // virtual void move_resize(const Size& aNewOffset, const Size& aNewSize) = 0;
    virtual void move_resize(const Size& aNewOffset, const Size& aOuterSize, double aInnerWidth) = 0;
    virtual void viewport_offset(const Size& aViewportOffset) = 0;

    virtual Surface& subsurface(const Size& aOffset, const Size& aOuterSize, double aInnerWidth, bool aClip) = 0;
    virtual Surface& subsurface(bool aClip) = 0;

    virtual void line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;

    virtual void circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth) = 0;
    virtual void circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor) = 0;
    virtual void square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt) = 0;

    virtual void path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void path_outline(const double* first, const double* last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt) = 0;
    virtual void path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor) = 0;
    virtual void path_fill(const double* first, const double* last, Color aFillColor) = 0;

    virtual void double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth) = 0;
    virtual void grid(double aStep, Color aLineColor, double aLineWidth) = 0;
    virtual void border(Color aLineColor, double aLineWidth) = 0;
    virtual void background(Color aColor) = 0;

    virtual void text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle = TextStyle(), double aRotation = 0) = 0;
    virtual Size text_size(std::string aText, double aSize, const TextStyle& aTextStyle = TextStyle(), double* x_bearing = nullptr) = 0;

 protected:
    inline Surface() = default;

    virtual Location arrow_head(const Location& a, double angle, double sign, Color aColor, double aArrowWidth) = 0;

}; // class Surface

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
