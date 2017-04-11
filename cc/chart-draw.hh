#pragma once

#include "acmacs-base/transformation.hh"
#include "acmacs-draw/viewport.hh"

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings;
class Surface;

// ----------------------------------------------------------------------

class ChartDrawBase
{
 public:
    virtual inline ~ChartDrawBase() {}

    virtual void init_settings() = 0;
    virtual void prepare(const AntigenicMapsDrawSettings& aSettings) = 0;
    virtual void draw(Surface& aSurface) const = 0;

    virtual const Viewport& viewport(const Transformation* aSettingsTransformation) = 0;
    inline const Viewport& viewport(const Transformation* aSettingsTransformation) const { return const_cast<ChartDrawBase*>(this)->viewport(aSettingsTransformation); }

    virtual std::string lab() const = 0;

    constexpr static const size_t AntigenNotFound = static_cast<size_t>(-1);
      // returns AntigenNotFound if not found
    virtual size_t find_antigen(std::string aName) const = 0;

}; // class ChartDrawBase

// ----------------------------------------------------------------------

class ChartReadError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
