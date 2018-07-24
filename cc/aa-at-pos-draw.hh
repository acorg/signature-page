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

}; // class AAAtPosDrawSettings

// ----------------------------------------------------------------------

class AAAtPosDraw
{
 public:
    AAAtPosDraw(acmacs::surface::Surface& aSurface, const Tree& aTree, const HzSections& aHzSections, AAAtPosDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections), mSettings(aSettings) {}

    void prepare();
    void draw();

    acmacs::surface::Surface& surface() { return mSurface; }

 private:
    acmacs::surface::Surface& mSurface;
    const Tree& mTree;
    const HzSections& mHzSections;
    AAAtPosDrawSettings& mSettings;
    std::vector<size_t> positions_;
    std::map<size_t, std::map<char, size_t>> aa_per_pos_;
    std::map<size_t, std::map<char, Color>> colors_;

    void collect_aa_per_pos();
    void find_most_diverse_positions();
    void draw_hz_section_lines();

}; // class AAAtPosDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
