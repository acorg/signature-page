#pragma once

#include <map>

#include "acmacs-base/color.hh"
#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;
class HzSections;

// ----------------------------------------------------------------------

class AAAtPosSection : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<size_t>      pos{this, "_pos"};
    acmacs::settings::field<std::string> aa{this, "aa"};
    acmacs::settings::field<size_t>      num{this, "num"};
    acmacs::settings::field<std::string> first{this, "first"};
    acmacs::settings::field<std::string> last{this, "last"};
};

// ----------------------------------------------------------------------

class AAAtPosDrawSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<double>                  width{this, "width", 0};
    acmacs::settings::field<double>                  right_margin{this, "right_margin", 0};
    acmacs::settings::field<double>                  line_width{this, "line_width", 0.15};
    acmacs::settings::field<double>                  line_length{this, "line_length", 0.5};         // fraction of the surface width
    acmacs::settings::field<size_t>                  diverse_index_threshold{this, "diverse_index_threshold", 3};
    acmacs::settings::field_array<size_t>            positions{this, "positions"};
    acmacs::settings::field<bool>                    report_most_diverse_positions{this, "report_most_diverse_positions", false};
    acmacs::settings::field<size_t>                  small_section_threshold{this, "small_section_threshold", 3}; // remove sections having this or fewer number of sequences
    acmacs::settings::field_array_of<AAAtPosSection> sections{this, "?sections"};

}; // class AAAtPosDrawSettings

// ----------------------------------------------------------------------

class AAAtPosDraw
{
 public:
    AAAtPosDraw(acmacs::surface::Surface& aSurface, const Tree& aTree, HzSections& aHzSections, AAAtPosDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections), mSettings(aSettings) {}

    void prepare();
    void draw(bool init_settings, size_t hz_section_threshold);

    acmacs::surface::Surface& surface() { return mSurface; }

 private:
    acmacs::surface::Surface& mSurface;
    const Tree& mTree;
    HzSections& mHzSections;
    AAAtPosDrawSettings& mSettings;
    std::vector<size_t> positions_;
    std::map<size_t, std::map<char, size_t>> aa_per_pos_;
    std::map<size_t, std::map<char, Color>> colors_;

    void collect_aa_per_pos();
    void find_most_diverse_positions();
    void set_colors();
    void draw_hz_section_lines() const;
    void make_aa_pos_sections(bool init_settings, size_t hz_section_threshold);

}; // class AAAtPosDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
