#pragma once

#include <iostream>
#include <map>

#pragma GCC diagnostic push
#include "acmacs-base/boost-diagnostics.hh"
#include "boost/variant.hpp"
#pragma GCC diagnostic pop

#include "acmacs-base/transformation.hh"
#include "acmacs-base/settings.hh"
#include "acmacs-draw/surface.hh"

// ----------------------------------------------------------------------

class Tree;
class TreeDraw;
class HzSections;
class SignaturePageDrawSettings;
class TimeSeriesDrawSettings;
class AntigenicMapsDrawSettings;
class MappedAntigensDraw;
class ChartDrawBase;
class AntigenicMapsLayout;
class SettingsInitializer;

// ----------------------------------------------------------------------

class AntigenicMapsDrawBase
{
 public:
    AntigenicMapsDrawBase(acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, TimeSeriesDrawSettings& aTimeSeriesDrawSettings, AntigenicMapsDrawSettings& aSettings);
    virtual ~AntigenicMapsDrawBase();

    virtual void init_settings(const SettingsInitializer& settings_initilizer);
    virtual void prepare();
    virtual void draw(acmacs::surface::Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections);
    virtual ChartDrawBase& chart() = 0;
    virtual const ChartDrawBase& chart() const = 0;
    virtual void make_layout() = 0;

    acmacs::surface::Surface& surface() { return mSurface; }
    const Tree& tree() const { return mTree; }
    const HzSections& hz_sections() const { return mHzSections; }
    SignaturePageDrawSettings& signature_page_settings() { return mSignaturePageDrawSettings; }
    const SignaturePageDrawSettings& signature_page_settings() const { return mSignaturePageDrawSettings; }
    const TimeSeriesDrawSettings& time_series_settings() const { return mTimeSeriesDrawSettings; }
    AntigenicMapsDrawSettings& settings() { return mSettings; }
    const AntigenicMapsDrawSettings& settings() const { return mSettings; }

    AntigenicMapsLayout& layout() { return *mLayout; }
      // const AntigenicMapsLayout& layout() const { return *mLayout; }

 protected:
    void layout(AntigenicMapsLayout* aLayout);

 private:
    acmacs::surface::Surface& mSurface;
    Tree& mTree;
    HzSections& mHzSections;
    // MappedAntigensDraw& mMappedAntigensDraw;
    SignaturePageDrawSettings& mSignaturePageDrawSettings;
    TimeSeriesDrawSettings& mTimeSeriesDrawSettings;
    AntigenicMapsDrawSettings& mSettings;
    std::unique_ptr<AntigenicMapsLayout> mLayout;

}; // class AntigenicMapsDrawBase

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, TimeSeriesDrawSettings& aTimeSeriesDrawSettings, AntigenicMapsDrawSettings& aSettings);

// ----------------------------------------------------------------------

// class SettingDict;
// class SettingList;

// using SettingValue = boost::variant<std::string, double, int, bool, SettingDict, SettingList>;

// using SettingListBase = std::vector<SettingValue>;
// using SettingDictBase = std::map<std::string, SettingValue>;

// class SettingList : public SettingListBase
// {
//  public:
//     SettingList();
//     SettingList(std::initializer_list<SettingValue> init);
// };

// class SettingDict : public SettingDictBase
// {
//  public:
//     SettingDict();
//     SettingDict(std::initializer_list<SettingDictBase::value_type> init);

//     template <typename Value> Value get(std::string aName, Value aDefault) const;
//     std::string get(std::string aName, const char* aDefault) const;
//     Location get(std::string aName, const Location& aDefault) const;
//     Viewport get_viewport() const;
//     const SettingList& get_mods() const;
// };

// inline SettingList::SettingList() {}
// inline SettingList::SettingList(std::initializer_list<SettingValue> init) : SettingListBase(init) {}
// inline SettingDict::SettingDict() {}
// inline SettingDict::SettingDict(std::initializer_list<SettingDictBase::value_type> init) : SettingDictBase(init) {}

// template <typename Value> inline const Value& SettingValue_get(const SettingValue& aValue, const Value& aDefault)
// {
//     const Value* value = boost::get<Value>(&aValue);
//     if (value)
//         return *value;
//     return aDefault;
// }

// inline double SettingValue_get(const SettingValue& aValue, double aDefault)
// {
//     double result = aDefault;
//     const double* dvalue = boost::get<double>(&aValue);
//     if (dvalue) {
//         result = *dvalue;
//     }
//     else {
//         const int* ivalue = boost::get<int>(&aValue);
//         if (ivalue)
//             result = static_cast<double>(*ivalue);
//     }
//     return result;
// }

// inline size_t SettingValue_get(const SettingValue& aValue, size_t aDefault)
// {
//     const int* ivalue = boost::get<int>(&aValue);
//     if (ivalue)
//         return static_cast<size_t>(*ivalue);
//     else
//         return aDefault;
// }

// inline bool SettingValue_get(const SettingValue& aValue, bool aDefault)
// {
//     bool result = aDefault;
//     const bool* bvalue = boost::get<bool>(&aValue);
//     if (bvalue) {
//         result = *bvalue;
//     }
//     else {
//         const int* ivalue = boost::get<int>(&aValue);
//         if (ivalue)
//             result = *ivalue;
//     }
//     return result;
// }

// template <typename Value> inline Value SettingDict::get(std::string aName, Value aDefault) const
// {
//     const auto found = find(aName);
//     if (found != end()) {
//         const auto* value = boost::get<Value>(&found->second);
//         if (value)
//             return *value;
//     }
//     return aDefault;
// }

// inline std::string SettingDict::get(std::string aName, const char* aDefault) const
// {
//     return get(aName, std::string{aDefault});
// }

// inline Location SettingDict::get(std::string aName, const Location& aDefault) const
// {
//     const auto found = find(aName);
//     if (found == end())
//         return aDefault;
//     const auto* list = boost::get<SettingList>(&found->second);
//     if (list && list->size() == 2 && std::all_of(list->begin(), list->end(), [](const auto& e) -> bool { return boost::get<double>(&e); }))
//         return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1])};
//     else
//         return {};
// }

// inline Viewport SettingDict::get_viewport() const
// {
//     const auto found = find("viewport");
//     if (found == end())
//         return {};
//     const auto* list = boost::get<SettingList>(&found->second);
//     if (list && std::all_of(list->begin(), list->end(), [](const auto& e) -> bool { return boost::get<double>(&e); })) {
//         switch (list->size()) {
//           case 3:
//               return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1]), *boost::get<double>(&(*list)[2])};
//           case 4:
//               return {*boost::get<double>(&(*list)[0]), *boost::get<double>(&(*list)[1]), *boost::get<double>(&(*list)[2]), *boost::get<double>(&(*list)[3])};
//         }
//     }
//     return {};
// }

// inline const SettingList& SettingDict::get_mods() const
// {
//     const auto found = find("mods");
//     if (found != end()) {
//         const auto* list = boost::get<SettingList>(&found->second);
//         if (list)
//             return *list;
//     }
// #pragma GCC diagnostic push
// #ifdef __clang__
// #pragma GCC diagnostic ignored "-Wexit-time-destructors"
// #endif
//     static const SettingList empty;
// #pragma GCC diagnostic pop
//     return empty;
// }

// ----------------------------------------------------------------------

class LabelSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<bool> show{this, "show"};
    acmacs::settings::field<std::string> name_type{this, "name_type"};
    acmacs::settings::field<acmacs::Offset> offset{this, "offset"};
    acmacs::settings::field<double> size{this, "size"};
};

class SelectVaccineSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string> passage{this, "passage"}, type{this, "type"}, name{this, "name"};
};

class SelectSettings : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string> full_name{this, "full_name"};
    acmacs::settings::field_object<SelectVaccineSettings> vaccine{this, "vaccine"};
};

// ----------------------------------------------------------------------

class AntigenicMapMod : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string> name{this, "N"};
    acmacs::settings::field<std::string> name_commented{this, "?N"};
    acmacs::settings::field_array<double> rel{this, "rel"}, viewport{this, "viewport"}, viewport_commented{this, "?viewport"}; // viewport
    acmacs::settings::field<double> outline_scale{this, "outline_scale"}, scale{this, "scale"}; // point_scale
    acmacs::settings::field<Color> outline{this, "outline"}, fill{this, "fill"}, text_color{this, "text_color"}, color{this, "color"};
    acmacs::settings::field<double> outline_width{this, "outline_width"}, size{this, "size"}, text_size{this, "text_size"}, line_width{this, "line_width"};
    acmacs::settings::field<std::string> font_family{this, "font_family"}, slant{this, "slant"}, weight{this, "weight"};  // title
    acmacs::settings::field<acmacs::Offset> offset{this, "offset"};  // title
    acmacs::settings::field<double> padding{this, "padding"}; // title
    acmacs::settings::field<double> degrees{this, "degrees"}, radians{this, "radians"}, angle{this, "angle"}; // rotate
    acmacs::settings::field<std::string> direction{this, "direction"}; // flip
    acmacs::settings::field_array<double> value{this, "value"}; // flip
    acmacs::settings::field_array<size_t> ignore_sera{this, "ignore_sera"}; // "tracked_serum_circles"
    acmacs::settings::field_object<LabelSettings> label{this, "label"}; // antigens
    acmacs::settings::field_object<SelectSettings> select{this, "select"}; // antigens
    acmacs::settings::field<bool> fill_by_date{this, "fill_by_date"};
    acmacs::settings::field<bool> report{this, "report"}, raise_if_not_found{this, "raise_if_not_found"}, raise_{this, "raise_"}, shown_on_all{this, "shown_on_all"}; // antigens
    acmacs::settings::field<std::string> order{this, "order"}; // antigens
    acmacs::settings::field<std::string> map{this, "map"}, serum{this, "serum"}, radius_line_dash{this, "radius_line_dash"}; // serum_circle
    acmacs::settings::field<double> radius_line_width{this, "radius_line_width"}, serum_size{this, "serum_size"}, serum_outline_width{this, "serum_outline_width"}; // serum_circle
    acmacs::settings::field<Color> radius_line{this, "radius_line"}, serum_outline{this, "serum_outline"}; // serum_circle
    acmacs::settings::field<double> serum_circle_radius_if_not_calculated{this, "serum_circle_radius_if_not_calculated"};

    acmacs::Viewport get_viewport(const acmacs::Viewport& aOrigViewport) const;

}; // class AntigenicMapMod

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettings : public acmacs::settings::object
{
 public:
    AntigenicMapsDrawSettings(acmacs::settings::base& parent);

    acmacs::settings::field<std::string>              layout{this, "layout", "labelled_grid"};
    acmacs::settings::field<size_t>                   columns{this, "columns", 3};
    acmacs::settings::field<double>                   gap{this, "gap", 20};
    acmacs::settings::field<Color>                    mapped_antigens_section_line_color{this, "mapped_antigens_section_line_color", BLACK};
    acmacs::settings::field<double>                   mapped_antigens_section_line_width{this, "mapped_antigens_section_line_width", 1};
    acmacs::settings::field_array_of<AntigenicMapMod> mods{this, "mods"};

    void viewport(const acmacs::Viewport& aViewport);
    void viewport_rel(const std::vector<double>& rel);
    void rotate_degrees(double degrees);
    void flip(std::string direction);

 private:
    void add_viewport_mod();

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
//     Offset label_offset;
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
//     acmacs::Transformation transformation;
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
