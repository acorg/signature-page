#pragma once

#include <iostream>
#include <map>

#pragma GCC diagnostic push
#include "acmacs-base/boost-diagnostics.hh"
#include "boost/variant.hpp"
#pragma GCC diagnostic pop

#include "acmacs-base/transformation.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;
class SignaturePageDrawSettings;
class AntigenicMapsDrawSettings;
class MappedAntigensDraw;
class ChartDrawBase;
class AntigenicMapsLayout;

// ----------------------------------------------------------------------

class AntigenicMapsDrawBase
{
 public:
    AntigenicMapsDrawBase(Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings);
    virtual ~AntigenicMapsDrawBase();

    virtual void init_settings();
    virtual void prepare();
    virtual void draw(Surface& aMappedAntigensDrawSurface);
    virtual ChartDrawBase& chart() = 0;
    virtual const ChartDrawBase& chart() const = 0;
    virtual void make_layout() = 0;

    inline Surface& surface() { return mSurface; }
    inline const Tree& tree() const { return mTree; }
    inline const HzSections& hz_sections() const { return mHzSections; }
    inline SignaturePageDrawSettings& signature_page_settings() { return mSignaturePageDrawSettings; }
    inline const SignaturePageDrawSettings& signature_page_settings() const { return mSignaturePageDrawSettings; }
    inline AntigenicMapsDrawSettings& settings() { return mSettings; }
    inline const AntigenicMapsDrawSettings& settings() const { return mSettings; }

    AntigenicMapsLayout& layout() { return *mLayout; }
      // const AntigenicMapsLayout& layout() const { return *mLayout; }

 protected:
    void layout(AntigenicMapsLayout* aLayout);

 private:
    Surface& mSurface;
    Tree& mTree;
    HzSections& mHzSections;
    // MappedAntigensDraw& mMappedAntigensDraw;
    SignaturePageDrawSettings& mSignaturePageDrawSettings;
    AntigenicMapsDrawSettings& mSettings;
    std::unique_ptr<AntigenicMapsLayout> mLayout;

}; // class AntigenicMapsDrawBase

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings);

// ----------------------------------------------------------------------

class SettingDict;
class SettingList;

using SettingValue = boost::variant<std::string, double, int, bool, SettingDict, SettingList>;

using SettingListBase = std::vector<SettingValue>;
using SettingDictBase = std::map<std::string, SettingValue>;

class SettingList : public SettingListBase
{
 public:
    SettingList();
    using SettingListBase::SettingListBase;
};

class SettingDict : public SettingDictBase
{
 public:
    SettingDict();
    using SettingDictBase::SettingDictBase;

    template <typename Value> Value get(std::string aName, Value aDefault) const;
    std::string get(std::string aName, const char* aDefault) const;
    Location get(std::string aName, const Location& aDefault) const;
    Viewport get_viewport() const;
    const SettingList& get_mods() const;
};

inline SettingList::SettingList() {}
inline SettingDict::SettingDict() {}

template <typename Value> inline const Value& SettingValue_get(const SettingValue& aValue, const Value& aDefault)
{
    const Value* value = boost::get<Value>(&aValue);
    if (value)
        return *value;
    return aDefault;
}

inline double SettingValue_get(const SettingValue& aValue, double aDefault)
{
    double result = aDefault;
    const double* dvalue = boost::get<double>(&aValue);
    if (dvalue) {
        result = *dvalue;
    }
    else {
        const int* ivalue = boost::get<int>(&aValue);
        if (ivalue)
            result = static_cast<double>(*ivalue);
    }
    return result;
}

inline size_t SettingValue_get(const SettingValue& aValue, size_t aDefault)
{
    const int* ivalue = boost::get<int>(&aValue);
    if (ivalue)
        return static_cast<size_t>(*ivalue);
    else
        return aDefault;
}

inline bool SettingValue_get(const SettingValue& aValue, bool aDefault)
{
    bool result = aDefault;
    const bool* bvalue = boost::get<bool>(&aValue);
    if (bvalue) {
        result = *bvalue;
    }
    else {
        const int* ivalue = boost::get<int>(&aValue);
        if (ivalue)
            result = *ivalue;
    }
    return result;
}

template <typename Value> inline Value SettingDict::get(std::string aName, Value aDefault) const
{
    const auto found = find(aName);
    if (found != end()) {
        const auto* value = boost::get<Value>(&found->second);
        if (value)
            return *value;
    }
    return aDefault;
}

inline std::string SettingDict::get(std::string aName, const char* aDefault) const
{
    return get(aName, std::string{aDefault});
}

inline Location SettingDict::get(std::string aName, const Location& aDefault) const
{
    const auto found = find(aName);
    if (found == end())
        return aDefault;
    const auto* list = boost::get<SettingList>(&found->second);
    if (list && list->size() == 2 && std::all_of(list->begin(), list->end(), [](const auto& e) -> bool { return boost::get<double>(&e); }))
        return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1])};
    else
        return {};
}

inline Viewport SettingDict::get_viewport() const
{
    const auto found = find("viewport");
    if (found == end())
        return {};
    const auto* list = boost::get<SettingList>(&found->second);
    if (list && std::all_of(list->begin(), list->end(), [](const auto& e) -> bool { return boost::get<double>(&e); })) {
        switch (list->size()) {
          case 3:
              return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1]), *boost::get<double>(&(*list)[2])};
          case 4:
              return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1]), *boost::get<double>(&(*list)[2]), *boost::get<double>(&(*list)[3])};
        }
    }
    return {};
}

inline const SettingList& SettingDict::get_mods() const
{
    const auto found = find("mods");
    if (found != end()) {
        const auto* list = boost::get<SettingList>(&found->second);
        if (list)
            return *list;
    }
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif
    static const SettingList empty;
#pragma GCC diagnostic pop
    return empty;
}

// ----------------------------------------------------------------------

class AntigenicMapMod : public SettingDict
{
 public:
    // AntigenicMapMod();
    using SettingDict::SettingDict;
    // ~AntigenicMapMod();

    inline std::string name() const { return get("N", std::string{}); }

}; // class AntigenicMapMod

class AntigenicMapsDrawSettings
{
 public:
    AntigenicMapsDrawSettings();
    ~AntigenicMapsDrawSettings();

    std::string layout;
    size_t columns;
    double gap;
    Color mapped_antigens_section_line_color;
    double mapped_antigens_section_line_width;
    std::vector<AntigenicMapMod> mods;

    inline std::vector<AntigenicMapMod>& get_mods() { return mods; }
    void viewport(const Viewport& aViewport);

}; // class AntigenicMapsDrawSettings

// ----------------------------------------------------------------------

class MarkAntigenSettings
{
};

// class MarkAntigenSettings
// {
//  public:
//     MarkAntigenSettings(bool aShow = true, std::string aName = std::string{});

//     bool show;
//     std::string name, label;
//     double scale, aspect, rotation, outline_width, label_line_width;
//     Color fill_color, outline_color, label_color, label_line_color;
//     Size label_offset;
//     double label_size;

// }; // class MarkAntigenSettings

// // ----------------------------------------------------------------------

// class AntigenicMapsDrawSettings
// {
//  public:
//     AntigenicMapsDrawSettings();
//     ~AntigenicMapsDrawSettings();

//     std::string layout;
//     size_t columns;
//     double gap;
//     Transformation transformation;
//     Viewport viewport;

//     double border_width, grid_line_width;
//     Color border_color, grid_line_color, background_color;

//     double point_scale;
//     double serum_scale, reference_antigen_scale, test_antigen_scale, vaccine_antigen_scale, tracked_antigen_scale;
//     double serum_outline_width, reference_antigen_outline_width, test_antigen_outline_width, vaccine_antigen_outline_width, sequenced_antigen_outline_width, tracked_antigen_outline_width;
//     Color serum_outline_color, reference_antigen_outline_color, test_antigen_outline_color, test_antigen_fill_color, vaccine_antigen_outline_color, sequenced_antigen_outline_color, sequenced_antigen_fill_color, tracked_antigen_outline_color;
//     bool tracked_antigen_colored_by_clade;
//     Color tracked_antigen_color;
//     double reassortant_rotation, egg_antigen_aspect;
//     bool show_tracked_sera;
//     Color serum_circle_color, tracked_serum_outline_color;
//     double serum_circle_thickness, tracked_serum_outline_width;
//     Color map_title_color;
//     Size map_title_offset;      // offset of the top left corner of the text in letter W fraction
//     double map_title_size;
//     Color mapped_antigens_section_line_color;
//     double mapped_antigens_section_line_width;

//     std::vector<MarkAntigenSettings> mark_antigens;

//     double _width; // obsolete, moved to SignaturePageDrawSettings

//       // for json importer
//     inline std::vector<MarkAntigenSettings>& get_mark_antigens() { return mark_antigens; }
//     inline std::vector<double>& get_transformation() { return transformation; }

// }; // class AntigenicMapsDrawSettings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
