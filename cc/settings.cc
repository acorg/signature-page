#include "settings.hh"
#include "acmacs-base/json-reader.hh"
#include "acmacs-base/json-writer.hh"

// ----------------------------------------------------------------------

static constexpr const char* SETTINGS_VERSION = "signature-page-settings-v2";

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

using HandlerBase = json_reader::HandlerBase<Settings>;

// ----------------------------------------------------------------------

class SettingsTextStyleHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, family, slant, weight};

 public:
    inline SettingsTextStyleHandler(Settings& aSettings, TextStyle& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::family:
                  mField.font_family(str, length);
                  break;
              case Keys::slant:
                  mField.slant(str, length);
                  break;
              case Keys::weight:
                  mField.weight(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    TextStyle& mField;
    static const std::map<std::string, Keys> key_mapper;
};

const std::map<std::string, SettingsTextStyleHandler::Keys> SettingsTextStyleHandler::key_mapper {
    {"family", Keys::family},
    {"slant", Keys::slant},
    {"weight", Keys::weight}
};

// ----------------------------------------------------------------------

class SettingsTreeHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than,
                force_line_width, line_width, root_edge, line_color, label_style,
                name_offset, color_nodes, aa_transition, vaccines};

 public:
    inline SettingsTreeHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::hide_isolated_before:
                  mTarget.tree_draw.hide_isolated_before.assign(str, length);
                  break;
              case Keys::line_color:
                  mTarget.tree_draw.line_color.from_string(str, length);
                  break;
              case Keys::color_nodes:
                  mTarget.tree_draw.color_nodes.assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mKey) {
              case Keys::hide_if_cumulative_edge_length_bigger_than:
                  mTarget.tree_draw.hide_if_cumulative_edge_length_bigger_than = d;
                  break;
              case Keys::line_width:
                  mTarget.tree_draw.line_width = d;
                  break;
              case Keys::root_edge:
                  mTarget.tree_draw.root_edge = d;
                  break;
              case Keys::name_offset:
                  mTarget.tree_draw.name_offset = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* Bool(bool b)
        {
            switch (mKey) {
              case Keys::force_line_width:
                  mTarget.tree_draw.force_line_width = b;
                  break;
              default:
                  HandlerBase::Bool(b);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::label_style:
                  result = new SettingsTextStyleHandler(mTarget, mTarget.tree_draw.label_style);
                  break;
                    // case Keys::aa_transition:
                    //     result = new (mTarget);
                    //     break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class SettingsTreeHandler

const std::map<std::string, SettingsTreeHandler::Keys> SettingsTreeHandler::key_mapper {
    {"hide_isolated_before", Keys::hide_isolated_before},
    {"hide_if_cumulative_edge_length_bigger_than", Keys::hide_if_cumulative_edge_length_bigger_than},
    {"force_line_width", Keys::force_line_width},
    {"line_width", Keys::line_width},
    {"root_edge", Keys::root_edge},
    {"line_color", Keys::line_color},
    {"label_style", Keys::label_style},
    {"name_offset", Keys::name_offset},
    {"color_nodes", Keys::color_nodes},
    {"aa_transition", Keys::aa_transition},
    {"vaccines", Keys::vaccines}
};

// ----------------------------------------------------------------------

class SettingsRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, Version, Tree };

 public:
    inline SettingsRootHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            const std::string found_key(str, length);
            if (found_key == "  version") {
                mKey = Keys::Version;
            }
            else if (found_key == "tree") {
                mKey = Keys::Tree;
            }
            else {
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::Version:
                  if (strncmp(str, SETTINGS_VERSION, std::min(length, static_cast<rapidjson::SizeType>(strlen(SETTINGS_VERSION))))) {
                      std::cerr << "Unsupported version: \"" << std::string(str, length) << '"' << std::endl;
                      throw json_reader::Failure();
                  }
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

        inline virtual HandlerBase* StartObject()
            {
                HandlerBase* result = nullptr;
                switch (mKey) {
                  case Keys::Tree:
                      result = new SettingsTreeHandler(mTarget);
                      break;
                  default:
                      result = HandlerBase::StartObject();
                      break;
                }
                return result;
            }

 private:
    Keys mKey;
};

// ----------------------------------------------------------------------

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{
    json_reader::read_from_file<Settings, SettingsRootHandler>(aFilename, aSettings);

} // read_settings

// ----------------------------------------------------------------------
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

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const TreeDrawVaccineSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("name") << aSettings.name
                  << JsonObjectKey("name?") << "empty for default settings"
                  << JsonObjectKey("label_color") << aSettings.label_color
                  << JsonObjectKey("label_size") << aSettings.label_size
                  << JsonObjectKey("label_style") << aSettings.label_style
                  << JsonObjectKey("line_color") << aSettings.line_color
                  << JsonObjectKey("line_width") << aSettings.line_width
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
                  << JsonObjectKey("aa_transition") << aSettings.aa_transition
                  << JsonObjectKey("vaccines") << aSettings.vaccines
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
