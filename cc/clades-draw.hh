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
    void extend(const Node& node);

    std::vector<CladeSection> sections;
};

using Clades = std::map<std::string, CladeData>; // clade name to data

std::ostream& operator << (std::ostream& out, const CladeSection& section);
std::ostream& operator << (std::ostream& out, const CladeData& clade);

// ----------------------------------------------------------------------

class CladesDrawSettings
{
 public:
    inline CladesDrawSettings()
        {}
    ~CladesDrawSettings();

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

    void add_clade(const std::pair<std::string, size_t>& aBegin, const std::pair<std::string, size_t>& aEnd, std::string aLabel, std::string aId);
    void hide_old_clades();
    void assign_slots();

}; // class CladesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
