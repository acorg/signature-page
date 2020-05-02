#include "acmacs-base/color-continent.hh"
#include "acmacs-base/color-distinct.hh"
#include "acmacs-draw/continent-map.hh"

#include "coloring.hh"
#include "tree.hh"
#include "legend.hh"

// ----------------------------------------------------------------------

Color ColoringByContinent::color(const Node& aNode) const
{
    return acmacs::continent_color(aNode.data.continent);

} // ColoringByContinent::color

// ----------------------------------------------------------------------

class ColoringByContinentMapLegend : public Legend
{

 public:
    ColoringByContinentMapLegend() = default;

    virtual void draw(acmacs::surface::Surface& aSurface, const LegendSettings& /*aSettings*/) const
        {
            continent_map_draw(aSurface);
        }

    virtual acmacs::Size size() const
        {
            return continent_map_size();
        }

}; // class ColoringByContinentMapLegend

// ----------------------------------------------------------------------

Legend* ColoringByContinent::legend() const
{
    return new ColoringByContinentMapLegend();

} // ColoringByContinent::legend

// ----------------------------------------------------------------------

void ColoringByPos::color_for_aa(const std::map<std::string, std::string>& colors)
{
    colors_ = colors;

} // ColoringByPos::color_for_aa

// ----------------------------------------------------------------------

Color ColoringByPos::color(const Node& aNode) const
{
    Color c("pink");
    const auto amino_acids = aNode.data.amino_acids();
    if (amino_acids.size() > mPos) {
        const char aa = amino_acids[mPos];
        if (!colors_.empty()) {
            try {
                c = colors_.at(std::string(1, aa));
                if (auto found = mUsed.find(aa); found == mUsed.end())
                    mUsed[aa] = std::make_pair(c, 1);
                else
                    ++found->second.second;
            }
            catch (std::out_of_range&) {
                throw std::runtime_error(std::string{"\"color_for_aa\" in settings does not provide color for "} + aa);
            }
        }
        else {
            auto distinct_colors = acmacs::color::distinct();
            try {
                auto& cc = mUsed.at(aa);
                ++cc.second;
                c = cc.first;
            }
            catch (std::out_of_range&) {
                if (aa != 'X') // X is always black
                    c = distinct_colors[mUsed.size()];
                mUsed[aa] = std::make_pair(c, 1);
            }
        }
    }
    return c;

} // ColoringByPos::color

// ----------------------------------------------------------------------

void ColoringByPos::report() const
{
    std::cout << "ColoringByPos: " << mUsed.size();
    for (const auto& u: mUsed)
        std::cout << fmt::format(" [{} {} {}]", u.first, u.second.first, u.second.second);
    std::cout << '\n';

} // ColoringByPos::report

// ----------------------------------------------------------------------

class ColoringByPosLegend : public Legend
{
 public:
    ColoringByPosLegend(const ColoringByPos& aColoring)
        : Legend(), mColoring(aColoring), mTitle(std::to_string(mColoring.pos() + 1)) {}

    virtual void draw(acmacs::surface::Surface& aSurface, const LegendSettings& aSettings) const
        {
              // aSurface.border(0xA0FFA000, 1);
            const auto title_size = aSurface.text_size(mTitle, Pixels{*aSettings.title_size}, aSettings.title_style);
            acmacs::PointCoordinates origin(0, title_size.height);
              //origin += Size((aSurface.text_size(mTitle, mFontSize, mStyle).width - label_size.width) / 2, label_size.height * mInterline);
            const auto text_size = aSurface.text_size("W", Pixels{*aSettings.text_size}, aSettings.text_style);
            double max_width = 0;
            for (auto& label_color: mColoring.used_colors()) {
                origin.y(origin.y() + text_size.height * aSettings.interline);
                const std::string text = std::string(1, label_color.first) + " (" + std::to_string(label_color.second.second) + ")";
                aSurface.text(origin, text, label_color.second.first, Pixels{*aSettings.text_size}, aSettings.text_style);
                max_width = std::max(max_width, aSurface.text_size(text, Pixels{*aSettings.text_size}, aSettings.text_style).width);
            }
            aSurface.text({(max_width - title_size.width) / 2, title_size.height}, mTitle, BLACK, Pixels{*aSettings.title_size}, aSettings.title_style);
        }

    virtual acmacs::Size size() const
        {
            return {static_cast<double>(mTitle.size()) * 10.0, static_cast<double>(mColoring.used_colors().size() + 1) * 10.0};
        }

 private:
    const ColoringByPos& mColoring;
    std::string mTitle;

}; // class ColoringByContinentLegend

Legend* ColoringByPos::legend() const
{
    return new ColoringByPosLegend(*this);

} // ColoringByPos::legend

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
