#include "settings.hh"
#include "acmacs-base/json-reader.hh"
#include "acmacs-base/json-writer.hh"

// ----------------------------------------------------------------------

static constexpr const char* SETTINGS_VERSION = "signature-page-settings-v2";

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{

} // read_settings

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const TextStyle& aStyle)
{
    return writer << StartObject
                  << JsonObjectKey("family") << aStyle.font_family()
                  << JsonObjectKey("slant") << aStyle.slant_as_stirng()
                  << JsonObjectKey("weight") << aStyle.weight_as_stirng()
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const Size& aSize)
{
    return writer << StartArray << aSize.width << aSize.height << EndArray;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const AATransitionPerBranchDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("size") << aSettings.size
                  << JsonObjectKey("color") << aSettings.color
                  << JsonObjectKey("style") << aSettings.style
                  << JsonObjectKey("interline") << aSettings.interline
                  << JsonObjectKey("label_offset") << aSettings.label_offset
                  << JsonObjectKey("label_connection_line_width") << aSettings.label_connection_line_width
                  << JsonObjectKey("label_connection_line_color") << aSettings.label_connection_line_color
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const AATransitionDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("show") << aSettings.show
                  << JsonObjectKey("number_strains_threshold") << aSettings.number_strains_threshold
                  << JsonObjectKey("number_strains_threshold?") << "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"
                  << JsonObjectKey("show_empty_left") << aSettings.show_empty_left
                  << JsonObjectKey("show_node_for_left_line") << aSettings.show_node_for_left_line
                  << JsonObjectKey("node_for_left_line_color") << aSettings.node_for_left_line_color
                  << JsonObjectKey("node_for_left_line_width") << aSettings.node_for_left_line_width
                  << JsonObjectKey("per_branch") << aSettings.per_branch
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const TreeDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("hide_isolated_before") << aSettings.hide_isolated_before
                  << JsonObjectKey("hide_if_cumulative_edge_length_bigger_than") << aSettings.hide_if_cumulative_edge_length_bigger_than
                  << JsonObjectKey("force_line_width") << aSettings.force_line_width
                  << JsonObjectKey("line_width") << aSettings.line_width
                  << JsonObjectKey("root_edge") << aSettings.root_edge
                  << JsonObjectKey("line_color") << aSettings.line_color
                  << JsonObjectKey("label_style") << aSettings.label_style
                  << JsonObjectKey("name_offset") << aSettings.name_offset
                  << JsonObjectKey("color_nodes") << aSettings.color_nodes
                  << JsonObjectKey("color_nodes?") << "black, continent, position number (e.g. 162)"
                  << JsonObjectKey("vaccine_label_color") << aSettings.vaccine_label_color
                  << JsonObjectKey("vaccine_label_size") << aSettings.vaccine_label_size
                  << JsonObjectKey("vaccine_label_style") << aSettings.vaccine_label_style
                  << JsonObjectKey("vaccine_line_color") << aSettings.vaccine_line_color
                  << JsonObjectKey("vaccine_line_width") << aSettings.vaccine_line_width
                  << JsonObjectKey("aa_transition") << aSettings.aa_transition
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const Settings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("  version") << SETTINGS_VERSION
                  << JsonObjectKey("tree") << aSettings.tree_draw
                  << EndObject;
}

// ----------------------------------------------------------------------

void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent)
{
    export_to_json(aSettings, SETTINGS_VERSION, aFilename, aIndent);

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
