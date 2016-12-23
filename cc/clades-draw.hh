#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "surface.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class TreeDraw;

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
    inline CladeData() = default;
    inline CladeData(const Node& node) : sections{{&node}} {}
    void extend(const Node& node, size_t section_inclusion_tolerance);
    void remove_small_sections(size_t section_exclusion_tolerance);

    std::vector<CladeSection> sections;
};

using Clades = std::map<std::string, CladeData>; // clade name to data

std::ostream& operator << (std::ostream& out, const CladeSection& section);
std::ostream& operator << (std::ostream& out, const CladeData& clade);

// ----------------------------------------------------------------------

class CladeDrawSettings
{
 public:
    inline CladeDrawSettings()
        : section_inclusion_tolerance(2), section_exclusion_tolerance(2), show_section_size_in_label(true),
          exclude(false)
        {}
    inline CladeDrawSettings(const CladeDrawSettings&) = default;
    inline CladeDrawSettings(CladeDrawSettings&&) = default;
    ~CladeDrawSettings();

    std::string name;           // empty for default settings
    size_t section_inclusion_tolerance; // max number of lines (strains) within section from another clade that do not interrupt the secion
    size_t section_exclusion_tolerance; // max number of lines (strains) to exclude small sections
    bool show_section_size_in_label;
    bool exclude; // exclude this clade

}; // class CladesDrawSettings

// ----------------------------------------------------------------------

class CladesDrawSettings
{
 public:
    inline CladesDrawSettings()
        : clades{{CladeDrawSettings{}}} {}
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

}; // class CladesDrawSettings

// ----------------------------------------------------------------------

class CladesDraw
{
 public:
    inline CladesDraw(Surface& aSurface, Tree& aTree, const TreeDraw& aTreeDraw, const CladesDrawSettings& aSettings)
        : mSurface(aSurface), mTree(aTree), mTreeDraw(aTreeDraw), mSettings(aSettings) {}

    void prepare();
    void draw();

 private:
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const CladesDrawSettings& mSettings;
    Clades mClades;

    void hide_old_clades();
    void assign_slots();

}; // class CladesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
