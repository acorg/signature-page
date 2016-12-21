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

class SettingsSizeHandler : public HandlerBase
{
 public:
    inline SettingsSizeHandler(Settings& aSettings, Size& aField) : HandlerBase{aSettings}, mElement(0), mField(aField) {}

    inline virtual HandlerBase* Double(double d)
        {
            switch (mElement) {
              case 0:
                  mField.width = d;
                  break;
              case 1:
                  mField.height = d;
                  break;
              default:
                  throw json_reader::Failure{};
            }
            ++mElement;
            return nullptr;
        }

 private:
    size_t mElement;
    Size& mField;
};

// ----------------------------------------------------------------------

class AATransitionPerBranchDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, size, color, style, interline, label_offset, label_connection_line_width, label_connection_line_color};

 public:
    inline AATransitionPerBranchDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

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
              case Keys::color:
                  mTarget.tree_draw.aa_transition.per_branch.color.from_string(str, length);
                  break;
              case Keys::label_connection_line_color:
                  mTarget.tree_draw.aa_transition.per_branch.label_connection_line_color.from_string(str, length);
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
              case Keys::size:
                  mTarget.tree_draw.aa_transition.per_branch.size = d;
                  break;
              case Keys::interline:
                  mTarget.tree_draw.aa_transition.per_branch.interline = d;
                  break;
              case Keys::label_connection_line_width:
                  mTarget.tree_draw.aa_transition.per_branch.label_connection_line_width = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::style:
                  result = new SettingsTextStyleHandler(mTarget, mTarget.tree_draw.aa_transition.per_branch.style);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* StartArray()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::label_offset:
                  result = new SettingsSizeHandler(mTarget, mTarget.tree_draw.aa_transition.per_branch.label_offset);
                  break;
              default:
                  result = HandlerBase::StartArray();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class AATransitionPerBranchDrawSettingsHandler

const std::map<std::string, AATransitionPerBranchDrawSettingsHandler::Keys> AATransitionPerBranchDrawSettingsHandler::key_mapper {
    {"size", Keys::size},
    {"color", Keys::color},
    {"style", Keys::style},
    {"interline", Keys::interline},
    {"label_offset", Keys::label_offset},
    {"label_connection_line_width", Keys::label_connection_line_width},
    {"label_connection_line_color", Keys::label_connection_line_color}
};

// ----------------------------------------------------------------------

class AATransitionDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, show, number_strains_threshold, show_empty_left, per_branch, show_node_for_left_line, node_for_left_line_color, node_for_left_line_width};

 public:
    inline AATransitionDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

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
              case Keys::node_for_left_line_color:
                  mTarget.tree_draw.aa_transition.node_for_left_line_color.from_string(str, length);
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
              case Keys::node_for_left_line_width:
                  mTarget.tree_draw.aa_transition.node_for_left_line_width = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* Uint(unsigned u)
        {
            switch (mKey) {
              case Keys::number_strains_threshold:
                  mTarget.tree_draw.aa_transition.number_strains_threshold = u;
                  break;
              default:
                  HandlerBase::Uint(u);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* Bool(bool b)
        {
            switch (mKey) {
              case Keys::show:
                  mTarget.tree_draw.aa_transition.show = b;
                  break;
              case Keys::show_empty_left:
                  mTarget.tree_draw.aa_transition.show_empty_left = b;
                  break;
              case Keys::show_node_for_left_line:
                  mTarget.tree_draw.aa_transition.show_node_for_left_line = b;
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
              case Keys::per_branch:
                  result = new AATransitionPerBranchDrawSettingsHandler(mTarget);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class AATransitionDrawSettingsHandler

const std::map<std::string, AATransitionDrawSettingsHandler::Keys> AATransitionDrawSettingsHandler::key_mapper {
    {"show", Keys::show},
    {"number_strains_threshold", Keys::number_strains_threshold},
    {"show_empty_left", Keys::show_empty_left},
    {"per_branch", Keys::per_branch},
    {"show_node_for_left_line", Keys::show_node_for_left_line},
    {"node_for_left_line_color", Keys::node_for_left_line_color},
    {"node_for_left_line_width", Keys::node_for_left_line_width}
};

// ----------------------------------------------------------------------

class TreeDrawVaccineSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, name, label_color, label_size, label_style, line_color, line_width};

 public:
    inline TreeDrawVaccineSettingsHandler(Settings& aSettings, TreeDrawVaccineSettings& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

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
              case Keys::name:
                  mField.name.assign(str, length);
                  break;
              case Keys::label_color:
                  mField.label_color.from_string(str, length);
                  break;
              case Keys::line_color:
                  mField.line_color.from_string(str, length);
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
              case Keys::label_size:
                  mField.label_size = d;
                  break;
              case Keys::line_width:
                  mField.line_width = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::label_style:
                  result = new SettingsTextStyleHandler(mTarget, mField.label_style);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    TreeDrawVaccineSettings& mField;
    static const std::map<std::string, Keys> key_mapper;

}; // class TreeDrawVaccineSettingsHandler

const std::map<std::string, TreeDrawVaccineSettingsHandler::Keys> TreeDrawVaccineSettingsHandler::key_mapper {
    {"name", Keys::name},
    {"label_color", Keys::label_color},
    {"label_size", Keys::label_size},
    {"label_style", Keys::label_style},
    {"line_color", Keys::line_color},
    {"line_width", Keys::line_width}
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
                switch (mKey) {
                  case Keys::vaccines:
                      result = new json_reader::ListHandler<Settings, TreeDrawVaccineSettings, TreeDrawVaccineSettingsHandler>(mTarget, mTarget.tree_draw.vaccines);
                      break;
                  default:
                      break;
                }
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
              case Keys::aa_transition:
                  result = new AATransitionDrawSettingsHandler(mTarget);
                  break;
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
