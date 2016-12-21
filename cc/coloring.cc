#include "coloring.hh"
#include "tree.hh"
#include "continent-path.hh"
#include "legend.hh"

// ----------------------------------------------------------------------

const char* const ColoringByContinent::ContinentLabels[9] = {
    "EUROPE", "CENTRAL-AMERICA", "MIDDLE-EAST", "NORTH-AMERICA", "AFRICA", "ASIA", "RUSSIA", "AUSTRALIA-OCEANIA", "SOUTH-AMERICA" //, "ANTARCTICA"
};

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif

const std::map<std::string, Color> ColoringByContinent::mContinents = {
    {"EUROPE",            0x00FF00},
    {"CENTRAL-AMERICA",   0xAAF9FF},
    {"MIDDLE-EAST",       0x8000FF},
    {"NORTH-AMERICA",     0x00008B},
    {"AFRICA",            0xFF8000},
    {"ASIA",              0xFF0000},
    {"RUSSIA",            0xB03060},
    {"AUSTRALIA-OCEANIA", 0xFF69B4},
    {"SOUTH-AMERICA",     0x40E0D0},
    {"ANTARCTICA",        0x808080},
    {"CHINA-SOUTH",       0xFF0000},
    {"CHINA-NORTH",       0x6495ED},
    {"CHINA-UNKNOWN",     0x808080},
    {"UNKNOWN",           0x808080},
};

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

Color ColoringByContinent::color(const Node& aNode) const
{
    return color(aNode.data.continent);

} // ColoringByContinent::color

// ----------------------------------------------------------------------

// class ColoringByContinentLegend : public Legend
// {

//  public:
//     inline ColoringByContinentLegend(const ColoringByContinent& aColoring) : Legend(), mColoring(aColoring) {}

//     virtual void draw(Surface& aSurface, const Viewport& aViewport, const SettingsLegend& aSettings) const
//         {
//             auto const label_size = aSurface.text_size("W", aSettings.font_size, aSettings.style);
//             auto y = aViewport.origin.y + label_size.height;
//             for (const auto& label: ColoringByContinentLegendLabels) {
//                 aSurface.text({aViewport.origin.x, y}, label, mColoring.color(label), aSettings.font_size, aSettings.style);
//                 y += label_size.height * aSettings.interline;
//             }
//         }

//     virtual Size size(Surface& aSurface, const SettingsLegend& aSettings) const
//         {
//             Size size(0, 0);
//             for (const auto& label: ColoringByContinentLegendLabels) {
//                 const auto label_size = aSurface.text_size(label, aSettings.font_size, aSettings.style);
//                 size.height += label_size.height * aSettings.interline;
//                 if (label_size.width > size.width)
//                     size.width = label_size.width;
//             }
//             return size;
//         }

//  private:
//     const ColoringByContinent& mColoring;

// }; // class ColoringByContinentLegend

// ----------------------------------------------------------------------

class ColoringByContinentMapLegend : public Legend
{

 public:
    inline ColoringByContinentMapLegend(const ColoringByContinent& aColoring) : Legend(), mColoring(aColoring) {}

    virtual void draw(Surface& aSurface) const
        {
              // aSurface.border(0xA0FFA000, 10);
              // Color geographic_map_outline_color = 0;
              // double geographic_map_outline_width = 1;
            for (const auto& continent: ColoringByContinent::ContinentLabels) {
                const auto& path = continent_map_path(continent);
                  // aSurface.path_outline(path.first, path.second, geographic_map_outline_color, geographic_map_outline_width);
                aSurface.path_fill(path.first, path.second, mColoring.color(continent));
            }
        }

    virtual Size size() const
        {
            return {continent_map_size[0], continent_map_size[1]};
        }

 private:
    const ColoringByContinent& mColoring;

}; // class ColoringByContinentMapLegend

// ----------------------------------------------------------------------

Legend* ColoringByContinent::legend() const
{
    return new ColoringByContinentMapLegend(*this);
    // Legend* legend;
    // if (aSettings.geographic_map)
    //       // legend = new ColoringByContinentMapLegend(*this);
    // else
    //     legend = new ColoringByContinentLegend(*this);
    // return legend;

} // ColoringByContinent::legend

// ----------------------------------------------------------------------

Color ColoringByPos::color(const Node& aNode) const
{
    Color c("pink");
    const auto amino_acids = aNode.data.amino_acids();
    if (amino_acids.size() > mPos) {
        const char aa = amino_acids[mPos];
        try {
            auto& cc = mUsed.at(aa);
            ++cc.second;
            c = cc.first;
        }
        catch (std::out_of_range&) {
            if (aa != 'X')      // X is always black
                c = Color::DistinctColors[mUsed.size()];
            mUsed[aa] = std::make_pair(c, 1);
        }
    }
    return c;

} // ColoringByPos::color

// ----------------------------------------------------------------------

void ColoringByPos::report() const
{
    std::cout << "ColoringByPos: " << mUsed.size();
    for (const auto& u: mUsed)
        std::cout << " [" << u.first << ' ' << u.second.first << ' ' << u.second.second << ']';
    std::cout << std::endl;

} // ColoringByPos::report

// ----------------------------------------------------------------------

class ColoringByPosLegend : public Legend
{
 public:
    inline ColoringByPosLegend(const ColoringByPos& aColoring)
        : Legend(), mColoring(aColoring), mTitle(std::to_string(mColoring.pos() + 1)), mFontSize(10), mInterline(1.5) {}

    virtual void draw(Surface& aSurface) const
        {
              // aSurface.border(0xA0FFA000, 1);
            const auto label_size = aSurface.text_size("W", mFontSize, mStyle);
            Location origin{0, label_size.height};
            aSurface.text(origin, mTitle, "black", mFontSize, mStyle);
              //origin += Size((aSurface.text_size(mTitle, mFontSize, mStyle).width - label_size.width) / 2, label_size.height * mInterline);
            for (auto& label_color: mColoring.used_colors()) {
                origin.y += label_size.height * mInterline;
                const std::string text = std::string(1, label_color.first) + " (" + std::to_string(label_color.second.second) + ")";
                aSurface.text(origin, text, label_color.second.first, mFontSize, mStyle);
            }
        }

    virtual Size size() const
        {
            const Size label_size{mFontSize * mTitle.size(), mFontSize}; // = aSurface.text_size("W", mFontSize, mStyle);
            return {label_size.width, label_size.height * (mColoring.used_colors().size() + 1)};
        }

 private:
    const ColoringByPos& mColoring;
    std::string mTitle;
    double mFontSize;
    TextStyle mStyle;
    double mInterline;

}; // class ColoringByContinentLegend

Legend* ColoringByPos::legend() const
{
    return new ColoringByPosLegend(*this);

} // ColoringByPos::legend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
