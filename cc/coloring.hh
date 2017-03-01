#pragma once

#include <map>

#include "acmacs-base/color.hh"

// ----------------------------------------------------------------------

class Node;
class Legend;
class SettingsLegend;

// ----------------------------------------------------------------------

class Coloring
{
 public:
    virtual ~Coloring() = default;
    virtual Color color(const Node&) const = 0;
    virtual Legend* legend() const = 0;
    virtual inline void report() const {}
};

// ----------------------------------------------------------------------

class ColoringBlack : public Coloring
{
 public:
    virtual inline Color color(const Node&) const { return 0; }
    virtual Legend* legend() const { return nullptr; }
};

// ----------------------------------------------------------------------

class ColoringByContinent : public Coloring
{
 public:
    virtual Color color(const Node& aNode) const;
    virtual Legend* legend() const;

}; // class ColoringByContinent


// ----------------------------------------------------------------------

class ColoringByPos : public Coloring
{
 public:
    using UsedColors = std::map<char, std::pair<Color, size_t>>;

    inline ColoringByPos(size_t aPos) : mPos(aPos - 1) {}

    virtual Color color(const Node& aNode) const;
    virtual Legend* legend() const;
    size_t pos() const { return mPos; }
    inline const UsedColors& used_colors() const { return mUsed; }

    virtual void report() const;

 private:
    size_t mPos;
    mutable UsedColors mUsed;
};

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
