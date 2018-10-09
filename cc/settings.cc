#include <cmath>

#include "acmacs-base/debug.hh"
#include "acmacs-base/float.hh"
#include "acmacs-base/read-file.hh"

#include "settings.hh"
using namespace std::string_literals;

// ----------------------------------------------------------------------

static constexpr const char* SETTINGS_VERSION_4 = "signature-page-settings-v4";

// ----------------------------------------------------------------------

//v1 AATransitionIndividualSettings::AATransitionIndividualSettings(const rjson::value& aData)
//v1     : rjson::array_field_container_child_element(aData),
//v1       label(*this, "label", "", rjson::initialize_field::yes),
//v1       first_leaf_seq_id(*this, "first_leaf_seq_id", ""),
//v1       show(*this, "show", true),
//v1       size(*this, "size", 8),
//v1       color(*this, "color", "black"),
//v1       style(*this, "style", std::string_view("Courier New")),
//v1       interline(*this, "interline", 1.2),
//v1       label_offset(*this, "label_offset", {-40, 20}),
//v1       label_connection_line_width(*this, "label_connection_line_width", 0.1),
//v1       label_connection_line_color(*this, "label_connection_line_color", "black")
//v1 {
//v1 }

//v1 AATransitionPerBranchDrawSettings::AATransitionPerBranchDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       show(*this, "show", true, rjson::initialize_field::yes),
//v1       size(*this, "size", 8, rjson::initialize_field::yes),
//v1       color(*this, "color", "black", rjson::initialize_field::yes),
//v1       style(*this, "style", std::string_view("Courier New"), rjson::initialize_field::yes),
//v1       interline(*this, "interline", 1.2, rjson::initialize_field::yes),
//v1       label_offset(*this, "label_offset", {-40, 20}, rjson::initialize_field::yes),
//v1       scatter_label_offset(*this, "scatter_label_offset", 0, rjson::initialize_field::yes),
//v1       scatter_label_offset_help(*this, "scatter_label_offset?", "randomization range for label offset in trying to avoid clattering (mostly for figuring out good offsets", rjson::initialize_field::yes),
//v1       label_connection_line_width(*this, "label_connection_line_width", 0.1, rjson::initialize_field::yes),
//v1       label_connection_line_color(*this, "label_connection_line_color", "black", rjson::initialize_field::yes),
//v1       by_aa_label(*this, "by_aa_label")
//v1 {
//v1 }

//v1 void AATransitionPerBranchDrawSettings::remove_for_signature_page_settings()
//v1 {
//v1     for (const auto name: {"show", "size", "color", "style", "interline", "label_offset", "scatter_label_offset", "scatter_label_offset?", "label_connection_line_width", "label_connection_line_color"})
//v1         remove_child(name);
//v1 }

//v1 AATransitionDrawSettings::AATransitionDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       show(*this, "show", true, rjson::initialize_field::yes),
//v1       number_strains_threshold(*this, "number_strains_threshold", 20, rjson::initialize_field::yes),
//v1       number_strains_threshold_help(*this, "number_strains_threshold?", "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value", rjson::initialize_field::yes),
//v1       show_empty_left(*this, "show_empty_left", false, rjson::initialize_field::yes),
//v1       per_branch(*this, "per_branch"),
//v1       show_node_for_left_line(*this, "show_node_for_left_line", false, rjson::initialize_field::yes),
//v1       node_for_left_line_color(*this, "node_for_left_line_color", "green", rjson::initialize_field::yes),
//v1       node_for_left_line_width(*this, "node_for_left_line_width", 1, rjson::initialize_field::yes)
//v1 {
//v1 }

//v1 void AATransitionDrawSettings::remove_for_signature_page_settings()
//v1 {
//v1     for (const auto name: {"show", "number_strains_threshold", "number_strains_threshold?", "show_empty_left", "show_node_for_left_line", "node_for_left_line_color", "node_for_left_line_width"})
//v1         remove_child(name);
//v1     per_branch.remove_for_signature_page_settings();
//v1 }

//v1 TreeDrawVaccineSettings::TreeDrawVaccineSettings(const rjson::value& aData)
//v1     : rjson::array_field_container_child_element(aData),
//v1       name(*this, "name", "", rjson::initialize_field::yes),
//v1       name_help(*this, "name?", "empty for default settings", rjson::initialize_field::yes),
//v1       label_color(*this, "label_color", "black", rjson::initialize_field::yes),
//v1       label_size(*this, "label_size", 10, rjson::initialize_field::yes),
//v1       label_style(*this, "label_style", {}, rjson::initialize_field::yes),
//v1       line_color(*this, "line_color", "black", rjson::initialize_field::yes),
//v1       line_width(*this, "line_width", 1, rjson::initialize_field::yes)
//v1 {
//v1 }

//v1 LegendSettings::LegendSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       offset(*this, "offset", {-30, 950}, rjson::initialize_field::yes),
//v1       width(*this, "width", 100, rjson::initialize_field::yes),
//v1       title_style(*this, "title_style", std::string_view("sans_serif"), rjson::initialize_field::yes),
//v1       title_size(*this, "title_size", 10, rjson::initialize_field::yes),
//v1       text_style(*this, "text_style", std::string_view("monospace"), rjson::initialize_field::yes),
//v1       text_size(*this, "text_size", 10, rjson::initialize_field::yes),
//v1       interline(*this, "interline", 1.5, rjson::initialize_field::yes)
//v1 {
//v1 }

TimeSeriesDrawSettings::TimeSeriesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      begin(*this, "begin", ""),
      end(*this, "end", ""),
      label_size(*this, "label_size", 8, rjson::initialize_field::yes),
      label_style(*this, "label_style", {}, rjson::initialize_field::yes),
      month_year_to_timeseries_gap(*this, "month_year_to_timeseries_gap", 2, rjson::initialize_field::yes),
      month_separator_color(*this, "month_separator_color", "black", rjson::initialize_field::yes),
      month_separator_width(*this, "month_separator_width", 0.5, rjson::initialize_field::yes),
      dash_width(*this, "dash_width", 0.5, rjson::initialize_field::yes),
      dash_line_width(*this, "dash_line_width", 1, rjson::initialize_field::yes)
{
}

//v1 CladeDrawSettings::CladeDrawSettings(const rjson::value& aData, std::string aName, bool aShow)
//v1     : rjson::array_field_container_child_element(aData),
//v1       name(*this, "name", aName, rjson::initialize_field::yes),
//v1       display_name(*this, "display_name", "", rjson::initialize_field::yes),
//v1       show(*this, "show", aShow, rjson::initialize_field::yes),
//v1       section_inclusion_tolerance(*this, "section_inclusion_tolerance", 10, rjson::initialize_field::yes),
//v1       section_exclusion_tolerance(*this, "section_exclusion_tolerance", 5, rjson::initialize_field::yes),
//v1       show_section_size_in_label(*this, "show_section_size_in_label", true, rjson::initialize_field::yes),
//v1       arrow_color(*this, "arrow_color", "black", rjson::initialize_field::yes),
//v1       line_width(*this, "line_width", 0.8, rjson::initialize_field::yes),
//v1       arrow_width(*this, "arrow_width", 3, rjson::initialize_field::yes),
//v1       separator_color(*this, "separator_color", "grey63", rjson::initialize_field::yes),
//v1       separator_width(*this, "separator_width", 0.5, rjson::initialize_field::yes),
//v1       label_position(*this, "label_position", "middle", rjson::initialize_field::yes),
//v1       label_offset(*this, "label_offset", {5, 0}, rjson::initialize_field::yes),
//v1       label_color(*this, "label_color", "black", rjson::initialize_field::yes),
//v1       label_size(*this, "label_size", 11, rjson::initialize_field::yes),
//v1       label_style(*this, "label_style", {}, rjson::initialize_field::yes),
//v1       label_rotation(*this, "label_rotation", 0, rjson::initialize_field::yes),
//v1       slot(*this, "slot", NoSlot, rjson::initialize_field::yes)
//v1 {
//v1 }

//v1 CladesDrawSettings::CladesDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       clades(*this, "clades"),
//v1       slot_width(*this, "slot_width", 10, rjson::initialize_field::yes)
//v1 {
//v1     clades.emplace_back();
//v1     // for (const auto* n: {"GLY", "NO-GLY", "DEL2017"}) {
//v1     //     auto cn = clades.emplace_back();
//v1     //     cn.name = n;
//v1     //     cn.show = false;
//v1     // }
//v1     // // for (const auto* n: {"3C2A1A", "3C2A1B", "3C2A1", "3C2A2", "3C2A3", "3C2A4"}) {
//v1     // //     auto cn = clades.emplace_back();
//v1     // //     cn.name = n;
//v1     // //     cn.section_inclusion_tolerance = 15;
//v1     // //     cn.section_exclusion_tolerance = 10;
//v1     // //     cn.slot = static_cast<int>(cn.name.get_value_ref().str().size() - 4);
//v1     // // }
//v1     // for (auto [name, display, slot] : {}) { // Sarah's H3 clades 2018-08-09
//v1     //     auto cn = clades.emplace_back();
//v1     //     cn.name = name;
//v1     //     cn.display_name = display;
//v1     //     cn.section_inclusion_tolerance = 15;
//v1     //     cn.section_exclusion_tolerance = 10;
//v1     //     cn.slot = slot;
//v1     // }
//v1 }

//v1 HzSection::HzSection(const rjson::value& aData)
//v1     : rjson::array_field_container_child_element(aData),
//v1       show(*this, "show", true, rjson::initialize_field::yes),
//v1       show_line(*this, "show_line", true, rjson::initialize_field::yes),
//v1       show_label_in_time_series(*this, "show_label_in_time_series", true, rjson::initialize_field::yes),
//v1       show_map(*this, "show_map", true, rjson::initialize_field::yes),
//v1       name(*this, "name", "", rjson::initialize_field::yes),
//v1       label(*this, "label", "", rjson::initialize_field::yes),
//v1       triggering_clades(*this, "triggering_clades"),
//v1       triggering_aa_pos(*this, "triggering_aa_pos")
//v1       // first(nullptr),
//v1       // last(nullptr),
//v1       // index("?")
//v1 {
//v1 }

//v1 HzSections::HzSections(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       vertical_gap(*this, "vertical_gap", 20, rjson::initialize_field::yes),
//v1       line_color(*this, "line_color", "grey63", rjson::initialize_field::yes),
//v1       line_width(*this, "line_width", 1, rjson::initialize_field::yes),
//v1       ts_label_size(*this, "ts_label_size", 10, rjson::initialize_field::yes),
//v1       ts_label_style(*this, "ts_label_style", {}, rjson::initialize_field::yes),
//v1       ts_label_color(*this, "ts_label_color", "black", rjson::initialize_field::yes),
//v1       sections(*this, "sections"),
//v1       show_labels_in_time_series_in_tree_mode(*this, "show_labels_in_time_series_in_tree_mode", false, rjson::initialize_field::yes)
//v1 {
//v1 }

MappedAntigensDrawSettings::MappedAntigensDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      width(*this, "width", 10, rjson::initialize_field::yes),
      line_width(*this, "line_width", 0.5, rjson::initialize_field::yes),
      line_color(*this, "line_color", "grey56", rjson::initialize_field::yes),
      line_length(*this, "line_length", 0.5, rjson::initialize_field::yes)
{
}

// AAAtPosSection::AAAtPosSection(const rjson::value& aData)
//     : rjson::array_field_container_child_element(aData),
//       pos(*this, "_pos", 0),
//       aa(*this, "aa", ""),
//       num(*this, "num", 0),
//       first(*this, "first", ""),
//       last(*this, "last", "")
// {
// } // AAAtPosSection::AAAtPosSection

AAAtPosDrawSettings::AAAtPosDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      width(*this, "width", 0, rjson::initialize_field::yes),
      right_margin(*this, "right_margin", 0, rjson::initialize_field::yes),
      line_width(*this, "line_width", 0.15, rjson::initialize_field::yes),
      line_length(*this, "line_length", 0.5, rjson::initialize_field::yes),
      diverse_index_threshold(*this, "diverse_index_threshold", 3, rjson::initialize_field::yes),
      positions(*this, "positions"),
      report_most_diverse_positions(*this, "report_most_diverse_positions", false, rjson::initialize_field::yes),
      small_section_threshold(*this, "small_section_threshold", 3, rjson::initialize_field::yes),
      sections(*this, "?sections")
{
} // AAAtPosDrawSettings::AAAtPosDrawSettings

// ----------------------------------------------------------------------

// AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//     : rjson::field_container_child(aParent, aFieldName),
//       layout(*this, "layout", "labelled_grid", rjson::initialize_field::yes),
//       columns(*this, "columns", 3, rjson::initialize_field::yes),
//       gap(*this, "gap", 20, rjson::initialize_field::yes),
//       mapped_antigens_section_line_color(*this, "mapped_antigens_section_line_color", "black", rjson::initialize_field::yes),
//       mapped_antigens_section_line_width(*this, "mapped_antigens_section_line_width", 1, rjson::initialize_field::yes),
//       mods(*this, "mods")
// {
//     mods.emplace_back().add("N", "viewport").add("rel", rjson::array{0, 0, 0});
//     mods.emplace_back().add("N", "point_scale").add("scale", 1.0).add("outline_scale", 1.0);
//     mods.emplace_back().add("N", "rotate").add("degrees", 0.0);
//     mods.emplace_back().add("N", "sera").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
//     mods.emplace_back().add("N", "reference_antigens").add("fill", "transparent").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
//     mods.emplace_back().add("N", "test_antigens").add("fill", "grey88").add("outline", "grey88").add("outline_width", 0.5).add("size", 3.0);
//     mods.emplace_back().add("N", "sequenced_antigens").add("fill", "grey63").add("outline", "white").add("outline_width", 0.5).add("size", 3.0);
//     mods.emplace_back().add("N", "tracked_antigens").add("fill", "green3").add("outline", "white").add("outline_width", 0.5).add("size", 5.0);
//     mods.emplace_back().add("N", "?tracked_sera").add("fill", "transparent").add("outline", "black").add("outline_width", 0.5).add("size", 5.0); // tracked sera above tracked antigens!
//     mods.emplace_back().add("N", "?tracked_serum_circles").add("outline", "black").add("outline_width", 0.5); // tracked serum circles above tracked antigens!
//     mods.emplace_back().add("N", "?serum_circle").add("map", "A").add("serum", "A(H3N2)/SOUTH CAROLINA/4/2017 CDC 2017-106")
//             .add("outline", "black").add("outline_width", 0.5).add("fill", "transparent").add("radius_line", "transparent").add("radius_line_width", 1.0).add("radius_line_dash", "nodash")
//             .add("serum_size", 5.0).add("serum_outline", "black").add("serum_outline_width", 0.5);
//     mods.emplace_back().add("N", "title").add("text_color", "black").add("text_size", 12.0).add("padding", 3.0)
//             .add("offset", rjson::array{0.0, 0.0}).add("weight", "normal").add("slant", "normal").add("font_family", "san serif");
//     mods.emplace_back().add("N", "background").add("color", "white");
//     mods.emplace_back().add("N", "grid").add("color", "grey80").add("line_width", 1.0);
//     mods.emplace_back().add("N", "border").add("color", "black").add("line_width", 1.0);

//       // vaccine spec via acmacs-map-draw/ModAntigens, since 2018-01-19
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "cell"}}}})
//             .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "egg"}}}})
//             .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("?N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "reassortant"}}}})
//             .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});

//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "cell"}}}})
//             .add("fill", "red").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "egg"}}}})
//             .add("fill", "red").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "reassortant"}}}})
//             .add("fill", "green").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});

//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "cell"}}}})
//             .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "egg"}}}})
//             .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
//     mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "reassortant"}}}})
//             .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
//             .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});

//     //   // old vaccine spec
//     // mods.emplace_back().add("N", "vaccines").add("mods", rjson::array{
//     //         rjson::object{{"size", 15.0}, {"outline", "white"}},
//     //         rjson::object{{"label", rjson::object{{"offset", rjson::array{0, 1}}, {"color", "black"},
//     //                                               {"weight", "normal"}, {"slant", "normal"},
//     //                                               {"font_family", "helvetica neu"}, {"size", 9.0},
//     //                                               {"name_type", "abbreviated_location_with_passage_type"}}}},
//     //         rjson::object{{"type", "previous"}, {"passage", "cell"}, {"fill", "blue"}},
//     //         rjson::object{{"type", "previous"}, {"passage", "egg"}, {"fill", "blue"}},
//     //         rjson::object{{"type", "previous"}, {"passage", "reassortant"}, {"fill", "blue"}},
//     //         rjson::object{{"type", "current"}, {"passage", "cell"}, {"fill", "red"}},
//     //         rjson::object{{"type", "current"}, {"passage", "egg"}, {"fill", "red"}},
//     //         rjson::object{{"type", "current"}, {"passage", "reassortant"}, {"fill", "green"}},
//     //         rjson::object{{"type", "surrogate"}, {"passage", "cell"}, {"fill", "pink"}},
//     //         rjson::object{{"type", "surrogate"}, {"passage", "egg"}, {"fill", "pink"}},
//     //         rjson::object{{"type", "surrogate"}, {"passage", "reassortant"}, {"fill", "pink"}},
//     //         rjson::object{{"?name", "TEXAS"}, {"?show", false}},
//     //    });
// }

// void AntigenicMapsDrawSettings::viewport(const acmacs::Viewport& aViewport)
// {
//     auto make_setting_list = [&aViewport]() -> rjson::value {
//         if (float_equal(aViewport.size.width, aViewport.size.height))
//             return rjson::array{aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width};
//         else
//             return rjson::array{aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width, aViewport.size.height};
//     };
//       // std::cerr << "DEBUG: AntigenicMapsDrawSettings::viewport" << std::endl;
//     if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name() == "viewport"; }); !vpmod) {
//         auto mod = mods.emplace_back();
//         mod.set_field("N", "viewport");
//         mod.set_field("viewport", make_setting_list());
//     }
//     else
//         (*vpmod).set_field("viewport", make_setting_list());

// } // AntigenicMapsDrawSettings::viewport

acmacs::Viewport AntigenicMapMod::get_viewport(const acmacs::Viewport& aOrigViewport) const
{
    acmacs::Viewport result = aOrigViewport;

    const auto& rel = operator[]("rel");
    try {
        switch (rel.size()) {
            case 3:
                result.set({aOrigViewport.left() + static_cast<double>(rel[0]), aOrigViewport.top() + static_cast<double>(rel[1])}, aOrigViewport.size.width + static_cast<double>(rel[2]));
                break;
            default:
                throw std::exception{};
        }
    }
    catch (std::exception&) {
        std::cerr << "ERROR: cannot convert json to array (viewport rel): " << rel << '\n';
        throw;
    }

    const auto& viewport = operator[]("viewport");
    try {
        switch (viewport.size()) {
            case 3:
                result.set({viewport[0], viewport[1]}, viewport[2]);
                break;
            case 4:
                result.set(acmacs::Location2D{viewport[0], viewport[1]}, acmacs::Size{viewport[2], viewport[3]});
                break;
            default:
                throw std::exception{};
        }
    }
    catch (std::exception&) {
        std::cerr << "ERROR: cannot convert json to array (viewport): " << viewport << '\n';
        throw;
    }

    return result;

} // AntigenicMapMod::get_viewport

// ----------------------------------------------------------------------

//v1 TreeDrawSettings::TreeDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       ladderize(*this, "ladderize", Tree::LadderizeMethod::NumberOfLeaves),
//v1       ladderize_help(*this, "ladderize?", "number-of-leaves or max-edge-length"),
//v1       mods(*this, "mods"),
//v1       mods_help(*this, "mods?", {"mods is a list of objects:", "{mod: root, s1: new-root}", "{mod: hide-isolated-before, s1: date}",
//v1                       "{mod: hide-if-cumulative-edge-length-bigger-than, d1: cumulative-length-threshold}", "{mod: before2015-58P-or-146I-or-559I}",
//v1                       "{mod: hide-between, s1: first-name-to-hide, s2: last-name-to-hide} - after ladderizing", "{mod: hide-one, s1: name-to-hide} - after ladderizing",
//v1                       "{mod: mark-with-line, s1: name-to-mark, s2: color-to-mark, d1: line-width-in-pixels}",
//v1                       "{mod: mark-clade-with-line, clade: , color: , line_width: line-width-in-pixels}",
//v1                       "{mod: mark-with-label, seq_id:, label:, line_color:, line_width:, label_offset:, label_size:, labeL_color:, label_style: }"
//v1           }, rjson::initialize_field::yes),
//v1       force_line_width(*this, "force_line_width", false),
//v1       line_width(*this, "line_width", 1),
//v1       root_edge(*this, "root_edge", 0),
//v1       line_color(*this, "line_color", "black"),
//v1       label_style(*this, "label_style", {}),
//v1       name_offset(*this, "name_offset", 0.3),
//v1       color_nodes(*this, "color_nodes", "continent"),
//v1       color_for_aa(*this, "color_for_aa", {}),
//v1       right_padding(*this, "right_padding", 0),
//v1       aa_transition(*this, "aa_transition"),
//v1       legend(*this, "legend")
//v1 {
//v1     mods.emplace_back().add("?mod", "hide-if-cumulative-edge-length-bigger-than").add("d1", 0.04);
//v1
//v1 } // TreeDrawSettings::TreeDrawSettings

//v1 void TreeDrawSettings::remove_for_signature_page_settings()
//v1 {
//v1     for (const auto name: {"ladderize", "ladderize?", "mods", "mods?", "force_line_width", "line_width", "root_edge", "line_color", "label_style", "name_offset", "color_nodes", "vaccines", "legend"})
//v1         remove_child(name);
//v1     aa_transition.remove_for_signature_page_settings();
//v1
//v1 } // TreeDrawSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

TitleDrawSettings::TitleDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
    : rjson::field_container_child(aParent, aFieldName),
      title(*this, "title", ""),
      color(*this, "color", "black", rjson::initialize_field::yes),
      size(*this, "size", 12, rjson::initialize_field::yes),
      style(*this, "style", {}, rjson::initialize_field::yes),
      offset(*this, "offset", {10, 30}, rjson::initialize_field::yes)
{
}

// ----------------------------------------------------------------------

//v1 SignaturePageDrawSettings::SignaturePageDrawSettings(rjson::field_container_parent& aParent, std::string aFieldName)
//v1     : rjson::field_container_child(aParent, aFieldName),
//v1       layout(*this, "layout", "auto", rjson::initialize_field::yes),
//v1       top(*this, "top", 60, rjson::initialize_field::yes),
//v1       bottom(*this, "bottom", 60, rjson::initialize_field::yes),
//v1       left(*this, "left", 50, rjson::initialize_field::yes),
//v1       right(*this, "right", 20, rjson::initialize_field::yes),
//v1       tree_margin_right(*this, "tree_margin_right", 0, rjson::initialize_field::yes),
//v1       mapped_antigens_margin_right(*this, "mapped_antigens_margin_right", 30, rjson::initialize_field::yes),
//v1       time_series_width(*this, "time_series_width", 400, rjson::initialize_field::yes),
//v1       clades_width(*this, "clades_width", 100, rjson::initialize_field::yes),
//v1       antigenic_maps_width(*this, "antigenic_maps_width", 300, rjson::initialize_field::yes)
//v1 {
//v1 }


// **********************************************************************

Settings::Settings()
    : version(*this, "  version", SETTINGS_VERSION_4, rjson::initialize_field::yes),
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
      // add sample text mod
    set_field("mods", rjson::value(rjson::object{{{"?N", "text"},
                    {"color", "black"},
                    {"offset", rjson::array{100.0, 100.0}},
                    {"size", 12.0},
                    {"style", rjson::object{{{"family", ""}, {"slant", "normal"}, {"weight", "normal"}}}},
                    {"text", "Text to show"}
                }}));

    // std::cerr << "Settings: " << to_json_pp(2) << '\n';

} // Settings::Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: reading settings from " << aFilename << std::endl;
    aSettings.update(rjson::parse_file(aFilename));
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
