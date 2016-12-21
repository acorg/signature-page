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
              // << JsonObjectKey("aa_transition") << aSettings.aa_transition
                  << JsonObjectKey("vaccine_label_color") << aSettings.vaccine_label_color
                  << JsonObjectKey("vaccine_label_size") << aSettings.vaccine_label_size
                  << JsonObjectKey("vaccine_label_style") << aSettings.vaccine_label_style
                  << JsonObjectKey("vaccine_line_color") << aSettings.vaccine_line_color
                  << JsonObjectKey("vaccine_line_width") << aSettings.vaccine_line_width
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
