#pragma once

#include "surface.hh"
#include "cairo.hh"

// ----------------------------------------------------------------------

class SurfaceCairo : public Surface
{
 public:
    virtual ~SurfaceCairo();

    virtual inline Size size() const { return mSize; }
    virtual inline double width() const { return mSize.width; }
    virtual inline double height() const { return mSize.height; }

    virtual inline Surface* subsurface(const Size& aOffset, const Size& aOuterSize, double aInnerWidth, bool aClip)
        {
            const double scale = aOuterSize.width / aInnerWidth;
            return new SurfaceCairo(this, mContext, aOffset, aOuterSize / scale, scale, aClip);
        }

    virtual void line(const Location& a, const Location& b, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt);
    virtual void rectangle(const Location& a, const Size& s, Color aColor, double aWidth, LineCap aLineCap = LineCap::Butt);
    virtual void rectangle_filled(const Location& a, const Size& s, Color aOutlineColor, double aWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);

    virtual void circle(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth);
    virtual void circle_filled(const Location& aCenter, double aDiameter, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor);
    virtual void square_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);
    virtual void triangle_filled(const Location& aCenter, double aSide, double aAspect, double aAngle, Color aOutlineColor, double aOutlineWidth, Color aFillColor, LineCap aLineCap = LineCap::Butt);

    virtual void path_outline(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt);
    virtual void path_outline(const double* first, const double* last, Color aOutlineColor, double aOutlineWidth, bool aClose = false, LineCap aLineCap = LineCap::Butt);
    virtual void path_fill(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last, Color aFillColor);
    virtual void path_fill(const double* first, const double* last, Color aFillColor);

    virtual void double_arrow(const Location& a, const Location& b, Color aColor, double aLineWidth, double aArrowWidth);
    virtual void grid(double aStep, Color aLineColor, double aLineWidth);
    virtual void border(Color aLineColor, double aLineWidth);
    virtual void background(Color aColor);

    virtual void text(const Location& a, std::string aText, Color aColor, double aSize, const TextStyle& aTextStyle = TextStyle(), double aRotation = 0);
    virtual Size text_size(std::string aText, double aSize, const TextStyle& aTextStyle = TextStyle(), double* x_bearing = nullptr);

 protected:
    SurfaceCairo* mParent;
    cairo_t* mContext;
    Size mOffset;
    Size mSize;
    double mScale;
    bool mClip;                 // force surface area clipping

    inline SurfaceCairo() : mParent(nullptr), mContext(nullptr), mScale(1.0) {}
    inline SurfaceCairo(SurfaceCairo* aParent, cairo_t* aContext, const Size& aOffset, const Size& aSize, double aScale, bool aClip)
        : mParent(aParent), mContext(aContext), mOffset(aOffset), mSize(aSize), mScale(aScale), mClip(aClip)
        {
            cairo_reference(mContext);
        }

    virtual Location arrow_head(const Location& a, double angle, double sign, Color aColor, double aArrowWidth);

    class context
    {
     public:
        context(SurfaceCairo& aSurface);
        ~context();

        inline context& set_line_width(double aWidth) { cairo_set_line_width(mContext, aWidth); return *this; }
        inline context& set_source_rgba(Color aColor) { cairo_set_source_rgba(mContext, aColor.red(), aColor.green(), aColor.blue(), aColor.alpha()); return *this; }
        inline context& set_line_cap(LineCap aLineCap) { cairo_set_line_cap(mContext, cairo_line_cap(aLineCap)); return *this; }
        inline context& set_line_join(LineJoin aLineJoin) { cairo_set_line_join(mContext, cairo_line_join(aLineJoin)); return *this; }
        inline context& move_to() { cairo_move_to(mContext, 0.0, 0.0); return *this; }
        inline context& move_to(const Location& a) { cairo_move_to(mContext, a.x, a.y); return *this; }
        inline context& line_to(const Location& a) { cairo_line_to(mContext, a.x, a.y); return *this; }
        inline context& lines_to(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last) { for ( ; first != last; ++first) { line_to(*first); } return *this; }
        inline context& rectangle(const Location& a, const Size& s) { cairo_rectangle(mContext, a.x, a.y, s.width, s.height); return *this; }
        inline context& arc(const Location& a, double radius, double angle1, double angle2) { cairo_arc(mContext, a.x, a.y, radius, angle1, angle2); return *this; }
        inline context& circle(double radius) { cairo_arc(mContext, 0.0, 0.0, radius, 0.0, 2.0 * M_PI); return *this; }
        inline context& circle(const Location& a, double radius) { cairo_arc(mContext, a.x, a.y, radius, 0.0, 2.0 * M_PI); return *this; }
        inline context& stroke() { cairo_stroke(mContext); return *this; }
        inline context& fill() { cairo_fill(mContext); return *this; }
        inline context& fill_preserve() { cairo_fill_preserve(mContext); return *this; }
        inline context& translate(const Size& a) { cairo_translate(mContext, a.width, a.height); return *this; }
        inline context& translate(const Location& a) { cairo_translate(mContext, a.x, a.y); return *this; }
        inline context& rotate(double aAngle) { cairo_rotate(mContext, aAngle); return *this; }
        inline context& scale(double x, double y) { cairo_scale(mContext, x, y); return *this; }
        inline context& scale(double x) { cairo_scale(mContext, x, x); return *this; }
        inline context& clip() { cairo_clip(mContext); return *this; }
        inline context& new_path() { cairo_new_path(mContext); return *this; }
        inline context& close_path() { cairo_close_path(mContext); return *this; }
        inline context& close_path_if(bool aClose) { if (aClose) cairo_close_path(mContext); return *this; }
        inline context& prepare_for_text(double aSize, const TextStyle& aTextStyle) { cairo_select_font_face(mContext, aTextStyle.font_family().c_str(), cairo_font_slant(aTextStyle.slant()), cairo_font_weight(aTextStyle.weight())); cairo_set_font_size(mContext, aSize); return *this; }
        inline context& show_text(std::string aText) { cairo_show_text(mContext, aText.c_str()); return *this; }
        inline context& text_extents(std::string aText, cairo_text_extents_t& extents) { cairo_text_extents(mContext, aText.c_str(), &extents); return *this; }

          // if Location::x is negative - move_to, else - path_to
        inline context& move_to_line_to(std::vector<Location>::const_iterator first, std::vector<Location>::const_iterator last)
            {
                for ( ; first != last; ++first) {
                    if (first->x < 0)
                        move_to({std::abs(first->x), std::abs(first->y)});
                    else
                        line_to(*first);
                }
                return *this;
            }

          // the same as above but with raw data
        inline context& move_to_line_to(const double* first, const double* last)
            {
                for ( ; first != last; first += 2) {
                    if (*first < 0)
                        move_to({std::abs(*first), std::abs(*(first+1))});
                    else
                        line_to({*first, *(first+1)});
                }
                return *this;
            }

        inline context& close_move_to_line_to(const double* first, const double* last)
            {
                for ( ; first != last; first += 2) {
                    if (*first < 0) {
                        close_path();
                        move_to({std::abs(*first), std::abs(*(first+1))});
                    }
                    else
                        line_to({*first, *(first+1)});
                }
                return *this;
            }

     private:
        context* mParent;
        cairo_t* mContext;

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
                return CAIRO_LINE_CAP_BUTT; // gcc wants return
            }

        inline cairo_line_join_t cairo_line_join(LineJoin aLineJoin) const
            {
                switch (aLineJoin) {
                  case LineJoin::Miter:
                      return CAIRO_LINE_JOIN_MITER;
                  case LineJoin::Round:
                      return CAIRO_LINE_JOIN_ROUND;
                  case LineJoin::Bevel:
                      return CAIRO_LINE_JOIN_ROUND;
                }
                return CAIRO_LINE_JOIN_MITER; // gcc wants return
            }

        inline cairo_font_slant_t  cairo_font_slant(TextStyle::Slant aSlant) const
            {
                switch (aSlant) {
                  case TextStyle::Slant::Normal:
                      return CAIRO_FONT_SLANT_NORMAL;
                  case TextStyle::Slant::Italic:
                      return CAIRO_FONT_SLANT_ITALIC;
                  // case TextStyle::Slant::Oblique:
                  //     return CAIRO_FONT_SLANT_OBLIQUE;
                }
                return CAIRO_FONT_SLANT_NORMAL; // gcc wants return
            }

        inline cairo_font_weight_t  cairo_font_weight(TextStyle::Weight aWeight) const
            {
                switch (aWeight) {
                  case TextStyle::Weight::Normal:
                      return CAIRO_FONT_WEIGHT_NORMAL;
                  case TextStyle::Weight::Bold:
                      return CAIRO_FONT_WEIGHT_BOLD;
                }
                  return CAIRO_FONT_WEIGHT_NORMAL; // gcc wants return
            }
    };

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
