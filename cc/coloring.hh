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
    virtual void report() const {}
};

// ----------------------------------------------------------------------

class ColoringBlack : public Coloring
{
 public:
    Color color(const Node&) const override { return 0; }
    Legend* legend() const override { return nullptr; }
};

// ----------------------------------------------------------------------

class ColoringByContinent : public Coloring
{
 public:
    Color color(const Node& aNode) const override;
    Legend* legend() const override;

}; // class ColoringByContinent


// ----------------------------------------------------------------------

class ColoringByPos : public Coloring
{
 public:
    using UsedColors = std::map<char, std::pair<Color, size_t>>;

    ColoringByPos(size_t aPos) : mPos(aPos - 1) {}

    Color color(const Node& aNode) const override;
    Legend* legend() const override;
    size_t pos() const { return mPos; }
    const UsedColors& used_colors() const { return mUsed; }
    void color_for_aa(const std::map<std::string, std::string>& colors);

    void report() const override;

 private:
    size_t mPos;
    mutable UsedColors mUsed;
    std::map<std::string, std::string> colors_;
};

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
