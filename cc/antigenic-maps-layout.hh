#pragma once

#include <map>

#include "acmacs-draw/viewport.hh"
#include "antigenic-maps-draw.hh"

class SettingsInitializer;

// ----------------------------------------------------------------------

class AntigenicMapsLayoutDraw
{
 public:
    AntigenicMapsLayoutDraw(AntigenicMapsDrawBase& aAntigenicMapsDraw) : mAntigenicMapsDraw(aAntigenicMapsDraw) {}
    virtual ~AntigenicMapsLayoutDraw();

    virtual void init_settings(const SettingsInitializer& settings_initilizer);
    virtual void prepare();
    virtual void prepare_apply_mods() = 0;
    virtual void prepare_chart_for_all_sections() = 0;
    virtual void prepare_drawing_chart(size_t aSectionIndex, std::string map_letter, bool report_antigens_in_hz_sections) = 0;
    virtual void draw_chart(acmacs::surface::Surface& aSurface, size_t aSectionIndex) = 0;

    AntigenicMapsDrawBase& antigenic_maps_draw() { return mAntigenicMapsDraw; }
    const AntigenicMapsDrawBase& antigenic_maps_draw() const { return mAntigenicMapsDraw; }
    AntigenicMapsDrawSettings& settings() { return mAntigenicMapsDraw.settings(); }
    const AntigenicMapsDrawSettings& settings() const { return mAntigenicMapsDraw.settings(); }
    auto& chart() { return mAntigenicMapsDraw.chart(); }
    const auto& chart() const { return mAntigenicMapsDraw.chart(); }
    acmacs::surface::Surface& surface() const { return mAntigenicMapsDraw.surface(); }
    const auto& hz_sections() const { return mAntigenicMapsDraw.hz_sections(); }
    const auto& signature_page_settings() const { return mAntigenicMapsDraw.signature_page_settings(); }
    auto& signature_page_settings() { return mAntigenicMapsDraw.signature_page_settings(); }
    const acmacs::Viewport& viewport() const;
    const auto& sequenced_antigens() const { return mSequencedAntigens; }

 protected:
    virtual void apply_mods_before(acmacs::surface::Surface& aSurface);
    virtual void apply_mods_after(acmacs::surface::Surface& aSurface);

    virtual void find_sequenced_antigens();

 private:
    AntigenicMapsDrawBase& mAntigenicMapsDraw;
    std::map<size_t, size_t> mSequencedAntigens; // antigen_no to section_no

}; // class AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

class AntigenicMapsLayout
{
 public:
    virtual ~AntigenicMapsLayout();

    virtual AntigenicMapsLayoutDraw& layout_draw() = 0;
    virtual void prepare() { layout_draw().prepare(); }
    virtual void init_settings(const SettingsInitializer& settings_initilizer) { layout_draw().init_settings(settings_initilizer); }
    virtual void draw(acmacs::surface::Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections) = 0;

}; // class AntigenicMapsLayout

// ----------------------------------------------------------------------

class LabelledGridBase : public AntigenicMapsLayout
{
 public:
    void draw(acmacs::surface::Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections) override;

 private:
    void draw_mapped_antigens_section(size_t aSectionIndex, acmacs::surface::Surface& aMappedAntigensDrawSurface);

}; // class LabelledGridBase

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
