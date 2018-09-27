#include <cmath>

#include "acmacs-base/debug.hh"
#include "acmacs-base/float.hh"
#include "acmacs-base/read-file.hh"

#include "settings.hh"
using namespace std::string_literals;

// ----------------------------------------------------------------------

static constexpr const char* SETTINGS_VERSION_4 = "signature-page-settings-v4";

// ----------------------------------------------------------------------

AATransitionIndividualSettings::AATransitionIndividualSettings(const rjson::v1::value& aData)
    : rjson::v1::array_field_container_child_element(aData),
      label(*this, "label", "", rjson::v1::initialize_field::yes),
      first_leaf_seq_id(*this, "first_leaf_seq_id", ""),
      show(*this, "show", true),
      size(*this, "size", 8),
      color(*this, "color", "black"),
      style(*this, "style", {"Courier New"}),
      interline(*this, "interline", 1.2),
      label_offset(*this, "label_offset", {-40, 20}),
      label_connection_line_width(*this, "label_connection_line_width", 0.1),
      label_connection_line_color(*this, "label_connection_line_color", "black"),
      _no_pp(*this, rjson::v1::object::no_pp_key, true, rjson::v1::initialize_field::yes)
{
}

AATransitionPerBranchDrawSettings::AATransitionPerBranchDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      show(*this, "show", true, rjson::v1::initialize_field::yes),
      size(*this, "size", 8, rjson::v1::initialize_field::yes),
      color(*this, "color", "black", rjson::v1::initialize_field::yes),
      style(*this, "style", {"Courier New"}, rjson::v1::initialize_field::yes),
      interline(*this, "interline", 1.2, rjson::v1::initialize_field::yes),
      label_offset(*this, "label_offset", {-40, 20}, rjson::v1::initialize_field::yes),
      scatter_label_offset(*this, "scatter_label_offset", 0, rjson::v1::initialize_field::yes),
      scatter_label_offset_help(*this, "scatter_label_offset?", "randomization range for label offset in trying to avoid clattering (mostly for figuring out good offsets", rjson::v1::initialize_field::yes),
      label_connection_line_width(*this, "label_connection_line_width", 0.1, rjson::v1::initialize_field::yes),
      label_connection_line_color(*this, "label_connection_line_color", "black", rjson::v1::initialize_field::yes),
      by_aa_label(*this, "by_aa_label")
{
}

void AATransitionPerBranchDrawSettings::remove_for_signature_page_settings()
{
    for (const auto name: {"show", "size", "color", "style", "interline", "label_offset", "scatter_label_offset", "scatter_label_offset?", "label_connection_line_width", "label_connection_line_color"})
        remove_child(name);
}

AATransitionDrawSettings::AATransitionDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      show(*this, "show", true, rjson::v1::initialize_field::yes),
      number_strains_threshold(*this, "number_strains_threshold", 20, rjson::v1::initialize_field::yes),
      number_strains_threshold_help(*this, "number_strains_threshold?", "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value", rjson::v1::initialize_field::yes),
      show_empty_left(*this, "show_empty_left", false, rjson::v1::initialize_field::yes),
      per_branch(*this, "per_branch"),
      show_node_for_left_line(*this, "show_node_for_left_line", false, rjson::v1::initialize_field::yes),
      node_for_left_line_color(*this, "node_for_left_line_color", "green", rjson::v1::initialize_field::yes),
      node_for_left_line_width(*this, "node_for_left_line_width", 1, rjson::v1::initialize_field::yes)
{
}

void AATransitionDrawSettings::remove_for_signature_page_settings()
{
    for (const auto name: {"show", "number_strains_threshold", "number_strains_threshold?", "show_empty_left", "show_node_for_left_line", "node_for_left_line_color", "node_for_left_line_width"})
        remove_child(name);
    per_branch.remove_for_signature_page_settings();
}

TreeDrawVaccineSettings::TreeDrawVaccineSettings(const rjson::v1::value& aData)
    : rjson::v1::array_field_container_child_element(aData),
      name(*this, "name", "", rjson::v1::initialize_field::yes),
      name_help(*this, "name?", "empty for default settings", rjson::v1::initialize_field::yes),
      label_color(*this, "label_color", "black", rjson::v1::initialize_field::yes),
      label_size(*this, "label_size", 10, rjson::v1::initialize_field::yes),
      label_style(*this, "label_style", {}, rjson::v1::initialize_field::yes),
      line_color(*this, "line_color", "black", rjson::v1::initialize_field::yes),
      line_width(*this, "line_width", 1, rjson::v1::initialize_field::yes)
{
}

LegendSettings::LegendSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      offset(*this, "offset", {-30, 950}, rjson::v1::initialize_field::yes),
      width(*this, "width", 100, rjson::v1::initialize_field::yes),
      title_style(*this, "title_style", {"sans_serif"}, rjson::v1::initialize_field::yes),
      title_size(*this, "title_size", 10, rjson::v1::initialize_field::yes),
      text_style(*this, "text_style", {"monospace"}, rjson::v1::initialize_field::yes),
      text_size(*this, "text_size", 10, rjson::v1::initialize_field::yes),
      interline(*this, "interline", 1.5, rjson::v1::initialize_field::yes)
{
}

TimeSeriesDrawSettings::TimeSeriesDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      begin(*this, "begin", ""),
      end(*this, "end", ""),
      label_size(*this, "label_size", 8, rjson::v1::initialize_field::yes),
      label_style(*this, "label_style", {}, rjson::v1::initialize_field::yes),
      month_year_to_timeseries_gap(*this, "month_year_to_timeseries_gap", 2, rjson::v1::initialize_field::yes),
      month_separator_color(*this, "month_separator_color", "black", rjson::v1::initialize_field::yes),
      month_separator_width(*this, "month_separator_width", 0.5, rjson::v1::initialize_field::yes),
      dash_width(*this, "dash_width", 0.5, rjson::v1::initialize_field::yes),
      dash_line_width(*this, "dash_line_width", 1, rjson::v1::initialize_field::yes)
{
}

CladeDrawSettings::CladeDrawSettings(const rjson::v1::value& aData, std::string aName, bool aShow)
    : rjson::v1::array_field_container_child_element(aData),
      name(*this, "name", aName, rjson::v1::initialize_field::yes),
      display_name(*this, "display_name", "", rjson::v1::initialize_field::yes),
      show(*this, "show", aShow, rjson::v1::initialize_field::yes),
      section_inclusion_tolerance(*this, "section_inclusion_tolerance", 10, rjson::v1::initialize_field::yes),
      section_exclusion_tolerance(*this, "section_exclusion_tolerance", 5, rjson::v1::initialize_field::yes),
      show_section_size_in_label(*this, "show_section_size_in_label", true, rjson::v1::initialize_field::yes),
      arrow_color(*this, "arrow_color", "black", rjson::v1::initialize_field::yes),
      line_width(*this, "line_width", 0.8, rjson::v1::initialize_field::yes),
      arrow_width(*this, "arrow_width", 3, rjson::v1::initialize_field::yes),
      separator_color(*this, "separator_color", "grey63", rjson::v1::initialize_field::yes),
      separator_width(*this, "separator_width", 0.5, rjson::v1::initialize_field::yes),
      label_position(*this, "label_position", "middle", rjson::v1::initialize_field::yes),
      label_offset(*this, "label_offset", {5, 0}, rjson::v1::initialize_field::yes),
      label_color(*this, "label_color", "black", rjson::v1::initialize_field::yes),
      label_size(*this, "label_size", 11, rjson::v1::initialize_field::yes),
      label_style(*this, "label_style", {}, rjson::v1::initialize_field::yes),
      label_rotation(*this, "label_rotation", 0, rjson::v1::initialize_field::yes),
      slot(*this, "slot", NoSlot, rjson::v1::initialize_field::yes)
{
}

CladesDrawSettings::CladesDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      clades(*this, "clades"),
      slot_width(*this, "slot_width", 10, rjson::v1::initialize_field::yes)
{
    clades.emplace_back();
    // for (const auto* n: {"GLY", "NO-GLY", "DEL2017"}) {
    //     auto cn = clades.emplace_back();
    //     cn.name = n;
    //     cn.show = false;
    // }
    // // for (const auto* n: {"3C2A1A", "3C2A1B", "3C2A1", "3C2A2", "3C2A3", "3C2A4"}) {
    // //     auto cn = clades.emplace_back();
    // //     cn.name = n;
    // //     cn.section_inclusion_tolerance = 15;
    // //     cn.section_exclusion_tolerance = 10;
    // //     cn.slot = static_cast<int>(cn.name.get_value_ref().str().size() - 4);
    // // }
    // for (auto [name, display, slot] : {}) { // Sarah's H3 clades 2018-08-09
    //     auto cn = clades.emplace_back();
    //     cn.name = name;
    //     cn.display_name = display;
    //     cn.section_inclusion_tolerance = 15;
    //     cn.section_exclusion_tolerance = 10;
    //     cn.slot = slot;
    // }
}

HzSection::HzSection(const rjson::v1::value& aData)
    : rjson::v1::array_field_container_child_element(aData),
      show(*this, "show", true, rjson::v1::initialize_field::yes),
      show_line(*this, "show_line", true, rjson::v1::initialize_field::yes),
      show_label_in_time_series(*this, "show_label_in_time_series", true, rjson::v1::initialize_field::yes),
      show_map(*this, "show_map", true, rjson::v1::initialize_field::yes),
      name(*this, "name", "", rjson::v1::initialize_field::yes),
      label(*this, "label", "", rjson::v1::initialize_field::yes),
      triggering_clades(*this, "triggering_clades"),
      triggering_aa_pos(*this, "triggering_aa_pos")
      // first(nullptr),
      // last(nullptr),
      // index("?")
{
}

HzSections::HzSections(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      vertical_gap(*this, "vertical_gap", 20, rjson::v1::initialize_field::yes),
      line_color(*this, "line_color", "grey63", rjson::v1::initialize_field::yes),
      line_width(*this, "line_width", 1, rjson::v1::initialize_field::yes),
      ts_label_size(*this, "ts_label_size", 10, rjson::v1::initialize_field::yes),
      ts_label_style(*this, "ts_label_style", {}, rjson::v1::initialize_field::yes),
      ts_label_color(*this, "ts_label_color", "black", rjson::v1::initialize_field::yes),
      sections(*this, "sections"),
      show_labels_in_time_series_in_tree_mode(*this, "show_labels_in_time_series_in_tree_mode", false, rjson::v1::initialize_field::yes)
{
}

MappedAntigensDrawSettings::MappedAntigensDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      width(*this, "width", 10, rjson::v1::initialize_field::yes),
      line_width(*this, "line_width", 0.5, rjson::v1::initialize_field::yes),
      line_color(*this, "line_color", "grey56", rjson::v1::initialize_field::yes),
      line_length(*this, "line_length", 0.5, rjson::v1::initialize_field::yes)
{
}

AAAtPosSection::AAAtPosSection(const rjson::v1::value& aData)
    : rjson::v1::array_field_container_child_element(aData),
      pos(*this, "_pos", 0),
      aa(*this, "aa", ""),
      num(*this, "num", 0),
      first(*this, "first", ""),
      last(*this, "last", "")
{
} // AAAtPosSection::AAAtPosSection

AAAtPosDrawSettings::AAAtPosDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      width(*this, "width", 0, rjson::v1::initialize_field::yes),
      right_margin(*this, "right_margin", 0, rjson::v1::initialize_field::yes),
      line_width(*this, "line_width", 0.15, rjson::v1::initialize_field::yes),
      line_length(*this, "line_length", 0.5, rjson::v1::initialize_field::yes),
      diverse_index_threshold(*this, "diverse_index_threshold", 3, rjson::v1::initialize_field::yes),
      positions(*this, "positions"),
      report_most_diverse_positions(*this, "report_most_diverse_positions", false, rjson::v1::initialize_field::yes),
      small_section_threshold(*this, "small_section_threshold", 3, rjson::v1::initialize_field::yes),
      sections(*this, "?sections")
{
} // AAAtPosDrawSettings::AAAtPosDrawSettings

// ----------------------------------------------------------------------

AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      layout(*this, "layout", "labelled_grid", rjson::v1::initialize_field::yes),
      columns(*this, "columns", 3, rjson::v1::initialize_field::yes),
      gap(*this, "gap", 20, rjson::v1::initialize_field::yes),
      mapped_antigens_section_line_color(*this, "mapped_antigens_section_line_color", "black", rjson::v1::initialize_field::yes),
      mapped_antigens_section_line_width(*this, "mapped_antigens_section_line_width", 1, rjson::v1::initialize_field::yes),
      mods(*this, "mods")
{
    using namespace rjson::v1::literals;

    mods.emplace_back().add("N", "viewport").add("rel", rjson::v1::array{0, 0, 0});
    mods.emplace_back().add("N", "point_scale").add("scale", 1.0).add("outline_scale", 1.0);
    mods.emplace_back().add("N", "rotate").add("degrees", 0.0);
    mods.emplace_back().add("N", "sera").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "reference_antigens").add("fill", "transparent").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "test_antigens").add("fill", "grey88").add("outline", "grey88").add("outline_width", 0.5).add("size", 3.0);
    mods.emplace_back().add("N", "sequenced_antigens").add("fill", "grey63").add("outline", "white").add("outline_width", 0.5).add("size", 3.0);
    mods.emplace_back().add("N", "tracked_antigens").add("fill", "green3").add("outline", "white").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "?tracked_sera").add("fill", "transparent").add("outline", "black").add("outline_width", 0.5).add("size", 5.0); // tracked sera above tracked antigens!
    mods.emplace_back().add("N", "?tracked_serum_circles").add("outline", "black").add("outline_width", 0.5); // tracked serum circles above tracked antigens!
    mods.emplace_back().add("N", "?serum_circle").add("map", "A").add("serum", "A(H3N2)/SOUTH CAROLINA/4/2017 CDC 2017-106").add("outline", "black").add("outline_width", 0.5);
    mods.emplace_back().add("N", "title").add("text_color", "black").add("text_size", 12.0).add("padding", 3.0)
            .add("offset", rjson::v1::array{0.0, 0.0}).add("weight", "normal").add("slant", "normal").add("font_family", "san serif");
    mods.emplace_back().add("N", "background").add("color", "white");
    mods.emplace_back().add("N", "grid").add("color", "grey80").add("line_width", 1.0);
    mods.emplace_back().add("N", "border").add("color", "black").add("line_width", 1.0);

      // vaccine spec via acmacs-map-draw/ModAntigens, since 2018-01-19
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "previous"_rj}, {"passage", "cell"_rj}}}})
            .add("fill", "blue"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "previous"_rj}, {"passage", "egg"_rj}}}})
            .add("fill", "blue"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("?N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "previous"_rj}, {"passage", "reassortant"_rj}}}})
            .add("fill", "blue"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});

    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "current"_rj}, {"passage", "cell"_rj}}}})
            .add("fill", "red"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "current"_rj}, {"passage", "egg"_rj}}}})
            .add("fill", "red"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "current"_rj}, {"passage", "reassortant"_rj}}}})
            .add("fill", "green"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});

    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "surrogate"_rj}, {"passage", "cell"_rj}}}})
            .add("fill", "pink"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "surrogate"_rj}, {"passage", "egg"_rj}}}})
            .add("fill", "pink"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::v1::object{{"vaccine", rjson::v1::object{{"type", "surrogate"_rj}, {"passage", "reassortant"_rj}}}})
            .add("fill", "pink"_rj).add("outline", "white"_rj).add("size", 15.0_rj).add("order", "raise"_rj).add("report", false)
            .add("label", rjson::v1::object{{"offset", rjson::v1::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"_rj}, {"size", 9.0_rj}});
}

void AntigenicMapsDrawSettings::viewport(const acmacs::Viewport& aViewport)
{
    using namespace rjson::v1::literals;
    auto make_setting_list = [&aViewport]() -> rjson::v1::array {
        if (float_equal(aViewport.size.width, aViewport.size.height))
            return {rjson::v1::number{aViewport.origin.x()}, rjson::v1::number{aViewport.origin.y()}, rjson::v1::number{aViewport.size.width}};
        else
            return {rjson::v1::number{aViewport.origin.x()}, rjson::v1::number{aViewport.origin.y()}, rjson::v1::number{aViewport.size.width}, rjson::v1::number{aViewport.size.height}};
    };
      // std::cerr << "DEBUG: AntigenicMapsDrawSettings::viewport" << std::endl;
    auto vpmod = std::find_if(mods.begin(), mods.end(), [](const auto& mod) -> bool { return mod.name() == "viewport"; });
    if (vpmod == mods.end()) {
        auto mod = mods.emplace_back();
        mod.set_field("N", "viewport"_rj);
        mod.set_field("viewport", make_setting_list());
    }
    else
        (*vpmod).set_field("viewport", make_setting_list());

} // AntigenicMapsDrawSettings::viewport

acmacs::Viewport AntigenicMapMod::get_viewport(const acmacs::Viewport& aOrigViewport) const
{
    acmacs::Viewport result = aOrigViewport;

    try {
        const rjson::v1::array& ar = operator[]("rel");
        try {
            switch (ar.size()) {
              case 3:
                  result.set({aOrigViewport.left() + static_cast<double>(ar[0]), aOrigViewport.top() + static_cast<double>(ar[1])}, aOrigViewport.size.width + static_cast<double>(ar[2]));
                  break;
              default:
                  throw std::exception{};
            }
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to array (viewport rel): " << ar << '\n';
            throw;
        }
    }
    catch (rjson::v1::field_not_found&) {
    }

    try {
        const rjson::v1::array& ar = operator[]("viewport");
        try {
            switch (ar.size()) {
              case 3:
                  result.set({ar[0], ar[1]}, ar[2]);
                  break;
              case 4:
                  result.set(acmacs::Location2D{ar[0], ar[1]}, acmacs::Size{ar[2], ar[3]});
                  break;
              default:
                  throw std::exception{};
            }
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to array (viewport): " << ar << '\n';
            throw;
        }
    }
    catch (rjson::v1::field_not_found&) {
    }

    return result;

} // AntigenicMapMod::get_viewport

// ----------------------------------------------------------------------

TreeDrawSettings::TreeDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      ladderize(*this, "ladderize", Tree::LadderizeMethod::NumberOfLeaves),
      ladderize_help(*this, "ladderize?", "number-of-leaves or max-edge-length"),
      mods(*this, "mods"),
      mods_help(*this, "mods?", {"mods is a list of objects:", "{mod: root, s1: new-root}", "{mod: hide-isolated-before, s1: date}",
                      "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}", "{mod: before2015-58P-or-146I-or-559I}",
                      "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing", "{mod: hide-one, s1: name-to-hide} - after ladderizing",
                      "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}",
                      "{mod: mark-clade-with-line, clade: , color: , line_width: line-width-in-pixels}",
                      "{mod: mark-with-label, seq_id:, label:, line_color:, line_width:, label_offset:, label_size:, labeL_color:, label_style: }"
          }, rjson::v1::initialize_field::yes),
      force_line_width(*this, "force_line_width", false),
      line_width(*this, "line_width", 1),
      root_edge(*this, "root_edge", 0),
      line_color(*this, "line_color", "black"),
      label_style(*this, "label_style", {}),
      name_offset(*this, "name_offset", 0.3),
      color_nodes(*this, "color_nodes", "continent"),
      color_for_aa(*this, "color_for_aa", {}),
      right_padding(*this, "right_padding", 0),
      aa_transition(*this, "aa_transition"),
      legend(*this, "legend")
{
    mods.emplace_back().add("?mod", "hide-if-cumulative-edge-length-bigger-than").add("d1", rjson::v1::number{0.04});

} // TreeDrawSettings::TreeDrawSettings

void TreeDrawSettings::remove_for_signature_page_settings()
{
    for (const auto name: {"ladderize", "ladderize?", "mods", "mods?", "force_line_width", "line_width", "root_edge", "line_color", "label_style", "name_offset", "color_nodes", "vaccines", "legend"})
        remove_child(name);
    aa_transition.remove_for_signature_page_settings();

} // TreeDrawSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

TitleDrawSettings::TitleDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      title(*this, "title", ""),
      color(*this, "color", "black", rjson::v1::initialize_field::yes),
      size(*this, "size", 12, rjson::v1::initialize_field::yes),
      style(*this, "style", {}, rjson::v1::initialize_field::yes),
      offset(*this, "offset", {10, 30}, rjson::v1::initialize_field::yes)
{
}

// ----------------------------------------------------------------------

SignaturePageDrawSettings::SignaturePageDrawSettings(rjson::v1::field_container_parent& aParent, std::string aFieldName)
    : rjson::v1::field_container_child(aParent, aFieldName),
      layout(*this, "layout", "auto", rjson::v1::initialize_field::yes),
      top(*this, "top", 60, rjson::v1::initialize_field::yes),
      bottom(*this, "bottom", 60, rjson::v1::initialize_field::yes),
      left(*this, "left", 50, rjson::v1::initialize_field::yes),
      right(*this, "right", 20, rjson::v1::initialize_field::yes),
      tree_margin_right(*this, "tree_margin_right", 0, rjson::v1::initialize_field::yes),
      mapped_antigens_margin_right(*this, "mapped_antigens_margin_right", 30, rjson::v1::initialize_field::yes),
      time_series_width(*this, "time_series_width", 400, rjson::v1::initialize_field::yes),
      clades_width(*this, "clades_width", 100, rjson::v1::initialize_field::yes),
      antigenic_maps_width(*this, "antigenic_maps_width", 300, rjson::v1::initialize_field::yes),
      _force_pp(*this, rjson::v1::object::force_pp_key, true, rjson::v1::initialize_field::yes)
{
}


// **********************************************************************

Settings::Settings()
    : version(*this, "  version", SETTINGS_VERSION_4, rjson::v1::initialize_field::yes),
      signature_page(*this, "signature_page"),
      title(*this, "title"),
      tree_draw(*this, "tree"),
      time_series(*this, "time_series"),
      clades(*this, "clades"),
      hz_sections(*this, "hz_sections"),
      mapped_antigens(*this, "mapped_antigens"),
      aa_at_pos(*this, "aa_at_pos"),
      antigenic_maps(*this, "antigenic_maps")
{
    using namespace rjson::v1::literals;
      // add sample text mod
    set_field("mods", rjson::v1::array(rjson::v1::object{{{"?N", "text"_rj},
                    {"color", "black"_rj},
                    {"offset", rjson::v1::array{100.0, 100.0}},
                    {"size", rjson::v1::number{12.0}},
                    {"style", rjson::v1::object{{{"family", rjson::v1::string{}}, {"slant", "normal"_rj}, {"weight", "normal"_rj}}}},
                    {"text", "Text to show"_rj}
                }}));

    // std::cerr << "Settings: " << to_json_pp(2) << '\n';

} // Settings::Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: reading settings from " << aFilename << std::endl;
    aSettings.update(rjson::v1::parse_file(aFilename));
    aSettings.upgrade();

} // read_settings

// ----------------------------------------------------------------------

void Settings::upgrade()             // upgrade to the new version in case old version data provided
{
    if (version != SETTINGS_VERSION_4) {
        throw std::runtime_error("Unsupported settings version: " + static_cast<std::string>(version));
    }

} // Settings::upgrade

// ----------------------------------------------------------------------

void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent)
{
    std::cout << "INFO: writing settings to " << aFilename << std::endl;
    acmacs::file::write(aFilename, aSettings.to_json_pp(aIndent));

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
