#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "acmacs-draw/surface.hh"
#include "tree.hh"

// ----------------------------------------------------------------------

class TreeDraw;
class TimeSeriesDraw;

// ----------------------------------------------------------------------

class CladeDrawSettings
{
 public:
    constexpr static const size_t NoSlot = static_cast<size_t>(-1);

    CladeDrawSettings(std::string aName = std::string{}, bool aShow = true);
    inline CladeDrawSettings(const CladeDrawSettings&) = default;
    inline CladeDrawSettings(CladeDrawSettings&&) = default;
    ~CladeDrawSettings();

    std::string name;           // empty for default settings
    std::string display_name;
    bool show; // show this clade
    size_t section_inclusion_tolerance; // max number of lines (strains) within section from another clade that do not interrupt the secion
    size_t section_exclusion_tolerance; // max number of lines (strains) to exclude small sections
    bool show_section_size_in_label;
    Color arrow_color;
    double line_width;
    double arrow_width;
    Color separator_color;
    double separator_width;
    std::string label_position; // middle, top, bottom
    Size label_offset;
    Color label_color;
    double label_size;
    TextStyle label_style;
    double label_rotation;
    size_t slot;

}; // class CladeDrawSettings

// ----------------------------------------------------------------------

class CladesDrawSettings
{
 public:
    CladesDrawSettings();
    ~CladesDrawSettings();

    inline const CladeDrawSettings& for_clade(std::string name) const
        {
            auto p = std::find_if(clades.begin(), clades.end(), [&name](const auto& c) { return c.name == name; });
            if (p == clades.end()) {
                p = std::find_if(clades.begin(), clades.end(), [](const auto& c) { return c.name.empty(); });
                if (p == clades.end())
                    throw std::runtime_error("CladeDrawSettings::for_clade");
            }
            return *p;
        }

    std::vector<CladeDrawSettings> clades;
    double slot_width;

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
