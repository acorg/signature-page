#pragma once

#include <map>

#include "acmacs-base/color.hh"
#include "acmacs-draw/surface.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class ChartDrawBase;
class HzSections;

// ----------------------------------------------------------------------

class AAAtPosSection : public rjson::array_field_container_child_element
{
 public:
    AAAtPosSection(const rjson::value& aData);

    rjson::field_get_set<size_t> pos;
    rjson::field_get_set<std::string> aa;
    rjson::field_get_set<size_t> num;
    rjson::field_get_set<std::string> first;
    rjson::field_get_set<std::string> last;
};

class AAAtPosDrawSettings : public rjson::field_container_child
{
 public:
    AAAtPosDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    rjson::field_get_set<double> width;
    rjson::field_get_set<double> right_margin;
    rjson::field_get_set<double> line_width;
    rjson::field_get_set<double> line_length;         // fraction of the surface width
    rjson::field_get_set<size_t> diverse_index_threshold;
    rjson::array_field_container_child<size_t> positions;
    rjson::field_get_set<bool> report_most_diverse_positions;
    rjson::field_get_set<size_t> small_section_threshold; // remove sections having this or fewer number of sequences
    rjson::array_field_container_child<AAAtPosSection> sections;

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
