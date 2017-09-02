#include <cmath>

#include "acmacs-base/debug.hh"
#include "acmacs-base/float.hh"

//#include "acmacs-base/read-file.hh"
#include "acmacs-base/json-writer.hh"
#include "acmacs-base/json-importer.hh"
namespace jsi = json_importer;
namespace jsw = json_writer;

#include "settings.hh"

// ----------------------------------------------------------------------

static constexpr const char* SETTINGS_VERSION_4 = "signature-page-settings-v4";
// static constexpr const char* SETTINGS_VERSION_3 = "signature-page-settings-v3";
// static constexpr const char* SETTINGS_VERSION_2 = "signature-page-settings-v2";

// ----------------------------------------------------------------------

AATransitionPerBranchDrawSettings::AATransitionPerBranchDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      size(*this, "size", 8),
      color(*this, "color", "black"),
      style(*this, "style", {"Courier New"}),
      interline(*this, "interline", 1.2),
      label_offset(*this, "label_offset", {-40, 20}),
      label_connection_line_width(*this, "label_connection_line_width", 0.1),
      label_connection_line_color(*this, "label_connection_line_color", "black")
{
}

AATransitionDrawSettings::AATransitionDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      show(*this, "show", true),
      number_strains_threshold(*this, "number_strains_threshold", 20),
      number_strains_threshold_help(*this, "number_strains_threshold?", "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"),
      show_empty_left(*this, "show_empty_left", false),
      per_branch(*this, "per_branch"),
      show_node_for_left_line(*this, "show_node_for_left_line", false),
      node_for_left_line_color(*this, "node_for_left_line_color", "green"),
      node_for_left_line_width(*this, "node_for_left_line_width", 1)
{
}

TreeDrawVaccineSettings::TreeDrawVaccineSettings(const rjson::value& aData)
    : rjson::array_field_container_child_element(aData),
      name(*this, "name", ""),
      name_help(*this, "name?", "empty for default settings"),
      label_color(*this, "label_color", "black"),
      label_size(*this, "label_size", 10),
      label_style(*this, "label_style", {}),
      line_color(*this, "line_color", "black"),
      line_width(*this, "line_width", 1)
{
}

LegendSettings::LegendSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      offset(*this, "offset", {-30, 950}),
      width(*this, "width", 100),
      title_style(*this, "title_style", {"sans_serif", TextStyle::Slant::Normal, TextStyle::Weight::Bold}),
      title_size(*this, "title_size", 10),
      text_style(*this, "text_style", {"monospace"}),
      text_size(*this, "text_size", 10),
      interline(*this, "interline", 1.5)
{
}

TimeSeriesDrawSettings::TimeSeriesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      begin(*this, "begin", ""),
      end(*this, "end", ""),
      label_size(*this, "label_size", 8),
      label_style(*this, "label_style", {}),
      month_year_to_timeseries_gap(*this, "month_year_to_timeseries_gap", 2),
      month_separator_color(*this, "month_separator_color", "black"),
      month_separator_width(*this, "month_separator_width", 0.5),
      dash_width(*this, "dash_width", 0.5),
      dash_line_width(*this, "dash_line_width", 1)
{
}

CladeDrawSettings::CladeDrawSettings(const rjson::value& aData, std::string aName, bool aShow)
    : rjson::array_field_container_child_element(aData),
      name(*this, "name", aName),
      display_name(*this, "display_name", ""),
      show(*this, "show", aShow),
      section_inclusion_tolerance(*this, "section_inclusion_tolerance", 10),
      section_exclusion_tolerance(*this, "section_exclusion_tolerance", 5),
      show_section_size_in_label(*this, "show_section_size_in_label", true),
      arrow_color(*this, "arrow_color", "black"),
      line_width(*this, "line_width", 0.8),
      arrow_width(*this, "arrow_width", 3),
      separator_color(*this, "separator_color", "grey63"),
      separator_width(*this, "separator_width", 0.5),
      label_position(*this, "label_position", "middle"),
      label_offset(*this, "label_offset", {5, 0}),
      label_color(*this, "label_color", "black"),
      label_size(*this, "label_size", 11),
      label_style(*this, "label_style", {}),
      label_rotation(*this, "label_rotation", 0),
      slot(*this, "slot", NoSlot)
{
}

CladesDrawSettings::CladesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      clades(*this, "clades"),
      slot_width(*this, "slot_width", 10)
{
    clades.emplace_back();
    auto c2 = clades.emplace_back();
    c2.name = "gly";
    c2.show = false;
    auto c3 = clades.emplace_back();
    c3.name = "no-gly";
    c3.show = false;
}

HzSection::HzSection(const rjson::value& aData)
    : rjson::array_field_container_child_element(aData),
      show(*this, "show", true),
      show_line(*this, "show_line", true),
      show_label_in_time_series(*this, "show_label_in_time_series", true),
      show_map(*this, "show_map", true),
      name(*this, "name", ""),
      label(*this, "label", "")
      // first(nullptr),
      // last(nullptr),
      // index("?")
{
}

// HzSection::HzSection(std::string aName, bool aShowLine)
//     : show(true),
// show_line(aShowLine),
// show_label_in_time_series(true),
// show_map(true),
// name(aName),
// first(nullptr),
// last(nullptr),
// index(1, '?')
// {
// }

// HzSection::HzSection(const Node& aFirst, bool aShow, bool aShowLine, bool aShowMap)
//     : show(aShow),
// show_line(aShowLine),
// show_label_in_time_series(false),
// show_map(aShowMap),
// name(aFirst.seq_id),
// first(&aFirst),
// last(nullptr),
// index(1, '?')
// {
// }

HzSections::HzSections(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      vertical_gap(*this, "vertical_gap", 20),
      line_color(*this, "line_color", "grey63"),
      line_width(*this, "line_width", 1),
      ts_label_size(*this, "ts_label_size", 10),
      ts_label_style(*this, "ts_label_style", {}),
      ts_label_color(*this, "ts_label_color", "black"),
      sections(*this, "sections"),
      show_labels_in_time_series_in_tree_mode(*this, "show_labels_in_time_series_in_tree_mode", false)
{
}

MappedAntigensDrawSettings::MappedAntigensDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      width(*this, "width", 10),
      line_width(*this, "line_width", 0.5),
      line_color(*this, "line_color", "grey56"),
      line_length(*this, "line_length", 0.5)
{
}

// ----------------------------------------------------------------------

AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      layout(*this, "layout", "labelled_grid"),
      columns(*this, "columns", 3),
      gap(*this, "gap", 20),
      mapped_antigens_section_line_color(*this, "mapped_antigens_section_line_color", "black"),
      mapped_antigens_section_line_width(*this, "mapped_antigens_section_line_width", 1),
      mods(*this, "mods")
{
      // $$

    // using namespace std::string_literals;
    // mods.push_back(AntigenicMapMod{{"N", "viewport"s}, {"viewport", SettingList{0.0, 0.0, 0.0}}});
    // mods.push_back(AntigenicMapMod{{"N", "point_scale"s}, {"scale", 1.0}, {"outline_scale"s, 1.0}});
    // mods.push_back(AntigenicMapMod{{"N", "rotate_degrees"s}, {"angle", 0.0}});
    // mods.push_back(AntigenicMapMod{{"N", "sera"s}, {"outline", "grey88"s}, {"outline_width", 0.5}, {"size", 5.0}});
    // mods.push_back(AntigenicMapMod{{"N", "reference_antigens"s}, {"fill", "transparent"s}, {"outline", "grey88"s}, {"outline_width", 0.5}, {"size", 5.0}});
    // mods.push_back(AntigenicMapMod{{"N", "test_antigens"s}, {"fill", "grey88"s}, {"outline", "grey88"s}, {"outline_width", 0.5}, {"size", 3.0}});
    // mods.push_back(AntigenicMapMod{{"N", "sequenced_antigens"s}, {"fill", "grey63"s}, {"outline", "white"s}, {"outline_width", 0.5}, {"size", 3.0}});
    // mods.push_back(AntigenicMapMod{{"N", "tracked_antigens"s}, {"fill", "green3"s}, {"outline", "white"s}, {"outline_width", 0.5}, {"size", 5.0}});
    // mods.push_back(AntigenicMapMod{{"N", "tracked_sera"s}, {"fill", "transparent"s}, {"outline", "black"s}, {"outline_width", 0.5}, {"size", 5.0}}); // tracked sera above tracked antigens!
    // mods.push_back(AntigenicMapMod{{"N", "tracked_serum_circles"s}, {"outline", "black"s}, {"outline_width", 0.5}}); // tracked serum circles above tracked antigens!
    // mods.push_back(AntigenicMapMod{{"N", "vaccines"s}, {"mods", SettingList{
    //     SettingDict{{"size", 15}},
    //     SettingDict{{"outline", "white"s}, {"fill", "blue"s}},
    //     SettingDict{{"type", "current"s}, {"fill", "red"s}},
    //     SettingDict{{"type", "current"s}, {"passage", "reassortant"s}, {"fill", "green"s}},
    //     SettingDict{{"type", "surrogate"s}, {"fill", "pink"s}},
    //     SettingDict{{"name", "TEXAS"s}, {"?show", false}},
    //     SettingDict{{"label", SettingDict{{"offset", SettingList{0, 1}}, {"color", "black"s}, {"weight", "normal"s}, {"slant", "normal"s},
    //                                       {"font_family", "helvetica neu"s}, {"size", 9}, {"name_type", "abbreviated_location_with_passage_type"s}}}},
    // }}});

    // mods.push_back(AntigenicMapMod{{"N", "title"s}, {"text_color", "black"s}, {"text_size", 12.0}, {"padding", 3.0}, {"offset", SettingList{0.0, 0.0}}, {"weight", "normal"s}, {"slant", "normal"s}, {"font_family", "san serif"s}});
    // mods.push_back(AntigenicMapMod{{"N", "background"s}, {"color", "white"s}});
    // mods.push_back(AntigenicMapMod{{"N", "grid"s}, {"color", "grey80"s}, {"line_width", 1.0}});
    // mods.push_back(AntigenicMapMod{{"N", "border"s}, {"color", "black"s}, {"line_width", 1.0}});
}

//       vaccine_antigen_scale(15),
//       vaccine_antigen_outline_width(0.5),
//       vaccine_antigen_outline_color("white"),
//       tracked_antigen_colored_by_clade(false),
//       show_tracked_sera(false), serum_circle_color("grey50"), tracked_serum_outline_color("black"), serum_circle_thickness(0.1), tracked_serum_outline_width(0.1),

// AntigenicMapsDrawSettings::AntigenicMapsDrawSettings()
//     : layout("labelled_grid"), columns(3), gap(20),
//       border_width(1), grid_line_width(0.5),
//       border_color("black"), grid_line_color("grey63"), background_color("white"), // 0xFFFFF8
//       point_scale(1), serum_scale(5), reference_antigen_scale(5), test_antigen_scale(3), vaccine_antigen_scale(15), tracked_antigen_scale(5),
//       serum_outline_width(0.5), reference_antigen_outline_width(0.5), test_antigen_outline_width(0.5), vaccine_antigen_outline_width(0.5),
//       sequenced_antigen_outline_width(0.5), tracked_antigen_outline_width(0.5),
//       serum_outline_color("grey88"), reference_antigen_outline_color("grey88"), test_antigen_outline_color("grey88"),
//       test_antigen_fill_color("grey88"), vaccine_antigen_outline_color("white"), sequenced_antigen_outline_color("white"), sequenced_antigen_fill_color("grey63"),
//       tracked_antigen_outline_color("white"), tracked_antigen_colored_by_clade(false), tracked_antigen_color("green3"),
//       reassortant_rotation(0.5 /* M_PI / 6.0 */), egg_antigen_aspect(0.75),
//       show_tracked_sera(false), serum_circle_color("grey50"), tracked_serum_outline_color("black"), serum_circle_thickness(0.1), tracked_serum_outline_width(0.1),
//       map_title_color("black"), map_title_offset{0.3, 0.4}, map_title_size(11),
//       mapped_antigens_section_line_color("black"), mapped_antigens_section_line_width(1),
//       mark_antigens{{true, "? VT 14-002966-VIR SIAT1 (2014-06-29)"}},
//       _width(0)
// {
// }

void AntigenicMapsDrawSettings::viewport(const Viewport& aViewport)
{
    auto make_setting_list = [&aViewport]() -> SettingList {
        if (float_equal(aViewport.size.width, aViewport.size.height))
            return SettingList{aViewport.origin.x, aViewport.origin.y, aViewport.size.width};
        else
            return SettingList{aViewport.origin.x, aViewport.origin.y, aViewport.size.width, aViewport.size.height};
    };
    using namespace std::string_literals;
    AntigenicMapMod viewport_mod{{"N", "viewport"s}, {"viewport", make_setting_list()}};
      // std::cerr << "DEBUG: AntigenicMapsDrawSettings::viewport" << std::endl;
    const auto vpmod = std::find_if(mods.begin(), mods.end(), [](const auto& mod) -> bool { return mod.name() == "viewport"; });
    if (vpmod == mods.end())
        mods.push_back(viewport_mod);
    else
        *vpmod = viewport_mod;

} // AntigenicMapsDrawSettings::viewport

// ----------------------------------------------------------------------

TreeDrawSettings::TreeDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      ladderize(*this, "ladderize", Tree::LadderizeMethod::NumberOfLeaves),
      ladderize_help(*this, "ladderize?", "number-of-leaves or max-edge-length"),
      mods(*this, "mods"),
      mods_help(*this, "mods?", {"mods is a list of objects:", "{mod: root, s1: new-root}", "{mod: hide-isolated-before, s1: date}",
                      "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}", "{mod: before2015-58P-or-146I-or-559I}",
                      "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing", "{mod: hide-one, s1: name-to-hide} - after ladderizing",
                      "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}"
          }),
      force_line_width(*this, "force_line_width", false),
      line_width(*this, "line_width", 1),
      root_edge(*this, "root_edge", 0),
      line_color(*this, "line_color", "black"),
      label_style(*this, "label_style", {}),
      name_offset(*this, "name_offset", 0.3),
      color_nodes(*this, "color_nodes", "continent"),
      aa_transition(*this, "aa_transition"),
      vaccines(*this, "vaccines"),
      legend(*this, "legend")
{
} // TreeDrawSettings::TreeDrawSettings

// ----------------------------------------------------------------------

TitleDrawSettings::TitleDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      title(*this, "title", ""),
      color(*this, "color", "black"),
      size(*this, "size", 12),
      style(*this, "style", {}),
      offset(*this, "offset", {10, 30})
{
}

// ----------------------------------------------------------------------

SignaturePageDrawSettings::SignaturePageDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      layout(*this, "layout", "auto"),
      top(*this, "top", 60),
      bottom(*this, "bottom", 60),
      left(*this, "left", 50),
      right(*this, "right", 20),
      tree_margin_right(*this, "tree_margin_right", 0),
      mapped_antigens_margin_right(*this, "mapped_antigens_margin_right", 30),
      time_series_width(*this, "time_series_width", 400),
      clades_width(*this, "clades_width", 100),
      antigenic_maps_width(*this, "antigenic_maps_width", 500)
{
}


// **********************************************************************

Settings::Settings()
    : signature_page(*this, "signature_page"),
      title(*this, "title"),
      tree_draw(*this, "tree"),
      time_series(*this, "time_series"),
      clades(*this, "clades"),
      hz_sections(*this, "hz_sections"),
      mapped_antigens(*this, "mapped_antigens"),
      antigenic_maps(*this, "antigenic_maps")
{

} // Settings::Settings

// ----------------------------------------------------------------------

// void Settings::set_version(const char* str, size_t length)
// {
//     version.assign(str, length);
//     if (version != SETTINGS_VERSION_4) // && version != SETTINGS_VERSION_3 && version != SETTINGS_VERSION_2)
//         throw std::runtime_error("Unsupported sigp settings version: \"" + version + "\"");
// }

// ----------------------------------------------------------------------

// void Settings::use_json(rjson::value&& aValue)
// {
//     mData = std::move(aValue);
//     distribute_parsed_value();

// } // Settings::use_json

// ----------------------------------------------------------------------

// void Settings::distribute_parsed_value()
// {
//     signature_page.use_json(mData.get_ref("signature_page", rjson::object{}));
//     title.use_json(mData.get_ref("title", rjson::object{}));

//     // tree_draw.use_json(mData.get_ref("tree", rjson::object{}));
//     // time_series.use_json(mData.get_ref("time_series", rjson::object{}));
//     // clades.use_json(mData.get_ref("clades", rjson::object{}));
//     // hz_sections.use_json(mData.get_ref("hz_sections", rjson::object{}));
//     // mapped_antigens.use_json(mData.get_ref("mapped_antigens", rjson::object{}));
//     // antigenic_maps.use_json(mData.get_ref("antigenic_maps", rjson::object{}));


//     // std::cerr << "DEBUG: " << mData << DEBUG_LINE_FUNC << '\n';
//     // std::cerr << "DEBUG: " << signature_page.to_json() << DEBUG_LINE_FUNC << '\n';
//       // std::cerr << "DEBUG: signature_page.layout: " << signature_page.layout << '\n';

// } // Settings::distribute_parsed_value

// ----------------------------------------------------------------------

void Settings::upgrade()             // upgrade to the new version in case old version data provided
{
    // if (version == SETTINGS_VERSION_2) {
    //     signature_page.antigenic_maps_width = antigenic_maps._width;
    //     if (!tree_draw._root.empty())
    //         tree_draw.mods.emplace_back("root", tree_draw._root);
    //     if (!tree_draw._hide_isolated_before.empty())
    //         tree_draw.mods.emplace_back("hide-isolated-before", tree_draw._hide_isolated_before);
    //     if (tree_draw._hide_if_cumulative_edge_length_bigger_than > 0.0)
    //         tree_draw.mods.emplace_back("hide-if-cumulative-edge-length-bigger-than", tree_draw._hide_if_cumulative_edge_length_bigger_than);
    //     if (!tree_draw._hide_if.empty())
    //         tree_draw.mods.emplace_back(tree_draw._hide_if);
    // }
    // else if (version == SETTINGS_VERSION_3) {
    //       // fail, if old version data provided
    //     if (!float_zero(antigenic_maps._width))
    //         throw std::runtime_error("antigenic_maps.width provided, must be signature_page.antigenic_maps_width");
    //     if (!tree_draw._hide_isolated_before.empty())
    //         throw std::runtime_error("tree_draw.hide_isolated_before provided, must be in tree_draw.mods");
    //     if (tree_draw._hide_if_cumulative_edge_length_bigger_than > 0.0)
    //         throw std::runtime_error("tree_draw.hide_if_cumulative_edge_length_bigger_than provided, must be in tree_draw.mods");
    //     if (!tree_draw._hide_if.empty())
    //         throw std::runtime_error("tree_draw.hide_if provided, must be in tree_draw.mods");
    //     if (!tree_draw._root.empty())
    //         throw std::runtime_error("tree_draw.root provided, must be in tree_draw.mods");
    // }

} // Settings::upgrade

// ----------------------------------------------------------------------

class ColorStorer : public jsi::StorerBase
{
 public:
    inline ColorStorer(Color& aTarget) : mTarget(aTarget) {}
    inline virtual Base* String(const char* str, rapidjson::SizeType length) { mTarget.from_string(str, length); return jsi::storers::_i::pop(); }

 private:
    Color& mTarget;
};

template <typename SZ> class SZStorer : public jsi::StorerBase
{
 public:
    inline SZStorer(SZ& aTarget) : mTarget(aTarget), mPos(0) {}

    inline virtual Base* StartArray()
        {
            if (mPos)
                return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected StartArray event"));
            ++mPos;
            return nullptr;
        }

    inline virtual Base* EndArray()
        {
            if (mPos != 3)
                return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected EndArray event"));
            return jsi::storers::_i::pop();
        }

    virtual Base* Double(double d);

    inline virtual Base* Int(int i) { return Double(static_cast<double>(i)); }
    inline virtual Base* Uint(unsigned u) { return Double(static_cast<double>(u)); }

 private:
    SZ& mTarget;
    size_t mPos;
};

template <> jsi::StorerBase* SZStorer<Size>::Double(double d)
{
    switch (mPos) {
      case 1:
          mTarget.width = d;
          break;
      case 2:
          mTarget.height = d;
          break;
      default:
          return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected Double event"));
    }
    ++mPos;
    return nullptr;
}

template <> jsi::StorerBase* SZStorer<Location>::Double(double d)
{
    switch (mPos) {
      case 1:
          mTarget.x = d;
          break;
      case 2:
          mTarget.y = d;
          break;
      default:
          return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected Double event"));
    }
    ++mPos;
    return nullptr;
}

using SizeStorer = SZStorer<Size>;
using LocationStorer = SZStorer<Location>;

class ViewportStorer : public jsi::StorerBase
{
 public:
    inline ViewportStorer(Viewport& aTarget) : mTarget(aTarget), mPos(0) {}

    inline virtual Base* StartArray()
        {
            if (mPos)
                return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected StartArray event"));
            ++mPos;
            return nullptr;
        }

    inline virtual Base* EndArray()
        {
            if (mPos != 4 && mPos != 5)
                return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected EndArray event"));
            return jsi::storers::_i::pop();
        }

    virtual inline Base* Double(double d)
        {
            switch (mPos) {
              case 1:
                  mTarget.origin.x = d;
                  break;
              case 2:
                  mTarget.origin.y = d;
                  break;
              case 3:
                  mTarget.size.width = mTarget.size.height = d;
                  break;
              case 4:
                  mTarget.size.height = d;
                  break;
              default:
                  return jsi::storers::_i::failure(typeid(*this).name() + std::string(": unexpected Double event"));
            }
            ++mPos;
            return nullptr;
        }

    inline virtual Base* Int(int i) { return Double(static_cast<double>(i)); }
    inline virtual Base* Uint(unsigned u) { return Double(static_cast<double>(u)); }

 private:
    Viewport& mTarget;
    size_t mPos;
};

// ----------------------------------------------------------------------

class SettingValueStorer : public jsi::StorerBase
{
 public:
    inline SettingValueStorer(SettingValue& aTarget) : mTarget(aTarget) {}
    inline virtual Base* String(const char* str, rapidjson::SizeType length) { mTarget = std::string(str, length); return jsi::storers::_i::pop(); }
    inline virtual Base* Int(int i) { mTarget = i; return jsi::storers::_i::pop(); }
    inline virtual Base* Uint(unsigned u) { mTarget = static_cast<int>(u); return jsi::storers::_i::pop(); }
    inline virtual Base* Double(double d) { mTarget = d; return jsi::storers::_i::pop(); }
    inline virtual Base* Bool(bool b) { mTarget = b; return jsi::storers::_i::pop(); }
    virtual Base* StartArray();
    virtual Base* StartObject();

 private:
    SettingValue& mTarget;
};

class SettingDictStorer : public jsi::StorerBase
{
 public:
    inline SettingDictStorer(SettingDict& aTarget, bool aPop2 = false) : mTarget(aTarget), mPop2(aPop2) {}
    inline virtual Base* EndObject() { if (mPop2) return jsi::storers::_i::pop2(); else return jsi::storers::_i::pop(); }
    inline virtual Base* Key(const char* str, rapidjson::SizeType length) { std::string key(str, length); auto p = mTarget.emplace(key, SettingValue{}); return new SettingValueStorer(p.first->second); }

 private:
    SettingDict& mTarget;
    bool mPop2;
};

class SettingListStorer : public jsi::StorerBase
{
 public:
    inline SettingListStorer(SettingList& aTarget, bool aPop2 = false) : mTarget(aTarget), mPop2(aPop2) {}
    inline virtual Base* EndArray() { if (mPop2) return jsi::storers::_i::pop2(); else return jsi::storers::_i::pop(); }
    inline virtual Base* String(const char* str, rapidjson::SizeType length) { mTarget.emplace_back(std::string(str, length)); return nullptr; }
    inline virtual Base* Int(int i) { mTarget.emplace_back(i); return nullptr; }
    inline virtual Base* Uint(unsigned u) { mTarget.emplace_back(static_cast<int>(u)); return nullptr; }
    inline virtual Base* Double(double d) { mTarget.emplace_back(d); return nullptr; }
    inline virtual Base* StartObject() { mTarget.emplace_back(SettingDict{}); return new SettingDictStorer(*boost::get<SettingDict>(&mTarget.back()), false); }

 private:
    SettingList& mTarget;
    bool mPop2;
};

SettingValueStorer::Base* SettingValueStorer::StartArray()
{
    mTarget = SettingList{};
    return new SettingListStorer(*boost::get<SettingList>(&mTarget), true);
}

SettingValueStorer::Base* SettingValueStorer::StartObject()
{
    mTarget = SettingDict{};
    return new SettingDictStorer(*boost::get<SettingDict>(&mTarget), true);
}

class AntigenicMapModsStorer : public jsi::StorerBase
{
 public:
    inline AntigenicMapModsStorer(std::vector<AntigenicMapMod>& aTarget) : mTarget(aTarget) {}
    inline virtual Base* StartArray() { mTarget.clear(); return nullptr; }
    inline virtual Base* EndArray() { return jsi::storers::_i::pop(); }
    inline virtual Base* StartObject() { mTarget.emplace_back(); return new SettingDictStorer(mTarget.back()); }

 private:
    std::vector<AntigenicMapMod>& mTarget;
};

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: reading settings from " << aFilename << std::endl;
    aSettings.use_json(rjson::parse_file(aFilename));
      // jsi::import(acmacs_base::read_file(aFilename), aSettings, settings_data);
    aSettings.upgrade();

} // read_settings

// ----------------------------------------------------------------------

// void read_settings(Settings& aSettings, std::string aFilename)
// {
//     jsi::data<TextStyle> style_data = {
//         {"family", jsi::field(static_cast<void (TextStyle::*)(const char*, size_t)>(&TextStyle::font_family))},
//         {"slant", jsi::field(static_cast<void (TextStyle::*)(const char*, size_t)>(&TextStyle::slant))},
//         {"weight", jsi::field(static_cast<void (TextStyle::*)(const char*, size_t)>(&TextStyle::weight))},
//     };

//     jsi::data<SignaturePageDrawSettings> signature_page_data = {
//         {"layout", jsi::field(&SignaturePageDrawSettings::set_layot)},
//         {"top", jsi::field(&SignaturePageDrawSettings::top)},
//         {"bottom", jsi::field(&SignaturePageDrawSettings::bottom)},
//         {"left", jsi::field(&SignaturePageDrawSettings::left)},
//         {"right", jsi::field(&SignaturePageDrawSettings::right)},
//         {"tree_margin_right", jsi::field(&SignaturePageDrawSettings::tree_margin_right)},
//         {"mapped_antigens_margin_right", jsi::field(&SignaturePageDrawSettings::mapped_antigens_margin_right)},
//         {"time_series_width", jsi::field(&SignaturePageDrawSettings::time_series_width)},
//         {"clades_width", jsi::field(&SignaturePageDrawSettings::clades_width)},
//         {"antigenic_maps_width", jsi::field(&SignaturePageDrawSettings::antigenic_maps_width)},
//     };

//     jsi::data<AATransitionPerBranchDrawSettings> per_branch_data = {
//         {"size", jsi::field(&AATransitionPerBranchDrawSettings::size)},
//         {"color", jsi::field<ColorStorer>(&AATransitionPerBranchDrawSettings::color)},
//         {"style", jsi::field(&AATransitionPerBranchDrawSettings::style, style_data)},
//         {"interline", jsi::field(&AATransitionPerBranchDrawSettings::interline)},
//         {"label_offset", jsi::field<SizeStorer>(&AATransitionPerBranchDrawSettings::label_offset)},
//         {"label_connection_line_width", jsi::field(&AATransitionPerBranchDrawSettings::label_connection_line_width)},
//         {"label_connection_line_color", jsi::field<ColorStorer>(&AATransitionPerBranchDrawSettings::label_connection_line_color)},
//     };

//     jsi::data<AATransitionDrawSettings> aa_transition_data = {
//         {"show", jsi::field(&AATransitionDrawSettings::show)},
//         {"number_strains_threshold", jsi::field(&AATransitionDrawSettings::number_strains_threshold)},
//         {"show_empty_left", jsi::field(&AATransitionDrawSettings::show_empty_left)},
//         {"per_branch", jsi::field(&AATransitionDrawSettings::per_branch, per_branch_data)},
//         {"show_node_for_left_line", jsi::field(&AATransitionDrawSettings::show_node_for_left_line)},
//         {"node_for_left_line_color", jsi::field<ColorStorer>(&AATransitionDrawSettings::node_for_left_line_color)},
//         {"node_for_left_line_width", jsi::field(&AATransitionDrawSettings::node_for_left_line_width)},
//     };

//     jsi::data<TreeDrawVaccineSettings> vaccine_data = {
//         {"name", jsi::field(&TreeDrawVaccineSettings::name)},
//         {"label_color", jsi::field<ColorStorer>(&TreeDrawVaccineSettings::label_color)},
//         {"label_size", jsi::field(&TreeDrawVaccineSettings::label_size)},
//         {"label_style", jsi::field(&TreeDrawVaccineSettings::label_style, style_data)},
//         {"line_color", jsi::field<ColorStorer>(&TreeDrawVaccineSettings::line_color)},
//         {"line_width", jsi::field(&TreeDrawVaccineSettings::line_width)},
//     };

//     jsi::data<LegendSettings> legend_data = {
//         {"offset", jsi::field<SizeStorer>(&LegendSettings::offset)},
//         {"width", jsi::field(&LegendSettings::width)},
//         {"title_style", jsi::field(&LegendSettings::title_style, style_data)},
//         {"title_size", jsi::field(&LegendSettings::title_size)},
//         {"text_style", jsi::field(&LegendSettings::text_style, style_data)},
//         {"text_size", jsi::field(&LegendSettings::text_size)},
//         {"interline", jsi::field(&LegendSettings::interline)},
//     };

//     jsi::data<TreeDrawMod> tree_draw_mod_data = {
//         {"mod", jsi::field(&TreeDrawMod::mod)},
//         {"d1", jsi::field(&TreeDrawMod::d1)},
//         {"s1", jsi::field(&TreeDrawMod::s1)},
//         {"s2", jsi::field(&TreeDrawMod::s2)},
//     };

//     jsi::data<TreeDrawSettings> tree_draw_data = {
//         {"ladderize", jsi::field(&TreeDrawSettings::ladderize_from_string)},
//         {"mods", jsi::field(&TreeDrawSettings::get_mods, tree_draw_mod_data)},
//         {"force_line_width", jsi::field(&TreeDrawSettings::force_line_width)},
//         {"line_width", jsi::field(&TreeDrawSettings::line_width)},
//         {"root_edge", jsi::field(&TreeDrawSettings::root_edge)},
//         {"line_color", jsi::field<ColorStorer>(&TreeDrawSettings::line_color)},
//         {"label_style", jsi::field<>(&TreeDrawSettings::label_style, style_data)},
//         {"name_offset", jsi::field(&TreeDrawSettings::name_offset)},
//         {"color_nodes", jsi::field(&TreeDrawSettings::color_nodes)},
//         {"aa_transition", jsi::field(&TreeDrawSettings::aa_transition, aa_transition_data)},
//         {"vaccines", jsi::field(&TreeDrawSettings::get_vaccines, vaccine_data)},
//         {"legend", jsi::field(&TreeDrawSettings::legend, legend_data)},
//           // v2
//         {"root", jsi::field(&TreeDrawSettings::_root)},
//         {"hide_isolated_before", jsi::field(&TreeDrawSettings::_hide_isolated_before)},
//         {"hide_if_cumulative_edge_length_bigger_than", jsi::field(&TreeDrawSettings::_hide_if_cumulative_edge_length_bigger_than)},
//         {"hide_if", jsi::field(&TreeDrawSettings::_hide_if)},
//     };

//     jsi::data<TimeSeriesDrawSettings> time_series_data = {
//         {"begin", jsi::field(&TimeSeriesDrawSettings::begin)},
//         {"end", jsi::field(&TimeSeriesDrawSettings::end)},
//         {"label_size", jsi::field(&TimeSeriesDrawSettings::label_size)},
//         {"label_style", jsi::field(&TimeSeriesDrawSettings::label_style, style_data)},
//         {"month_year_to_timeseries_gap", jsi::field(&TimeSeriesDrawSettings::month_year_to_timeseries_gap)},
//         {"month_separator_color", jsi::field<ColorStorer>(&TimeSeriesDrawSettings::month_separator_color)},
//         {"month_separator_width", jsi::field(&TimeSeriesDrawSettings::month_separator_width)},
//         {"dash_width", jsi::field(&TimeSeriesDrawSettings::dash_width)},
//         {"dash_line_width", jsi::field(&TimeSeriesDrawSettings::dash_line_width)},
//     };

//     jsi::data<CladeDrawSettings> clade_data = {
//         {"name", jsi::field(&CladeDrawSettings::name)},
//         {"display_name", jsi::field(&CladeDrawSettings::display_name)},
//         {"show", jsi::field(&CladeDrawSettings::show)},
//         {"section_inclusion_tolerance", jsi::field(&CladeDrawSettings::section_inclusion_tolerance)},
//         {"section_exclusion_tolerance", jsi::field(&CladeDrawSettings::section_exclusion_tolerance)},
//         {"show_section_size_in_label", jsi::field(&CladeDrawSettings::show_section_size_in_label)},
//         {"arrow_color", jsi::field<ColorStorer>(&CladeDrawSettings::arrow_color)},
//         {"line_width", jsi::field(&CladeDrawSettings::line_width)},
//         {"arrow_width", jsi::field(&CladeDrawSettings::arrow_width)},
//         {"separator_color", jsi::field<ColorStorer>(&CladeDrawSettings::separator_color)},
//         {"separator_width", jsi::field(&CladeDrawSettings::separator_width)},
//         {"label_position", jsi::field(&CladeDrawSettings::label_position)},
//         {"label_offset", jsi::field<SizeStorer>(&CladeDrawSettings::label_offset)},
//         {"label_color", jsi::field<ColorStorer>(&CladeDrawSettings::label_color)},
//         {"label_size", jsi::field(&CladeDrawSettings::label_size)},
//         {"label_style", jsi::field(&CladeDrawSettings::label_style, style_data)},
//         {"label_rotation", jsi::field(&CladeDrawSettings::label_rotation)},
//         {"slot", jsi::field(&CladeDrawSettings::slot)},
//     };

//     jsi::data<CladesDrawSettings> clades_data = {
//         {"clades", jsi::field(&CladesDrawSettings::get_clades, clade_data)},
//         {"slot_width", jsi::field(&CladesDrawSettings::slot_width)},
//     };

//     jsi::data<HzSection> hz_section_data = {
//         {"show", jsi::field(&HzSection::show)},
//         {"show_line", jsi::field(&HzSection::show_line)},
//         {"show_label_in_time_series", jsi::field(&HzSection::show_label_in_time_series)},
//         {"show_map", jsi::field(&HzSection::show_map)},
//         {"name", jsi::field(&HzSection::name)},
//         {"label", jsi::field(&HzSection::label)},
//     };

//     jsi::data<HzSections> hz_sections_data = {
//         {"vertical_gap", jsi::field(&HzSections::vertical_gap)},
//         {"line_color", jsi::field<ColorStorer>(&HzSections::line_color)},
//         {"line_width", jsi::field(&HzSections::line_width)},
//         {"ts_label_size", jsi::field(&HzSections::ts_label_size)},
//         {"ts_label_style", jsi::field(&HzSections::ts_label_style, style_data)},
//         {"ts_label_color", jsi::field<ColorStorer>(&HzSections::ts_label_color)},
//         {"show_labels_in_time_series_in_tree_mode", jsi::field(&HzSections::show_labels_in_time_series_in_tree_mode)},
//         {"sections", jsi::field(&HzSections::get_sections, hz_section_data)},
//     };

//     jsi::data<MappedAntigensDrawSettings> mapped_antigens_data = {
//         {"width", jsi::field(&MappedAntigensDrawSettings::width)},
//         {"line_width", jsi::field(&MappedAntigensDrawSettings::line_width)},
//         {"line_color", jsi::field<ColorStorer>(&MappedAntigensDrawSettings::line_color)},
//         {"line_length", jsi::field(&MappedAntigensDrawSettings::line_length)},
//     };

//     jsi::data<AntigenicMapsDrawSettings> antigenic_maps_data = {
//         {"layout", jsi::field(&AntigenicMapsDrawSettings::layout)},
//         {"columns", jsi::field(&AntigenicMapsDrawSettings::columns)},
//         {"gap", jsi::field(&AntigenicMapsDrawSettings::gap)},
//         {"mapped_antigens_section_line_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::mapped_antigens_section_line_color)},
//         {"mapped_antigens_section_line_width", jsi::field(&AntigenicMapsDrawSettings::mapped_antigens_section_line_width)},
//         {"mods", jsi::field<AntigenicMapModsStorer>(&AntigenicMapsDrawSettings::get_mods)},
//     };

//     // jsi::data<MarkAntigenSettings> mark_antigens_data = {
//     //     {"show", jsi::field(&MarkAntigenSettings::show)},
//     //     {"name", jsi::field(&MarkAntigenSettings::name)},
//     //     {"label", jsi::field(&MarkAntigenSettings::label)},
//     //     {"scale", jsi::field(&MarkAntigenSettings::scale)},
//     //     {"aspect", jsi::field(&MarkAntigenSettings::aspect)},
//     //     {"rotation", jsi::field(&MarkAntigenSettings::rotation)},
//     //     {"outline_width", jsi::field(&MarkAntigenSettings::outline_width)},
//     //     {"label_line_width", jsi::field(&MarkAntigenSettings::label_line_width)},
//     //     {"fill_color", jsi::field<ColorStorer>(&MarkAntigenSettings::fill_color)},
//     //     {"outline_color", jsi::field<ColorStorer>(&MarkAntigenSettings::outline_color)},
//     //     {"label_color", jsi::field<ColorStorer>(&MarkAntigenSettings::label_color)},
//     //     {"label_line_color", jsi::field<ColorStorer>(&MarkAntigenSettings::label_line_color)},
//     //     {"label_offset", jsi::field<SizeStorer>(&MarkAntigenSettings::label_offset)},
//     //     {"label_size", jsi::field(&MarkAntigenSettings::label_size)},
//     // };

//     // jsi::data<AntigenicMapsDrawSettings> antigenic_maps_data = {
//     //     {"layout", jsi::field(&AntigenicMapsDrawSettings::layout)},
//     //     {"width", jsi::field(&AntigenicMapsDrawSettings::_width)}, // v2
//     //     {"columns", jsi::field(&AntigenicMapsDrawSettings::columns)},
//     //     {"gap", jsi::field(&AntigenicMapsDrawSettings::gap)},
//     //     {"transformation", jsi::field(&AntigenicMapsDrawSettings::get_transformation)},
//     //     {"viewport", jsi::field<ViewportStorer>(&AntigenicMapsDrawSettings::viewport)},
//     //     {"border_width", jsi::field(&AntigenicMapsDrawSettings::border_width)},
//     //     {"grid_line_width", jsi::field(&AntigenicMapsDrawSettings::grid_line_width)},
//     //     {"border_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::border_color)},
//     //     {"grid_line_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::grid_line_color)},
//     //     {"background_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::background_color)},
//     //     {"point_scale", jsi::field(&AntigenicMapsDrawSettings::point_scale)},
//     //     {"serum_scale", jsi::field(&AntigenicMapsDrawSettings::serum_scale)},
//     //     {"reference_antigen_scale", jsi::field(&AntigenicMapsDrawSettings::reference_antigen_scale)},
//     //     {"test_antigen_scale", jsi::field(&AntigenicMapsDrawSettings::test_antigen_scale)},
//     //     {"vaccine_antigen_scale", jsi::field(&AntigenicMapsDrawSettings::vaccine_antigen_scale)},
//     //     {"tracked_antigen_scale", jsi::field(&AntigenicMapsDrawSettings::tracked_antigen_scale)},
//     //     {"serum_outline_width", jsi::field(&AntigenicMapsDrawSettings::serum_outline_width)},
//     //     {"reference_antigen_outline_width", jsi::field(&AntigenicMapsDrawSettings::reference_antigen_outline_width)},
//     //     {"test_antigen_outline_width", jsi::field(&AntigenicMapsDrawSettings::test_antigen_outline_width)},
//     //     {"vaccine_antigen_outline_width", jsi::field(&AntigenicMapsDrawSettings::vaccine_antigen_outline_width)},
//     //     {"sequenced_antigen_outline_width", jsi::field(&AntigenicMapsDrawSettings::sequenced_antigen_outline_width)},
//     //     {"tracked_antigen_outline_width", jsi::field(&AntigenicMapsDrawSettings::tracked_antigen_outline_width)},
//     //     {"serum_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::serum_outline_color)},
//     //     {"reference_antigen_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::reference_antigen_outline_color)},
//     //     {"test_antigen_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::test_antigen_outline_color)},
//     //     {"test_antigen_fill_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::test_antigen_fill_color)},
//     //     {"vaccine_antigen_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::vaccine_antigen_outline_color)},
//     //     {"sequenced_antigen_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::sequenced_antigen_outline_color)},
//     //     {"sequenced_antigen_fill_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::sequenced_antigen_fill_color)},
//     //     {"tracked_antigen_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::tracked_antigen_outline_color)},
//     //     {"tracked_antigen_colored_by_clade", jsi::field(&AntigenicMapsDrawSettings::tracked_antigen_colored_by_clade)},
//     //     {"tracked_antigen_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::tracked_antigen_color)},
//     //     {"reassortant_rotation", jsi::field(&AntigenicMapsDrawSettings::reassortant_rotation)},
//     //     {"egg_antigen_aspect", jsi::field(&AntigenicMapsDrawSettings::egg_antigen_aspect)},
//     //     {"show_tracked_sera", jsi::field(&AntigenicMapsDrawSettings::show_tracked_sera)},
//     //     {"serum_circle_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::serum_circle_color)},
//     //     {"tracked_serum_outline_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::tracked_serum_outline_color)},
//     //     {"serum_circle_thickness", jsi::field(&AntigenicMapsDrawSettings::serum_circle_thickness)},
//     //     {"tracked_serum_outline_width", jsi::field(&AntigenicMapsDrawSettings::tracked_serum_outline_width)},
//     //     {"map_title_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::map_title_color)},
//     //     {"map_title_offset", jsi::field<SizeStorer>(&AntigenicMapsDrawSettings::map_title_offset)},
//     //     {"map_title_size", jsi::field(&AntigenicMapsDrawSettings::map_title_size)},
//     //     {"mapped_antigens_section_line_color", jsi::field<ColorStorer>(&AntigenicMapsDrawSettings::mapped_antigens_section_line_color)},
//     //     {"mapped_antigens_section_line_width", jsi::field(&AntigenicMapsDrawSettings::mapped_antigens_section_line_width)},
//     //     {"mark_antigens", jsi::field(&AntigenicMapsDrawSettings::get_mark_antigens, mark_antigens_data)},
//     // };

//     jsi::data<TitleDrawSettings> title_data = {
//         {"title", jsi::field(&TitleDrawSettings::title)},
//         {"color", jsi::field<ColorStorer>(&TitleDrawSettings::color)},
//         {"size", jsi::field(&TitleDrawSettings::size)},
//         {"style", jsi::field(&TitleDrawSettings::style, style_data)},
//         {"offset", jsi::field<SizeStorer>(&TitleDrawSettings::offset)},
//     };


//     jsi::data<Settings> settings_data = {
//         {"_", jsi::field(&Settings::indentation)},
//         {"  version", jsi::field(&Settings::set_version)},
//         {"signature_page", jsi::field(&Settings::signature_page, signature_page_data)},
//         {"tree", jsi::field(&Settings::tree_draw, tree_draw_data)},
//         {"time_series", jsi::field(&Settings::time_series, time_series_data)},
//         {"clades", jsi::field(&Settings::clades, clades_data)},
//         {"hz_sections", jsi::field(&Settings::hz_sections, hz_sections_data)},
//         {"mapped_antigens", jsi::field(&Settings::mapped_antigens, mapped_antigens_data)},
//         {"antigenic_maps", jsi::field(&Settings::antigenic_maps, antigenic_maps_data)},
//         {"title", jsi::field(&Settings::title, title_data)},
//     };

//     std::cout << "Reading settings from " << aFilename << std::endl;
//     jsi::import(acmacs_base::read_file(aFilename), aSettings, settings_data);
//     aSettings.upgrade();

// } // read_settings

// **********************************************************************

template <typename RW, typename FValue> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const rjson::field_get_set<FValue>& aValue)
{
    return writer << static_cast<FValue>(aValue);
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TextStyle& aStyle)
{
    return writer << jsw::start_object
                  << jsw::key("family") << aStyle.font_family()
                  << jsw::key("slant") << aStyle.slant_as_stirng()
                  << jsw::key("weight") << aStyle.weight_as_stirng()
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Size& aSize)
{
    return writer << jsw::start_array << aSize.width << aSize.height << jsw::end_array;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, Color aColor)
{
    return writer << aColor.to_string();
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const AATransitionPerBranchDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("size") << aSettings.size
                  << jsw::key("color") << aSettings.color
                  << jsw::key("style") << aSettings.style
                  << jsw::key("interline") << aSettings.interline
                  << jsw::key("label_offset") << aSettings.label_offset
                  << jsw::key("label_connection_line_width") << aSettings.label_connection_line_width
                  << jsw::key("label_connection_line_color") << aSettings.label_connection_line_color
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const AATransitionDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("show") << aSettings.show
                  << jsw::key("number_strains_threshold") << aSettings.number_strains_threshold
                  << jsw::key("number_strains_threshold?") << aSettings.number_strains_threshold_help // "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"
                  << jsw::key("show_empty_left") << aSettings.show_empty_left
                  << jsw::key("show_node_for_left_line") << aSettings.show_node_for_left_line
                  << jsw::key("node_for_left_line_color") << aSettings.node_for_left_line_color
                  << jsw::key("node_for_left_line_width") << aSettings.node_for_left_line_width
                  << jsw::key("per_branch") << aSettings.per_branch
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawVaccineSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("name") << aSettings.name
                  << jsw::key("name?") << aSettings.name_help // "empty for default settings"
                  << jsw::key("label_color") << aSettings.label_color
                  << jsw::key("label_size") << aSettings.label_size
                  << jsw::key("label_style") << aSettings.label_style
                  << jsw::key("line_color") << aSettings.line_color
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const LegendSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("offset") << aSettings.offset
                  << jsw::key("width") << aSettings.width
                  << jsw::key("title_style") << aSettings.title_style
                  << jsw::key("title_size") << aSettings.title_size
                  << jsw::key("text_style") << aSettings.text_style
                  << jsw::key("text_size") << aSettings.text_size
                  << jsw::key("interline") << aSettings.interline
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawMod& aMod)
{
    return writer << jsw::start_object
                  << jsw::key("mod") << aMod.mod
                  << jsw::if_non_negative("d1", aMod.d1)
                  << jsw::if_not_empty("s1", aMod.s1)
                  << jsw::if_not_empty("s2", aMod.s2)
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

inline std::string ladderize_to_string(const TreeDrawSettings& aSettings)
{
    return aSettings.ladderize.get_value_ref();
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawMods& aMods)
{
    return jsw::write_list(writer, aMods);
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawVaccines& aMods)
{
    return jsw::write_list(writer, aMods);
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawSettings& aSettings)
{
    return writer << jsw::start_object

                  << jsw::key("ladderize?") << aSettings.ladderize_help // "number-of-leaves, max-edge-length"
                  << jsw::key("ladderize") << ladderize_to_string(aSettings)
                  // << jsw::key("mods 1?") << "mods is a list of objects:"
                  // << jsw::key("mods 2?") << "{mod: root, s1: new-root}"
                  // << jsw::key("mods 3?") << "{mod: hide-isolated-before, s1: date}"
                  // << jsw::key("mods 4?") << "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}"
                  // << jsw::key("mods 5?") << "{mod: before2015-58P-or-146I-or-559I}"
                  // << jsw::key("mods 6?") << "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing"
                  // << jsw::key("mods 7?") << "{mod: hide-one, s1: name-to-hide} - after ladderizing"
                  // << jsw::key("mods 8?") << "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}"
                  << jsw::key("mods") << aSettings.mods
                  << jsw::key("mods?") << aSettings.mods_help
              // v2 << jsw::key("root") << aSettings.root
              // v2 << jsw::key("hide_isolated_before") << aSettings.hide_isolated_before
              // v2 << jsw::key("hide_if_cumulative_edge_length_bigger_than") << aSettings.hide_if_cumulative_edge_length_bigger_than
              // v2 << jsw::key("hide_if") << aSettings.hide_if

                  << jsw::key("force_line_width") << aSettings.force_line_width
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::key("root_edge") << aSettings.root_edge
                  << jsw::key("line_color") << aSettings.line_color
                  << jsw::key("label_style") << aSettings.label_style
                  << jsw::key("name_offset") << aSettings.name_offset
                  << jsw::key("color_nodes") << aSettings.color_nodes
                  << jsw::key("color_nodes?") << "black, continent, position number (e.g. 162)"
                  << jsw::key("aa_transition") << aSettings.aa_transition
                  << jsw::key("vaccines") << aSettings.vaccines
                  << jsw::key("legend") << aSettings.legend
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const SignaturePageDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("layout") << aSettings.layout
                  << jsw::key("top") << aSettings.top
                  << jsw::key("bottom") << aSettings.bottom
                  << jsw::key("left") << aSettings.left
                  << jsw::key("right") << aSettings.right
                  << jsw::key("tree_margin_right") << aSettings.tree_margin_right
                  << jsw::key("mapped_antigens_margin_right") << aSettings.mapped_antigens_margin_right
                  << jsw::key("time_series_width") << aSettings.time_series_width
                  << jsw::key("clades_width") << aSettings.clades_width
                  << jsw::key("antigenic_maps_width") << aSettings.antigenic_maps_width
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TimeSeriesDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("begin") << aSettings.begin
                  << jsw::key("end") << aSettings.end
                  << jsw::key("label_size") << aSettings.label_size
                  << jsw::key("label_style") << aSettings.label_style
                  << jsw::key("month_year_to_timeseries_gap") << aSettings.month_year_to_timeseries_gap
                  << jsw::key("month_separator_color") << aSettings.month_separator_color
                  << jsw::key("month_separator_width") << aSettings.month_separator_width
                  << jsw::key("dash_width") << aSettings.dash_width
                  << jsw::key("dash_line_width") << aSettings.dash_line_width
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const CladeDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("name") << aSettings.name
                  << jsw::key("display_name") << aSettings.display_name
                  << jsw::key("show") << aSettings.show
                  << jsw::key("slot") << static_cast<int>(aSettings.slot)
                  << jsw::key("section_inclusion_tolerance") << aSettings.section_inclusion_tolerance
                  << jsw::key("section_exclusion_tolerance") << aSettings.section_exclusion_tolerance
                  << jsw::key("show_section_size_in_label") << aSettings.show_section_size_in_label
                  << jsw::key("arrow_color") << aSettings.arrow_color
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::key("arrow_width") << aSettings.arrow_width
                  << jsw::key("separator_color") << aSettings.separator_color
                  << jsw::key("separator_width") << aSettings.separator_width
                  << jsw::key("label_position") << aSettings.label_position
                  << jsw::key("label_offset") << aSettings.label_offset
                  << jsw::key("label_color") << aSettings.label_color
                  << jsw::key("label_size") << aSettings.label_size
                  << jsw::key("label_style") << aSettings.label_style
                  << jsw::key("label_rotation") << aSettings.label_rotation
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const rjson::array_field_container_child<CladeDrawSettings>& clades)
{
    return jsw::write_list(writer, clades);
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const CladesDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("slot_width") << aSettings.slot_width
                  << jsw::key("clades") << aSettings.clades
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const HzSection& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("label") << aSettings.label
                  << jsw::key("name") << aSettings.name
                  << jsw::key("show") << aSettings.show
                  << jsw::key("show_label_in_time_series") << aSettings.show_label_in_time_series
                  << jsw::key("show_line") << aSettings.show_line
                  << jsw::key("show_map") << aSettings.show_map
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const rjson::array_field_container_child<HzSection>& clades)
{
    return jsw::write_list(writer, clades);
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const HzSections& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("vertical_gap") << aSettings.vertical_gap
                  << jsw::key("line_color") << aSettings.line_color
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::key("ts_label_size") << aSettings.ts_label_size
                  << jsw::key("ts_label_style") << aSettings.ts_label_style
                  << jsw::key("ts_label_color") << aSettings.ts_label_color
                  << jsw::key("show_labels_in_time_series_in_tree_mode") << aSettings.show_labels_in_time_series_in_tree_mode
                  << jsw::key("sections") << aSettings.sections
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const MappedAntigensDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("width") << aSettings.width
                  << jsw::key("line_color") << aSettings.line_color
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::key("line_length") << aSettings.line_length
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Viewport& aViewport)
{
    writer << jsw::start_array << aViewport.origin.x << aViewport.origin.y << aViewport.size.width;
    if (!float_equal(aViewport.size.width, aViewport.size.height))
        writer << aViewport.size.height;
    writer << jsw::end_array;
    return writer;
}

// ----------------------------------------------------------------------

// template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const MarkAntigenSettings& aSettings)
// {
//     return writer << jsw::start_object
//                   << jsw::key("show") << aSettings.show
//                   << jsw::key("name") << aSettings.name
//                   << jsw::key("scale") << aSettings.scale
//                   << jsw::key("aspect") << aSettings.aspect
//                   << jsw::key("rotation") << aSettings.rotation
//                   << jsw::key("fill_color") << aSettings.fill_color
//                   << jsw::key("outline_color") << aSettings.outline_color
//                   << jsw::key("outline_width") << aSettings.outline_width
//                   << jsw::key("label") << aSettings.label
//                   << jsw::key("label_color") << aSettings.label_color
//                   << jsw::key("label_offset") << aSettings.label_offset
//                   << jsw::key("label_size") << aSettings.label_size
//                   << jsw::key("label_line_color") << aSettings.label_line_color
//                   << jsw::key("label_line_width") << aSettings.label_line_width
//                   << jsw::end_object;
// }

// // ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const SettingValue& aSettingValue)
{
    // std::cerr << "DEBUG: SettingValue " << aSettingValue.which() << " " << aSettingValue << std::endl;
    switch (aSettingValue.which()) {
      case 0:
          writer << *boost::get<std::string>(&aSettingValue);
          break;
      case 1:
          writer << std::round(*boost::get<double>(&aSettingValue) * 100.0) / 100.0;
          break;
      case 2:
          writer << *boost::get<int>(&aSettingValue);
          break;
      case 3:
          writer << *boost::get<bool>(&aSettingValue);
          break;
      case 4:
          writer << *boost::get<SettingDict>(&aSettingValue);
          break;
      case 5:
          writer << *boost::get<SettingList>(&aSettingValue);
          break;
    }
    return writer;
}

// template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const SettingList& aSettings)
// {
//     writer << jsw::start_array;
//     for (const auto& value: aSettings)
//         writer << value;
//     writer << jsw::end_array;
//     return writer;
// }

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const SettingDict& aSettings)
{
    writer << jsw::start_object;
    for (const auto& value: aSettings)
        writer << jsw::key(value.first) << value.second;
    writer << jsw::end_object;
    return writer;
}

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const AntigenicMapsDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("layout") << aSettings.layout
                  << jsw::key("columns") << aSettings.columns
                  << jsw::key("gap") << aSettings.gap
                  << jsw::key("mapped_antigens_section_line_color") << aSettings.mapped_antigens_section_line_color
                  << jsw::key("mapped_antigens_section_line_width") << aSettings.mapped_antigens_section_line_width
                  << jsw::key("mods") << aSettings.mods
                  << jsw::end_object;
}

// template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const AntigenicMapsDrawSettings& aSettings)
// {
//     return writer << jsw::start_object
//                   << jsw::key("layout") << aSettings.layout
//               // v2 << jsw::key("width") << aSettings.width
//                   << jsw::key("columns") << aSettings.columns
//                   << jsw::key("gap") << aSettings.gap
//                   << jsw::key("transformation") << aSettings.transformation
//                   << jsw::key("viewport") << aSettings.viewport
//                   << jsw::key("show_tracked_sera") << aSettings.show_tracked_sera
//                   << jsw::key("background_color") << aSettings.background_color
//                   << jsw::key("border_color") << aSettings.border_color
//                   << jsw::key("border_width") << aSettings.border_width
//                   << jsw::key("grid_line_width") << aSettings.grid_line_width
//                   << jsw::key("grid_line_color") << aSettings.grid_line_color
//                   << jsw::key("point_scale") << aSettings.point_scale
//                   << jsw::key("serum_scale") << aSettings.serum_scale
//                   << jsw::key("reference_antigen_scale") << aSettings.reference_antigen_scale
//                   << jsw::key("test_antigen_scale") << aSettings.test_antigen_scale
//                   << jsw::key("vaccine_antigen_scale") << aSettings.vaccine_antigen_scale
//                   << jsw::key("tracked_antigen_scale") << aSettings.tracked_antigen_scale
//                   << jsw::key("serum_outline_width") << aSettings.serum_outline_width
//                   << jsw::key("reference_antigen_outline_width") << aSettings.reference_antigen_outline_width
//                   << jsw::key("test_antigen_outline_width") << aSettings.test_antigen_outline_width
//                   << jsw::key("vaccine_antigen_outline_width") << aSettings.vaccine_antigen_outline_width
//                   << jsw::key("sequenced_antigen_outline_width") << aSettings.sequenced_antigen_outline_width
//                   << jsw::key("serum_outline_color") << aSettings.serum_outline_color
//                   << jsw::key("reference_antigen_outline_color") << aSettings.reference_antigen_outline_color
//                   << jsw::key("test_antigen_outline_color") << aSettings.test_antigen_outline_color
//                   << jsw::key("test_antigen_fill_color") << aSettings.test_antigen_fill_color
//                   << jsw::key("vaccine_antigen_outline_color") << aSettings.vaccine_antigen_outline_color
//                   << jsw::key("sequenced_antigen_outline_color") << aSettings.sequenced_antigen_outline_color
//                   << jsw::key("sequenced_antigen_fill_color") << aSettings.sequenced_antigen_fill_color
//                   << jsw::key("tracked_antigen_outline_width") << aSettings.tracked_antigen_outline_width
//                   << jsw::key("tracked_antigen_outline_color") << aSettings.tracked_antigen_outline_color
//                   << jsw::key("tracked_antigen_colored_by_clade") << aSettings.tracked_antigen_colored_by_clade
//                   << jsw::key("tracked_antigen_color") << aSettings.tracked_antigen_color
//                   << jsw::key("reassortant_rotation") << aSettings.reassortant_rotation
//                   << jsw::key("egg_antigen_aspect") << aSettings.egg_antigen_aspect
//                   << jsw::key("serum_circle_color") << aSettings.serum_circle_color
//                   << jsw::key("tracked_serum_outline_color") << aSettings.tracked_serum_outline_color
//                   << jsw::key("serum_circle_thickness") << aSettings.serum_circle_thickness
//                   << jsw::key("tracked_serum_outline_width") << aSettings.tracked_serum_outline_width
//                   << jsw::key("map_title_color") << aSettings.map_title_color
//                   << jsw::key("map_title_offset") << aSettings.map_title_offset
//                   << jsw::key("map_title_size") << aSettings.map_title_size
//                   << jsw::key("mapped_antigens_section_line_color") << aSettings.mapped_antigens_section_line_color
//                   << jsw::key("mapped_antigens_section_line_width") << aSettings.mapped_antigens_section_line_width
//                   << jsw::key("mark_antigens") << aSettings.mark_antigens
//                   << jsw::end_object;
// }

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TitleDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("color") << aSettings.color
                  << jsw::key("offset") << aSettings.offset
                  << jsw::key("size") << aSettings.size
                  << jsw::key("style") << aSettings.style
                  << jsw::key("title") << aSettings.title
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Settings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("  version") << SETTINGS_VERSION_4
                  << jsw::key("signature_page") << aSettings.signature_page
                  << jsw::key("title") << aSettings.title
                  << jsw::key("tree") << aSettings.tree_draw
                  << jsw::key("time_series") << aSettings.time_series
                  << jsw::key("clades") << aSettings.clades
                  << jsw::key("hz_sections") << aSettings.hz_sections
                  << jsw::key("mapped_antigens") << aSettings.mapped_antigens
                  << jsw::key("antigenic_maps") << aSettings.antigenic_maps
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent)
{
    std::cerr << "INFO: writing settings to " << aFilename << std::endl;
    jsw::export_to_json(aSettings, aFilename, aIndent);

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
