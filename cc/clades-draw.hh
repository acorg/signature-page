#pragma once

#include <string>
#include <map>

#include "surface.hh"

// ----------------------------------------------------------------------

class Node;
class Tree;
class TreeDraw;

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
    class CladeData
    {
     public:
        inline CladeData() = default;
        CladeData(const Node& node);
        void extend(const Node& node);

        std::string first_id;
        size_t first_line;
        std::string last_id;
        size_t last_line;
    };

    using Clades = std::map<std::string, CladeData>; // clade name to data

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
