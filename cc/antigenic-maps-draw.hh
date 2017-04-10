#pragma once

#include <iostream>

#include "acmacs-base/transformation.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;
class SignaturePageDrawSettings;
class AntigenicMapsDrawSettings;
class MappedAntigensDraw;
class ChartDrawBase;

// ----------------------------------------------------------------------

class AntigenicMapsDrawBase
{
 public:
    inline AntigenicMapsDrawBase(Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections),
          mSignaturePageDrawSettings(aSignaturePageDrawSettings), mSettings(aSettings) {}
    virtual ~AntigenicMapsDrawBase();

    virtual void init_settings() = 0;
    virtual void prepare() = 0;
    virtual void draw(Surface& aMappedAntigensDrawSurface) = 0;
    virtual const ChartDrawBase& chart() const = 0;

    inline Surface& surface() { return mSurface; }
    inline const Tree& tree() const { return mTree; }
    inline const HzSections& hz_sections() const { return mHzSections; }
    inline SignaturePageDrawSettings& signature_page_settings() { return mSignaturePageDrawSettings; }
    inline const SignaturePageDrawSettings& signature_page_settings() const { return mSignaturePageDrawSettings; }
    inline AntigenicMapsDrawSettings& settings() { return mSettings; }
    inline const AntigenicMapsDrawSettings& settings() const { return mSettings; }

 private:
    Surface& mSurface;
    Tree& mTree;
    HzSections& mHzSections;
    // MappedAntigensDraw& mMappedAntigensDraw;
    SignaturePageDrawSettings& mSignaturePageDrawSettings;
    AntigenicMapsDrawSettings& mSettings;

}; // class AntigenicMapsDrawBase

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings);

// ----------------------------------------------------------------------

class MarkAntigenSettings
{
 public:
    MarkAntigenSettings(bool aShow = true, std::string aName = std::string{});

    bool show;
    std::string name, label;
    double scale, aspect, rotation, outline_width, label_line_width;
    Color fill_color, outline_color, label_color, label_line_color;
    Size label_offset;
    double label_size;

}; // class MarkAntigenSettings

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings
{
 public:
    AntigenicMapsDrawSettings();
    ~AntigenicMapsDrawSettings();

    std::string layout;
    size_t columns;
    double gap;
    Transformation transformation;
    Viewport viewport;

    double border_width, grid_line_width;
    Color border_color, grid_line_color, background_color;

    double point_scale;
    double serum_scale, reference_antigen_scale, test_antigen_scale, vaccine_antigen_scale, tracked_antigen_scale;
    double serum_outline_width, reference_antigen_outline_width, test_antigen_outline_width, vaccine_antigen_outline_width, sequenced_antigen_outline_width, tracked_antigen_outline_width;
    Color serum_outline_color, reference_antigen_outline_color, test_antigen_outline_color, test_antigen_fill_color, vaccine_antigen_outline_color, sequenced_antigen_outline_color, sequenced_antigen_fill_color, tracked_antigen_outline_color;
    bool tracked_antigen_colored_by_clade;
    Color tracked_antigen_color;
    double reassortant_rotation, egg_antigen_aspect;
    bool show_tracked_sera;
    Color serum_circle_color, tracked_serum_outline_color;
    double serum_circle_thickness, tracked_serum_outline_width;
    Color map_title_color;
    Size map_title_offset;      // offset of the top left corner of the text in letter W fraction
    double map_title_size;
    Color mapped_antigens_section_line_color;
    double mapped_antigens_section_line_width;

    std::vector<MarkAntigenSettings> mark_antigens;

    double _width; // obsolete, moved to SignaturePageDrawSettings

      // for json importer
    inline std::vector<MarkAntigenSettings>& get_mark_antigens() { return mark_antigens; }
    inline std::vector<double>& get_transformation() { return transformation; }

}; // class AntigenicMapsDrawSettings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
