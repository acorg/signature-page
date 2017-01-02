#include "settings.hh"
#include "acmacs-base/json-reader.hh"
#include "acmacs-base/json-writer.hh"
#include "acmacs-base/float.hh"

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

TreeDrawSettings::~TreeDrawSettings()
{
}

TimeSeriesDrawSettings::~TimeSeriesDrawSettings()
{
}

CladeDrawSettings::~CladeDrawSettings()
{
}

CladesDrawSettings::~CladesDrawSettings()
{
}

HzSection::~HzSection()
{
}

HzSections::~HzSections()
{
}

MappedAntigensDrawSettings::~MappedAntigensDrawSettings()
{
}

AntigenicMapsDrawSettings::~AntigenicMapsDrawSettings()
{
}

TitleDrawSettings::~TitleDrawSettings()
{
}

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
                mKey = Keys::Unknown;
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
                mKey = Keys::Unknown;
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
                mKey = Keys::Unknown;
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
                mKey = Keys::Unknown;
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

class TreeDrawLegendSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, offset, width, title_style, title_size, text_style, text_size, interline};

 public:
    inline TreeDrawLegendSettingsHandler(Settings& aSettings, LegendSettings& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mKey) {
              case Keys::width:
                  mField.width = d;
                  break;
              case Keys::title_size:
                  mField.title_size = d;
                  break;
              case Keys::text_size:
                  mField.text_size = d;
                  break;
              case Keys::interline:
                  mField.interline = d;
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
              case Keys::title_style:
                  result = new SettingsTextStyleHandler(mTarget, mField.title_style);
                  break;
              case Keys::text_style:
                  result = new SettingsTextStyleHandler(mTarget, mField.text_style);
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
              case Keys::offset:
                  result = new SettingsSizeHandler(mTarget, mField.offset);
                  break;
              default:
                  result = HandlerBase::StartArray();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    LegendSettings& mField;
    static const std::map<std::string, Keys> key_mapper;

}; // class TreeDrawLegendSettingsHandler

const std::map<std::string, TreeDrawLegendSettingsHandler::Keys> TreeDrawLegendSettingsHandler::key_mapper {
    {"offset", Keys::offset},
    {"width", Keys::width},
    {"title_style", Keys::title_style},
    {"title_size", Keys::title_size},
    {"text_style", Keys::text_style},
    {"text_size", Keys::text_size},
    {"interline", Keys::interline}
};

// ----------------------------------------------------------------------

class TreeDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, hide_isolated_before, hide_if_cumulative_edge_length_bigger_than,
                force_line_width, line_width, root_edge, line_color, label_style,
                name_offset, color_nodes, aa_transition, vaccines, legend};

 public:
    inline TreeDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::vaccines:
                      mTarget.tree_draw.vaccines.clear();
                      result = new json_reader::ListHandler<Settings, TreeDrawVaccineSettings, TreeDrawVaccineSettingsHandler>(mTarget, mTarget.tree_draw.vaccines);
                      break;
                  default:
                      break;
                }
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
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
              case Keys::legend:
                  result = new TreeDrawLegendSettingsHandler(mTarget, mTarget.tree_draw.legend);
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

}; // class TreeDrawSettingsHandler

const std::map<std::string, TreeDrawSettingsHandler::Keys> TreeDrawSettingsHandler::key_mapper {
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
    {"vaccines", Keys::vaccines},
    {"legend", Keys::legend}
};

// ----------------------------------------------------------------------

class SignaturePageDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, layout, top, bottom, left, right, tree_margin_right, mapped_antigens_margin_right, time_series_width, clades_width};

 public:
    inline SignaturePageDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::layout:
                  mTarget.signature_page.set_layot(std::string(str, length));
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
              case Keys::top:
                  mTarget.signature_page.top = d;
                  break;
              case Keys::bottom:
                  mTarget.signature_page.bottom = d;
                  break;
              case Keys::left:
                  mTarget.signature_page.left = d;
                  break;
              case Keys::right:
                  mTarget.signature_page.right = d;
                  break;
              case Keys::tree_margin_right:
                  mTarget.signature_page.tree_margin_right = d;
                  break;
              case Keys::mapped_antigens_margin_right:
                  mTarget.signature_page.mapped_antigens_margin_right = d;
                  break;
              case Keys::time_series_width:
                  mTarget.signature_page.time_series_width = d;
                  break;
              case Keys::clades_width:
                  mTarget.signature_page.clades_width = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    // inline virtual HandlerBase* Bool(bool b)
    //     {
    //         switch (mKey) {
    //           case Keys:::
    //               mTarget.signature_page. = b;
    //               break;
    //           default:
    //               HandlerBase::Bool(b);
    //               break;
    //         }
    //         return nullptr;
    //     }

    // inline virtual HandlerBase* StartArray()
    //     {
    //         HandlerBase* result = nullptr;
    //         switch (mKey) {
    //           case Keys::offset:
    //               result = new SettingsSizeHandler(mTarget, mTarget.signature_page.offset);
    //               break;
    //           default:
    //               result = HandlerBase::StartArray();
    //               break;
    //         }
    //         return result;
    //     }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class SignaturePageDrawSettingsHandler

const std::map<std::string, SignaturePageDrawSettingsHandler::Keys> SignaturePageDrawSettingsHandler::key_mapper {
    {"layout", Keys::layout},
    {"top", Keys::top},
    {"bottom", Keys::bottom},
    {"left", Keys::left},
    {"right", Keys::right},
    {"tree_margin_right", Keys::tree_margin_right},
    {"mapped_antigens_margin_right", Keys::mapped_antigens_margin_right},
    {"time_series_width", Keys::time_series_width},
    {"clades_width", Keys::clades_width},
};

// ----------------------------------------------------------------------

class TimeSeriesDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, begin, end, label_size, label_style, month_year_to_timeseries_gap, month_separator_color, month_separator_width, dash_width, dash_line_width};

 public:
    inline TimeSeriesDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::begin:
                  mTarget.time_series.begin.assign(str, length);
                  break;
              case Keys::end:
                  mTarget.time_series.end.assign(str, length);
                  break;
              case Keys::month_separator_color:
                  mTarget.time_series.month_separator_color.from_string(str, length);
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
                  mTarget.time_series.label_size = d;
                  break;
              case Keys::month_year_to_timeseries_gap:
                  mTarget.time_series.month_year_to_timeseries_gap = d;
                  break;
              case Keys::month_separator_width:
                  mTarget.time_series.month_separator_width = d;
                  break;
              case Keys::dash_width:
                  mTarget.time_series.dash_width = d;
                  break;
              case Keys::dash_line_width:
                  mTarget.time_series.dash_line_width = d;
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
                  result = new SettingsTextStyleHandler(mTarget, mTarget.time_series.label_style);
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

}; // class TimeSeriesDrawSettingsHandler

const std::map<std::string, TimeSeriesDrawSettingsHandler::Keys> TimeSeriesDrawSettingsHandler::key_mapper {
    {"begin", Keys::begin},
    {"end", Keys::end},
    {"label_size", Keys::label_size},
    {"label_style", Keys::label_style},
    {"month_year_to_timeseries_gap", Keys::month_year_to_timeseries_gap},
    {"month_separator_color", Keys::month_separator_color},
    {"month_separator_width", Keys::month_separator_width},
    {"dash_width", Keys::dash_width},
    {"dash_line_width", Keys::dash_line_width}
};

// ----------------------------------------------------------------------

class CladeDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, name, display_name, show, section_inclusion_tolerance,
                section_exclusion_tolerance, show_section_size_in_label, arrow_color,
                line_width, arrow_width, separator_color, separator_width, label_position,
                label_offset, label_color, label_size, label_style, label_rotation, slot};

 public:
    inline CladeDrawSettingsHandler(Settings& aSettings, CladeDrawSettings& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
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
              case Keys::display_name:
                  mField.display_name.assign(str, length);
                  break;
              case Keys::arrow_color:
                  mField.arrow_color.from_string(str, length);
                  break;
              case Keys::separator_color:
                  mField.separator_color.from_string(str, length);
                  break;
              case Keys::label_position:
                  mField.label_position.assign(str, length);
                  break;
              case Keys::label_color:
                  mField.label_color.from_string(str, length);
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
              case Keys::line_width:
                  mField.line_width = d;
                  break;
              case Keys::arrow_width:
                  mField.arrow_width = d;
                  break;
              case Keys::separator_width:
                  mField.separator_width = d;
                  break;
              case Keys::label_size:
                  mField.label_size = d;
                  break;
              case Keys::label_rotation:
                  mField.label_rotation = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* Int(int i)
        {
            switch (mKey) {
              case Keys::slot:
                  mField.slot = static_cast<size_t>(i);
                  break;
              default:
                  HandlerBase::Int(i);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* Uint(unsigned u)
        {
            switch (mKey) {
              case Keys::section_inclusion_tolerance:
                  mField.section_inclusion_tolerance = u;
                  break;
              case Keys::section_exclusion_tolerance:
                  mField.section_exclusion_tolerance = u;
                  break;
              case Keys::slot:
                  mField.slot = u;
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
                  mField.show = b;
                  break;
              case Keys::show_section_size_in_label:
                  mField.show_section_size_in_label = b;
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
                  result = new SettingsTextStyleHandler(mTarget, mField.label_style);
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
                  result = new SettingsSizeHandler(mTarget, mField.label_offset);
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
    CladeDrawSettings& mField;

}; // class CladeDrawSettingsHandler

const std::map<std::string, CladeDrawSettingsHandler::Keys> CladeDrawSettingsHandler::key_mapper {
    {"name", Keys::name},
    {"display_name", Keys::display_name},
    {"show", Keys::show},
    {"section_inclusion_tolerance", Keys::section_inclusion_tolerance},
    {"section_exclusion_tolerance", Keys::section_exclusion_tolerance},
    {"show_section_size_in_label", Keys::show_section_size_in_label},
    {"arrow_color", Keys::arrow_color},
    {"line_width", Keys::line_width},
    {"arrow_width", Keys::arrow_width},
    {"separator_color", Keys::separator_color},
    {"separator_width", Keys::separator_width},
    {"label_position", Keys::label_position},
    {"label_offset", Keys::label_offset},
    {"label_color", Keys::label_color},
    {"label_size", Keys::label_size},
    {"label_style", Keys::label_style},
    {"label_rotation", Keys::label_rotation},
    {"slot", Keys::slot}
};

// ----------------------------------------------------------------------

class CladesDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, clades, slot_width};

 public:
    inline CladesDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::clades:
                      mTarget.clades.clades.clear();
                      result = new json_reader::ListHandler<Settings, CladeDrawSettings, CladeDrawSettingsHandler>(mTarget, mTarget.clades.clades);
                      break;
                  default:
                      break;
                }
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mKey) {
              case Keys::slot_width:
                  mTarget.clades.slot_width = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class CladesDrawSettingsHandler

const std::map<std::string, CladesDrawSettingsHandler::Keys> CladesDrawSettingsHandler::key_mapper {
    {"clades", Keys::clades},
    {"slot_width", Keys::slot_width}
};

// ----------------------------------------------------------------------

class HzSectionHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, show, show_line, show_map, name, label};

 public:
    inline HzSectionHandler(Settings& aSettings, HzSection& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
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
              case Keys::label:
                  mField.label.assign(str, length);
                  break;
              default:
                  result = HandlerBase::String(str, length);
                  break;
            }
            return result;
        }

    inline virtual HandlerBase* Bool(bool b)
        {
            switch (mKey) {
              case Keys::show:
                  mField.show = b;
                  break;
              case Keys::show_line:
                  mField.show_line = b;
                  break;
              case Keys::show_map:
                  mField.show_map = b;
                  break;
              default:
                  HandlerBase::Bool(b);
                  break;
            }
            return nullptr;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;
    HzSection& mField;

}; // class HzSectionHandler

const std::map<std::string, HzSectionHandler::Keys> HzSectionHandler::key_mapper {
    {"show", Keys::show},
    {"show_line", Keys::show_line},
    {"show_map", Keys::show_map},
    {"name", Keys::name},
    {"label", Keys::label},
};

// ----------------------------------------------------------------------

class HzSectionsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, vertical_gap, line_color, line_width, ts_label_size, ts_label_style, ts_label_color, sections};

 public:
    inline HzSectionsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::sections:
                      mTarget.hz_sections.sections.clear();
                      result = new json_reader::ListHandler<Settings, HzSection, HzSectionHandler>(mTarget, mTarget.hz_sections.sections);
                      break;
                  default:
                      break;
                }
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::line_color:
                  mTarget.hz_sections.line_color.from_string(str, length);
                  break;
              case Keys::ts_label_color:
                  mTarget.hz_sections.ts_label_color.from_string(str, length);
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
              case Keys::vertical_gap:
                  mTarget.hz_sections.vertical_gap = d;
                  break;
              case Keys::line_width:
                  mTarget.hz_sections.line_width = d;
                  break;
              case Keys::ts_label_size:
                  mTarget.hz_sections.ts_label_size = d;
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
              case Keys::ts_label_style:
                  result = new SettingsTextStyleHandler(mTarget, mTarget.hz_sections.ts_label_style);
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

}; // class HzSectionsHandler

const std::map<std::string, HzSectionsHandler::Keys> HzSectionsHandler::key_mapper {
    {"vertical_gap", Keys::vertical_gap},
    {"line_color", Keys::line_color},
    {"line_width", Keys::line_width},
    {"ts_label_size", Keys::ts_label_size},
    {"ts_label_style", Keys::ts_label_style},
    {"ts_label_color", Keys::ts_label_color},
    {"sections", Keys::sections}
};

// ----------------------------------------------------------------------

class MappedAntigensDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, width, line_color, line_width, line_length};

 public:
    inline MappedAntigensDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::line_color:
                  mTarget.mapped_antigens.line_color.from_string(str, length);
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
              case Keys::line_width:
                  mTarget.mapped_antigens.line_width = d;
                  break;
              case Keys::width:
                  mTarget.mapped_antigens.width = d;
                  break;
              case Keys::line_length:
                  mTarget.mapped_antigens.line_length = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
        }

 private:
    Keys mKey;
    static const std::map<std::string, Keys> key_mapper;

}; // class MappedAntigensDrawSettingsHandler

const std::map<std::string, MappedAntigensDrawSettingsHandler::Keys> MappedAntigensDrawSettingsHandler::key_mapper {
    {"width", Keys::width},
    {"line_color", Keys::line_color},
    {"line_width", Keys::line_width},
    {"line_length", Keys::line_length},
};

// ----------------------------------------------------------------------

class ViewportHandler : public HandlerBase
{
 public:
    inline ViewportHandler(Settings& aSettings, Viewport& aField) : HandlerBase{aSettings}, mField(aField), mPos(0) {}

    inline virtual HandlerBase* StartArray()
        {
            if (mPos)
                throw json_reader::Failure{};
            return nullptr;
        }

    inline virtual HandlerBase* EndArray()
        {
            if (mPos != 3 && mPos != 4)
                throw json_reader::Failure{"Unexpected viewport list size: " + std::to_string(mPos)};
            throw json_reader::Pop();
        }

    inline virtual HandlerBase* EndObject() { throw json_reader::Failure(); }

    inline virtual HandlerBase* Double(double d)
        {
            switch (mPos) {
              case 0:
                  mField.origin.x = d;
                  break;
              case 1:
                  mField.origin.y = d;
                  break;
              case 2:
                  mField.size.width = mField.size.height = d;
                  break;
              case 3:
                  mField.size.height = d;
                  break;
              default:
                  throw json_reader::Failure{};
            }
            ++mPos;
            return nullptr;
        }

 private:
    Viewport& mField;
    size_t mPos;

}; // class ViewportHandler

// ----------------------------------------------------------------------

class MarkAntigenSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, show, name, scale, aspect, rotation, fill_color, outline_color, outline_width, label_line_color, label_line_width, label, label_color, label_offset, label_size};

 public:
    inline MarkAntigenSettingsHandler(Settings& aSettings, MarkAntigenSettings& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
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
              case Keys::label:
                  mField.label.assign(str, length);
                  break;
              case Keys::fill_color:
                  mField.fill_color.from_string(str, length);
                  break;
              case Keys::outline_color:
                  mField.outline_color.from_string(str, length);
                  break;
              case Keys::label_color:
                  mField.label_color.from_string(str, length);
                  break;
              case Keys::label_line_color:
                  mField.label_line_color.from_string(str, length);
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
              case Keys::scale:
                  mField.scale = d;
                  break;
              case Keys::aspect:
                  mField.aspect = d;
                  break;
              case Keys::rotation:
                  mField.rotation = d;
                  break;
              case Keys::label_size:
                  mField.label_size = d;
                  break;
              case Keys::outline_width:
                  mField.outline_width = d;
                  break;
              case Keys::label_line_width:
                  mField.label_line_width = d;
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
              case Keys::show:
                  mField.show = b;
                  break;
              default:
                  HandlerBase::Bool(b);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* StartArray()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::label_offset:
                  result = new SettingsSizeHandler(mTarget, mField.label_offset);
                  break;
              default:
                  result = HandlerBase::StartArray();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    MarkAntigenSettings& mField;
    static const std::map<std::string, Keys> key_mapper;

}; // class MarkAntigenSettingsHandler

const std::map<std::string, MarkAntigenSettingsHandler::Keys> MarkAntigenSettingsHandler::key_mapper {
    {"show", Keys::show},
    {"name", Keys::name},
    {"scale", Keys::scale},
    {"aspect", Keys::aspect},
    {"rotation", Keys::rotation},
    {"fill_color", Keys::fill_color},
    {"outline_color", Keys::outline_color},
    {"label_line_color", Keys::label_line_color},
    {"label", Keys::label},
    {"label_offset", Keys::label_offset},
    {"label_size", Keys::label_size},
    {"outline_width", Keys::outline_width},
    {"label_line_width", Keys::label_line_width},
    {"label_color", Keys::label_color},
};

// ----------------------------------------------------------------------

class AntigenicMapsDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, layout, width, columns, gap, transformation, viewport,
                background_color, border_width, border_color, grid_line_width, grid_line_color,
                serum_scale, reference_antigen_scale, test_antigen_scale, vaccine_antigen_scale, tracked_antigen_scale,
                serum_outline_width, reference_antigen_outline_width, test_antigen_outline_width, vaccine_antigen_outline_width,
                sequenced_antigen_outline_width, serum_outline_color,
                reference_antigen_outline_color, test_antigen_outline_color, test_antigen_fill_color,
                vaccine_antigen_outline_color, sequenced_antigen_outline_color, sequenced_antigen_fill_color,
                tracked_antigen_outline_width, tracked_antigen_outline_color, tracked_antigen_colored_by_clade, tracked_antigen_color,
                reassortant_rotation, egg_antigen_aspect,
                serum_circle_color, serum_circle_thickness, map_title_color, map_title_offset, map_title_size,
                mapped_antigens_section_line_color, mapped_antigens_section_line_width, mark_antigens,
                };

 public:
    inline AntigenicMapsDrawSettingsHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
                switch (mKey) {
                  case Keys::transformation:
                      mTarget.antigenic_maps.transformation.clear();
                      result = new json_reader::DoubleListHandler<Settings>(mTarget, mTarget.antigenic_maps.transformation, 4);
                      break;
                  case Keys::viewport:
                      result = new ViewportHandler(mTarget, mTarget.antigenic_maps.viewport);
                      break;
                  case Keys::mark_antigens:
                      mTarget.antigenic_maps.mark_antigens.clear();
                      result = new json_reader::ListHandler<Settings, MarkAntigenSettings, MarkAntigenSettingsHandler>(mTarget, mTarget.antigenic_maps.mark_antigens);
                      break;
                  default:
                      break;
                }
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::layout:
                  mTarget.antigenic_maps.layout.assign(str, length);
                  break;
              case Keys::border_color:
                  mTarget.antigenic_maps.border_color.from_string(str, length);
                  break;
              case Keys::grid_line_color:
                  mTarget.antigenic_maps.grid_line_color.from_string(str, length);
                  break;
              case Keys::background_color:
                  mTarget.antigenic_maps.background_color.from_string(str, length);
                  break;
              case Keys::serum_outline_color:
                  mTarget.antigenic_maps.serum_outline_color.from_string(str, length);
                  break;
              case Keys::reference_antigen_outline_color:
                  mTarget.antigenic_maps.reference_antigen_outline_color.from_string(str, length);
                  break;
              case Keys::test_antigen_outline_color:
                  mTarget.antigenic_maps.test_antigen_outline_color.from_string(str, length);
                  break;
              case Keys::test_antigen_fill_color:
                  mTarget.antigenic_maps.test_antigen_fill_color.from_string(str, length);
                  break;
              case Keys::tracked_antigen_outline_color:
                  mTarget.antigenic_maps.tracked_antigen_outline_color.from_string(str, length);
                  break;
              case Keys::vaccine_antigen_outline_color:
                  mTarget.antigenic_maps.vaccine_antigen_outline_color.from_string(str, length);
                  break;
              case Keys::sequenced_antigen_outline_color:
                  mTarget.antigenic_maps.sequenced_antigen_outline_color.from_string(str, length);
                  break;
              case Keys::sequenced_antigen_fill_color:
                  mTarget.antigenic_maps.sequenced_antigen_fill_color.from_string(str, length);
                  break;
              case Keys::serum_circle_color:
                  mTarget.antigenic_maps.serum_circle_color.from_string(str, length);
                  break;
              case Keys::map_title_color:
                  mTarget.antigenic_maps.map_title_color.from_string(str, length);
                  break;
              case Keys::mapped_antigens_section_line_color:
                  mTarget.antigenic_maps.mapped_antigens_section_line_color.from_string(str, length);
                  break;
              case Keys::tracked_antigen_color:
                  mTarget.antigenic_maps.tracked_antigen_color.from_string(str, length);
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
              case Keys::border_width:
                  mTarget.antigenic_maps.border_width = d;
                  break;
              case Keys::grid_line_width:
                  mTarget.antigenic_maps.grid_line_width = d;
                  break;
              case Keys::width:
                  mTarget.antigenic_maps.width = d;
                  break;
              case Keys::gap:
                  mTarget.antigenic_maps.gap = d;
                  break;
              case Keys::serum_scale:
                  mTarget.antigenic_maps.serum_scale = d;
                  break;
              case Keys::reference_antigen_scale:
                  mTarget.antigenic_maps.reference_antigen_scale = d;
                  break;
              case Keys::test_antigen_scale:
                  mTarget.antigenic_maps.test_antigen_scale = d;
                  break;
              case Keys::vaccine_antigen_scale:
                  mTarget.antigenic_maps.vaccine_antigen_scale = d;
                  break;
              case Keys::tracked_antigen_scale:
                  mTarget.antigenic_maps.tracked_antigen_scale = d;
                  break;
              case Keys::serum_outline_width:
                  mTarget.antigenic_maps.serum_outline_width = d;
                  break;
              case Keys::reference_antigen_outline_width:
                  mTarget.antigenic_maps.reference_antigen_outline_width = d;
                  break;
              case Keys::test_antigen_outline_width:
                  mTarget.antigenic_maps.test_antigen_outline_width = d;
                  break;
              case Keys::vaccine_antigen_outline_width:
                  mTarget.antigenic_maps.vaccine_antigen_outline_width = d;
                  break;
              case Keys::sequenced_antigen_outline_width:
                  mTarget.antigenic_maps.sequenced_antigen_outline_width = d;
                  break;
              case Keys::tracked_antigen_outline_width:
                  mTarget.antigenic_maps.tracked_antigen_outline_width = d;
                  break;
              case Keys::reassortant_rotation:
                  mTarget.antigenic_maps.reassortant_rotation = d;
                  break;
              case Keys::egg_antigen_aspect:
                  mTarget.antigenic_maps.egg_antigen_aspect = d;
                  break;
              case Keys::serum_circle_thickness:
                  mTarget.antigenic_maps.serum_circle_thickness = d;
                  break;
              case Keys::map_title_size:
                  mTarget.antigenic_maps.map_title_size = d;
                  break;
              case Keys::mapped_antigens_section_line_width:
                  mTarget.antigenic_maps.mapped_antigens_section_line_width = d;
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
              case Keys::columns:
                  mTarget.antigenic_maps.columns = u;
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
              case Keys::tracked_antigen_colored_by_clade:
                  mTarget.antigenic_maps.tracked_antigen_colored_by_clade = b;
                  break;
              default:
                  HandlerBase::Bool(b);
                  break;
            }
            return nullptr;
        }

    inline virtual HandlerBase* StartArray()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::map_title_offset:
                  result = new SettingsSizeHandler(mTarget, mTarget.antigenic_maps.map_title_offset);
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

}; // class AntigenicMapsDrawSettingsHandler

const std::map<std::string, AntigenicMapsDrawSettingsHandler::Keys> AntigenicMapsDrawSettingsHandler::key_mapper {
    {"layout", Keys::layout},
    {"width", Keys::width},
    {"columns", Keys::columns},
    {"gap", Keys::gap},
    {"transformation", Keys::transformation},
    {"viewport", Keys::viewport},
    {"background_color", Keys::background_color},
    {"border_width", Keys::border_width},
    {"border_color", Keys::border_color},
    {"grid_line_width", Keys::grid_line_width},
    {"grid_line_color", Keys::grid_line_color},
    {"serum_scale", Keys::serum_scale},
    {"reference_antigen_scale", Keys::reference_antigen_scale},
    {"test_antigen_scale", Keys::test_antigen_scale},
    {"vaccine_antigen_scale", Keys::vaccine_antigen_scale},
    {"tracked_antigen_scale", Keys::tracked_antigen_scale},
    {"serum_outline_width", Keys::serum_outline_width},
    {"reference_antigen_outline_width", Keys::reference_antigen_outline_width},
    {"test_antigen_outline_width", Keys::test_antigen_outline_width},
    {"vaccine_antigen_outline_width", Keys::vaccine_antigen_outline_width},
    {"sequenced_antigen_outline_width", Keys::sequenced_antigen_outline_width},
    {"tracked_antigen_outline_width", Keys::tracked_antigen_outline_width},
    {"serum_outline_color", Keys::serum_outline_color},
    {"reference_antigen_outline_color", Keys::reference_antigen_outline_color},
    {"test_antigen_outline_color", Keys::test_antigen_outline_color},
    {"test_antigen_fill_color", Keys::test_antigen_fill_color},
    {"vaccine_antigen_outline_color", Keys::vaccine_antigen_outline_color},
    {"sequenced_antigen_outline_color", Keys::sequenced_antigen_outline_color},
    {"sequenced_antigen_fill_color", Keys::sequenced_antigen_fill_color},
    {"tracked_antigen_outline_color", Keys::tracked_antigen_outline_color},
    {"tracked_antigen_colored_by_clade", Keys::tracked_antigen_colored_by_clade},
    {"tracked_antigen_color", Keys::tracked_antigen_color},
    {"reassortant_rotation", Keys::reassortant_rotation},
    {"egg_antigen_aspect", Keys::egg_antigen_aspect},
    {"serum_circle_color", Keys::serum_circle_color},
    {"serum_circle_thickness", Keys::serum_circle_thickness},
    {"map_title_color", Keys::map_title_color},
    {"map_title_offset", Keys::map_title_offset},
    {"map_title_size", Keys::map_title_size},
    {"mapped_antigens_section_line_color", Keys::mapped_antigens_section_line_color},
    {"mapped_antigens_section_line_width", Keys::mapped_antigens_section_line_width},
    {"mark_antigens", Keys::mark_antigens},
};

// ----------------------------------------------------------------------

class SettingsRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, version, signature_page, tree, time_series, clades, hz_sections, mapped_antigens, antigenic_maps };

 public:
    inline SettingsRootHandler(Settings& aSettings) : HandlerBase{aSettings}, mKey(Keys::Unknown) {}

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            try {
                mKey = key_mapper.at(std::string(str, length));
            }
            catch (std::out_of_range&) {
                mKey = Keys::Unknown;
                result = HandlerBase::Key(str, length);
            }
            return result;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::version:
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
                  case Keys::signature_page:
                      result = new SignaturePageDrawSettingsHandler(mTarget);
                      break;
                  case Keys::tree:
                      result = new TreeDrawSettingsHandler(mTarget);
                      break;
                  case Keys::time_series:
                      result = new TimeSeriesDrawSettingsHandler(mTarget);
                      break;
                  case Keys::clades:
                      result = new CladesDrawSettingsHandler(mTarget);
                      break;
                  case Keys::hz_sections:
                      result = new HzSectionsHandler(mTarget);
                      break;
                  case Keys::mapped_antigens:
                      result = new MappedAntigensDrawSettingsHandler(mTarget);
                      break;
                  case Keys::antigenic_maps:
                      result = new AntigenicMapsDrawSettingsHandler(mTarget);
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
};

const std::map<std::string, SettingsRootHandler::Keys> SettingsRootHandler::key_mapper {
    {"  version", Keys::version},
    {"signature_page", Keys::signature_page},
    {"tree", Keys::tree},
    {"time_series", Keys::time_series},
    {"clades", Keys::clades},
    {"hz_sections", Keys::hz_sections},
    {"mapped_antigens", Keys::mapped_antigens},
    {"antigenic_maps", Keys::antigenic_maps},
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

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const LegendSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("offset") << aSettings.offset
                  << JsonObjectKey("width") << aSettings.width
                  << JsonObjectKey("title_style") << aSettings.title_style
                  << JsonObjectKey("title_size") << aSettings.title_size
                  << JsonObjectKey("text_style") << aSettings.text_style
                  << JsonObjectKey("text_size") << aSettings.text_size
                  << JsonObjectKey("interline") << aSettings.interline
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
                  << JsonObjectKey("legend") << aSettings.legend
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const SignaturePageDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("layout") << aSettings.layout_to_string()
                  << JsonObjectKey("top") << aSettings.top
                  << JsonObjectKey("bottom") << aSettings.bottom
                  << JsonObjectKey("left") << aSettings.left
                  << JsonObjectKey("right") << aSettings.right
                  << JsonObjectKey("tree_margin_right") << aSettings.tree_margin_right
                  << JsonObjectKey("mapped_antigens_margin_right") << aSettings.mapped_antigens_margin_right
                  << JsonObjectKey("time_series_width") << aSettings.time_series_width
                  << JsonObjectKey("clades_width") << aSettings.clades_width
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const TimeSeriesDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("begin") << aSettings.begin
                  << JsonObjectKey("end") << aSettings.end
                  << JsonObjectKey("label_size") << aSettings.label_size
                  << JsonObjectKey("label_style") << aSettings.label_style
                  << JsonObjectKey("month_year_to_timeseries_gap") << aSettings.month_year_to_timeseries_gap
                  << JsonObjectKey("month_separator_color") << aSettings.month_separator_color
                  << JsonObjectKey("month_separator_width") << aSettings.month_separator_width
                  << JsonObjectKey("dash_width") << aSettings.dash_width
                  << JsonObjectKey("dash_line_width") << aSettings.dash_line_width
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const CladeDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("name") << aSettings.name
                  << JsonObjectKey("display_name") << aSettings.display_name
                  << JsonObjectKey("show") << aSettings.show
                  << JsonObjectKey("slot") << static_cast<int>(aSettings.slot)
                  << JsonObjectKey("section_inclusion_tolerance") << aSettings.section_inclusion_tolerance
                  << JsonObjectKey("section_exclusion_tolerance") << aSettings.section_exclusion_tolerance
                  << JsonObjectKey("show_section_size_in_label") << aSettings.show_section_size_in_label
                  << JsonObjectKey("arrow_color") << aSettings.arrow_color
                  << JsonObjectKey("line_width") << aSettings.line_width
                  << JsonObjectKey("arrow_width") << aSettings.arrow_width
                  << JsonObjectKey("separator_color") << aSettings.separator_color
                  << JsonObjectKey("separator_width") << aSettings.separator_width
                  << JsonObjectKey("label_position") << aSettings.label_position
                  << JsonObjectKey("label_offset") << aSettings.label_offset
                  << JsonObjectKey("label_color") << aSettings.label_color
                  << JsonObjectKey("label_size") << aSettings.label_size
                  << JsonObjectKey("label_style") << aSettings.label_style
                  << JsonObjectKey("label_rotation") << aSettings.label_rotation
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const CladesDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("slot_width") << aSettings.slot_width
                  << JsonObjectKey("clades") << aSettings.clades
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const HzSection& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("name") << aSettings.name
                  << JsonObjectKey("show") << aSettings.show
                  << JsonObjectKey("show_line") << aSettings.show
                  << JsonObjectKey("show_map") << aSettings.show_map
                  << JsonObjectKey("label") << aSettings.label
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const HzSections& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("vertical_gap") << aSettings.vertical_gap
                  << JsonObjectKey("line_color") << aSettings.line_color
                  << JsonObjectKey("line_width") << aSettings.line_width
                  << JsonObjectKey("ts_label_size") << aSettings.ts_label_size
                  << JsonObjectKey("ts_label_style") << aSettings.ts_label_style
                  << JsonObjectKey("ts_label_color") << aSettings.ts_label_color
                  << JsonObjectKey("sections") << aSettings.sections
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const MappedAntigensDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("width") << aSettings.width
                  << JsonObjectKey("line_color") << aSettings.line_color
                  << JsonObjectKey("line_width") << aSettings.line_width
                  << JsonObjectKey("line_length") << aSettings.line_length
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const Viewport& aViewport)
{
    writer << StartArray << aViewport.origin.x << aViewport.origin.y << aViewport.size.width;
    if (!float_equal(aViewport.size.width, aViewport.size.height))
        writer << aViewport.size.height;
    writer << EndArray;
    return writer;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const MarkAntigenSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("show") << aSettings.show
                  << JsonObjectKey("name") << aSettings.name
                  << JsonObjectKey("scale") << aSettings.scale
                  << JsonObjectKey("aspect") << aSettings.aspect
                  << JsonObjectKey("rotation") << aSettings.rotation
                  << JsonObjectKey("fill_color") << aSettings.fill_color
                  << JsonObjectKey("outline_color") << aSettings.outline_color
                  << JsonObjectKey("outline_width") << aSettings.outline_width
                  << JsonObjectKey("label") << aSettings.label
                  << JsonObjectKey("label_color") << aSettings.label_color
                  << JsonObjectKey("label_offset") << aSettings.label_offset
                  << JsonObjectKey("label_size") << aSettings.label_size
                  << JsonObjectKey("label_line_color") << aSettings.label_line_color
                  << JsonObjectKey("label_line_width") << aSettings.label_line_width
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const AntigenicMapsDrawSettings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("layout") << aSettings.layout
                  << JsonObjectKey("width") << aSettings.width
                  << JsonObjectKey("columns") << aSettings.columns
                  << JsonObjectKey("gap") << aSettings.gap
                  << JsonObjectKey("transformation") << aSettings.transformation
                  << JsonObjectKey("viewport") << aSettings.viewport
                  << JsonObjectKey("background_color") << aSettings.background_color
                  << JsonObjectKey("border_color") << aSettings.border_color
                  << JsonObjectKey("border_width") << aSettings.border_width
                  << JsonObjectKey("grid_line_width") << aSettings.grid_line_width
                  << JsonObjectKey("grid_line_color") << aSettings.grid_line_color
                  << JsonObjectKey("serum_scale") << aSettings.serum_scale
                  << JsonObjectKey("reference_antigen_scale") << aSettings.reference_antigen_scale
                  << JsonObjectKey("test_antigen_scale") << aSettings.test_antigen_scale
                  << JsonObjectKey("vaccine_antigen_scale") << aSettings.vaccine_antigen_scale
                  << JsonObjectKey("tracked_antigen_scale") << aSettings.tracked_antigen_scale
                  << JsonObjectKey("serum_outline_width") << aSettings.serum_outline_width
                  << JsonObjectKey("reference_antigen_outline_width") << aSettings.reference_antigen_outline_width
                  << JsonObjectKey("test_antigen_outline_width") << aSettings.test_antigen_outline_width
                  << JsonObjectKey("vaccine_antigen_outline_width") << aSettings.vaccine_antigen_outline_width
                  << JsonObjectKey("sequenced_antigen_outline_width") << aSettings.sequenced_antigen_outline_width
                  << JsonObjectKey("serum_outline_color") << aSettings.serum_outline_color
                  << JsonObjectKey("reference_antigen_outline_color") << aSettings.reference_antigen_outline_color
                  << JsonObjectKey("test_antigen_outline_color") << aSettings.test_antigen_outline_color
                  << JsonObjectKey("test_antigen_fill_color") << aSettings.test_antigen_fill_color
                  << JsonObjectKey("vaccine_antigen_outline_color") << aSettings.vaccine_antigen_outline_color
                  << JsonObjectKey("sequenced_antigen_outline_color") << aSettings.sequenced_antigen_outline_color
                  << JsonObjectKey("sequenced_antigen_fill_color") << aSettings.sequenced_antigen_fill_color
                  << JsonObjectKey("tracked_antigen_outline_width") << aSettings.tracked_antigen_outline_width
                  << JsonObjectKey("tracked_antigen_outline_color") << aSettings.tracked_antigen_outline_color
                  << JsonObjectKey("tracked_antigen_colored_by_clade") << aSettings.tracked_antigen_colored_by_clade
                  << JsonObjectKey("tracked_antigen_color") << aSettings.tracked_antigen_color
                  << JsonObjectKey("reassortant_rotation") << aSettings.reassortant_rotation
                  << JsonObjectKey("egg_antigen_aspect") << aSettings.egg_antigen_aspect
                  << JsonObjectKey("serum_circle_color") << aSettings.serum_circle_color
                  << JsonObjectKey("serum_circle_thickness") << aSettings.serum_circle_thickness
                  << JsonObjectKey("map_title_color") << aSettings.map_title_color
                  << JsonObjectKey("map_title_offset") << aSettings.map_title_offset
                  << JsonObjectKey("map_title_size") << aSettings.map_title_size
                  << JsonObjectKey("mapped_antigens_section_line_color") << aSettings.mapped_antigens_section_line_color
                  << JsonObjectKey("mapped_antigens_section_line_width") << aSettings.mapped_antigens_section_line_width
                  << JsonObjectKey("mark_antigens") << aSettings.mark_antigens
                  << EndObject;
}

// ----------------------------------------------------------------------

template <typename RW> inline JsonWriterT<RW>& operator <<(JsonWriterT<RW>& writer, const Settings& aSettings)
{
    return writer << StartObject
                  << JsonObjectKey("  version") << SETTINGS_VERSION
                  << JsonObjectKey("signature_page") << aSettings.signature_page
                  << JsonObjectKey("tree") << aSettings.tree_draw
                  << JsonObjectKey("time_series") << aSettings.time_series
                  << JsonObjectKey("clades") << aSettings.clades
                  << JsonObjectKey("hz_sections") << aSettings.hz_sections
                  << JsonObjectKey("mapped_antigens") << aSettings.mapped_antigens
                  << JsonObjectKey("antigenic_maps") << aSettings.antigenic_maps
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
