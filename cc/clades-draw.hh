#pragma once

#include <string>

#include "surface.hh"

// ----------------------------------------------------------------------

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
    Surface& mSurface;
    Tree& mTree;
    const TreeDraw& mTreeDraw;
    const CladesDrawSettings& mSettings;

}; // class CladesDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
