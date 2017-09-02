#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "acmacs-draw/surface.hh"
#include "tree.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class TreeDraw;
class TimeSeriesDraw;

// ----------------------------------------------------------------------

class CladeDrawSettings : public rjson::array_field_container_child_element
{
 public:
    constexpr static const size_t NoSlot = static_cast<size_t>(-1);

    CladeDrawSettings(const rjson::value& aData, std::string aName = std::string{}, bool aShow = true);
    inline CladeDrawSettings(const CladeDrawSettings&) = default;
    inline CladeDrawSettings(CladeDrawSettings&&) = default;

    rjson::field_get_set<std::string> name;           // empty for default settings
    rjson::field_get_set<std::string> display_name;
    rjson::field_get_set<bool> show; // show this clade
    rjson::field_get_set<size_t> section_inclusion_tolerance; // max number of lines (strains) within section from another clade that do not interrupt the secion
    rjson::field_get_set<size_t> section_exclusion_tolerance; // max number of lines (strains) to exclude small sections
    rjson::field_get_set<bool> show_section_size_in_label;
    rjson::field_get_set<Color> arrow_color;
    rjson::field_get_set<double> line_width;
    rjson::field_get_set<double> arrow_width;
    rjson::field_get_set<Color> separator_color;
    rjson::field_get_set<double> separator_width;
    rjson::field_get_set<std::string> label_position; // middle, top, bottom
    rjson::field_get_set<Size> label_offset;
    rjson::field_get_set<Color> label_color;
    rjson::field_get_set<double> label_size;
    rjson::field_get_set<TextStyle> label_style;
    rjson::field_get_set<double> label_rotation;
    rjson::field_get_set<size_t> slot;

}; // class CladeDrawSettings

// ----------------------------------------------------------------------

class CladesDrawSettings : public rjson::field_container_child
{
 public:
    CladesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName);

    inline const CladeDrawSettings for_clade(std::string name) const
        {
            auto p = std::find_if(clades.begin(), clades.end(), [&name](const auto& c) { return static_cast<std::string>(c.name) == name; });
            if (p == clades.end()) {
                p = std::find_if(clades.begin(), clades.end(), [](const auto& c) { return c.name.empty(); });
                if (p == clades.end())
                    throw std::runtime_error("ERROR: no clade entry with empty name (for default settings)");
            }
            return *p;
        }

    rjson::array_field_container_child<CladeDrawSettings> clades;
    rjson::field_get_set<double> slot_width;

      // for json importer
    // inline std::vector<CladeDrawSettings>& get_clades() { return clades; }

}; // class CladesDrawSettings

// ----------------------------------------------------------------------

class CladeSection
{
 public:
    inline CladeSection(const Node* node) : first(node), last(node) {}
    const Node* first;
    const Node* last;
};

class CladeData
{
 public:
    inline CladeData() : slot{CladeDrawSettings::NoSlot} {}
    inline CladeData(const Node& node) : sections{{&node}}, slot{CladeDrawSettings::NoSlot} {}

    void extend(const Node& node, size_t section_inclusion_tolerance);
    void remove_small_sections(size_t section_exclusion_tolerance);

    inline const Node* first() const { return sections.front().first; }
    inline size_t first_line() const { return first()->draw.line_no; }
    inline const Node* last() const { return sections.back().last; }
    inline size_t last_line() const { return last()->draw.line_no; }

    inline bool shown() const { return slot != CladeDrawSettings::NoSlot; }

    inline void seq_ids(std::vector<std::pair<std::string, std::string>>& aSeqIds) const
        {
            for (const auto& sec: sections) {
                aSeqIds.emplace_back(sec.first->seq_id, sec.last->seq_id);
            }
        }

    std::vector<CladeSection> sections;
    size_t slot;
};

using Clades = std::map<std::string, CladeData>; // clade name to data

std::ostream& operator << (std::ostream& out, const CladeSection& section);
std::ostream& operator << (std::ostream& out, const CladeData& clade);

// ----------------------------------------------------------------------

class CladesDraw
{
 public:
    inline CladesDraw(Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const TimeSeriesDraw& aTimeSeriesDraw, CladesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mTimeSeriesDraw(aTimeSeriesDraw), mSettings(aSettings) {}

    void prepare();
    void draw();

    void init_settings();
    inline Surface& surface() { return mSurface; }
    inline const Clades* clades() const { return &mClades; }

 private:
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const TimeSeriesDraw& mTimeSeriesDraw;
    CladesDrawSettings& mSettings;
    Clades mClades;

    void collect();
    void assign_slots();
    void draw_right(size_t aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade);
    void draw_left(size_t aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade);

}; // class CladesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
