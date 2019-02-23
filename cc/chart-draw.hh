#pragma once

#include "acmacs-base/transformation.hh"
#include "acmacs-chart-2/chart.hh"
#include "acmacs-draw/viewport.hh"

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings;
namespace acmacs::surface { class Surface; }

// ----------------------------------------------------------------------

class ChartDrawBase
{
 public:
    virtual ~ChartDrawBase() = default;

    virtual void init_settings() = 0;
    virtual const acmacs::Viewport& viewport() const = 0;
    virtual const acmacs::Viewport& calculate_viewport() = 0;
    virtual void draw(acmacs::surface::Surface& aSurface) const = 0;

    std::string lab() const { return chart().info()->lab(acmacs::chart::Info::Compute::Yes); }
    std::string virus_type() const { return chart().info()->virus_type(acmacs::chart::Info::Compute::Yes); }
    std::string assay() const { return chart().info()->assay(acmacs::chart::Info::Compute::Yes); }

    virtual std::optional<size_t> find_antigen(std::string aName) const = 0;

    virtual const acmacs::chart::Chart& chart() const = 0;

}; // class ChartDrawBase

// ----------------------------------------------------------------------

class ChartReadError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
