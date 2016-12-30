#pragma once

#include "surface.hh"
#include "cairo.hh"

// ----------------------------------------------------------------------

class SurfaceCairoChild;

class SurfaceCairo : public Surface
{
 public:
    virtual SurfaceCairo* parent() { return nullptr; }
    virtual cairo_t* cairo_context() = 0;

    // virtual inline double width() const { return mSize.width; }
    // virtual inline double height() const { return mSize.height; }

    virtual Surface& subsurface(const Size& aOffset, const Size& aOuterSize, double aInnerWidth, bool aClip);
    virtual Surface& subsurface(bool aClip);

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
    std::vector<SurfaceCairoChild> mChildren;

    inline SurfaceCairo() { mChildren.reserve(10); } // reserve to avoid re-allocating and copying (copying is forbidden)

    virtual Location arrow_head(const Location& a, double angle, double sign, Color aColor, double aArrowWidth);

 private:
    friend class context;
    virtual double scale() const = 0;
    virtual bool clip() const { return false; }

}; // class SurfaceCairo

// ----------------------------------------------------------------------

class SurfaceCairoChild : public SurfaceCairo
{
 public:
    inline SurfaceCairoChild(SurfaceCairo& aParent, bool aClip)
        : mParent(aParent), mScale(1.0), mClip(aClip) {}
    inline SurfaceCairoChild(SurfaceCairo& aParent, const Size& aOffset, const Size& aSize, double aScale, bool aClip)
        : mParent(aParent), mOffset(aOffset), mSize(aSize), mScale(aScale), mClip(aClip) {}

    virtual inline SurfaceCairo* parent() { return &mParent; }
    virtual inline cairo_t* cairo_context() { return mParent.cairo_context(); }
    virtual inline const Size& size() const { return mSize; }
    virtual inline const Size& offset() const { return mOffset; }

      // virtual inline void move_resize(const Size& aNewOffset, const Size& aNewSize) { mOffset = aNewOffset; mScale *= mSize.width / aNewSize.width; mSize = aNewSize; }
    virtual inline void move_resize(const Size& aNewOffset, const Size& aOuterSize, double aInnerWidth) { mOffset = aNewOffset; mScale = aOuterSize.width / aInnerWidth; mSize = aOuterSize / mScale; }

 private:
    SurfaceCairo& mParent;
    Size mOffset;
    Size mSize;
    double mScale;
    bool mClip;                 // force surface area clipping

      // inline SurfaceCairoChild() : SurfaceCairo(), mParent(nullptr) {}
    virtual inline double scale() const { return mScale; }
    virtual inline bool clip() const { return mClip; }

}; // class SurfaceCairoChild

// ----------------------------------------------------------------------

class PdfCairo : public SurfaceCairo
{
 public:
    PdfCairo(std::string aFilename, double aWidth, double aHeight);
    virtual ~PdfCairo();

    virtual inline cairo_t* cairo_context() { return mCairoContext; }
    virtual inline const Size& size() const { return mSize; }
    virtual inline const Size& offset() const { return mOffset; }

      // virtual inline void move_resize(const Size& aNewOffset, const Size& aNewSize) { mOffset = aNewOffset; mScale *= mSize.width / aNewSize.width; mSize = aNewSize; }
    virtual inline void move_resize(const Size& aNewOffset, const Size& aOuterSize, double aInnerWidth) { mOffset = aNewOffset; mScale = aOuterSize.width / aInnerWidth; mSize = aOuterSize / mScale; }

 private:
    Size mSize;
    Size mOffset;
    double mScale;
    cairo_t* mCairoContext;

    virtual inline double scale() const { return mScale; }

}; // class PdfCairo

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
