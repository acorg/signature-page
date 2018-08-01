#pragma once

#include "acmacs-base/transformation.hh"
#include "acmacs-draw/viewport.hh"

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings;
namespace acmacs::surface { class Surface; }
namespace acmacs::chart { class Chart; }

// ----------------------------------------------------------------------

class ChartDrawBase
{
 public:
    virtual inline ~ChartDrawBase() {}

    virtual void init_settings() = 0;
    virtual const acmacs::Viewport& viewport() const = 0;
    virtual void draw(acmacs::surface::Surface& aSurface) const = 0;

    virtual std::string lab() const = 0;

    virtual std::optional<size_t> find_antigen(std::string aName) const = 0;

    virtual const acmacs::chart::Chart& chart() const = 0;

}; // class ChartDrawBase

// ----------------------------------------------------------------------

class ChartReadError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
