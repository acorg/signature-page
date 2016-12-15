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
      // virtual Legend* legend(const SettingsLegend& aSettings) const = 0;
};

// ----------------------------------------------------------------------

class ColoringBlack : public Coloring
{
 public:
    virtual inline Color color(const Node&) const { return 0; }
      // virtual Legend* legend(const SettingsLegend& /*aSettings*/) const { return nullptr; }
};

// ----------------------------------------------------------------------

class ColoringByContinent : public Coloring
{
 public:
    inline Color color(std::string aContinent) const
        {
            try {
                return mContinents.at(aContinent);
            }
            catch (std::out_of_range&) {
                return mContinents.at("UNKNOWN");
            }
        }

    virtual Color color(const Node& aNode) const;

      // virtual Legend* legend(const SettingsLegend& aSettings) const;

    constexpr static const char* const ContinentLabels[] = {"EUROPE", "CENTRAL-AMERICA", "MIDDLE-EAST", "NORTH-AMERICA", "AFRICA", "ASIA", "RUSSIA", "AUSTRALIA-OCEANIA", "SOUTH-AMERICA"};

 private:
    static const std::map<std::string, Color> mContinents;
};


// ----------------------------------------------------------------------

class ColoringByPos : public Coloring
{
 public:
    inline ColoringByPos(size_t aPos) : mPos(aPos), mColorsUsed(0) {}

    virtual Color color(const Node& aNode) const;
    // virtual Legend* legend(const SettingsLegend& aSettings) const;
    size_t pos() const { return mPos; }

    inline const std::map<char, Color>& aa_color() const { return mUsed; }

 private:
    size_t mPos;
    mutable size_t mColorsUsed;
    mutable std::map<char, Color> mUsed;
};

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
