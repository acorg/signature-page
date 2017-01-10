#include "settings.hh"
#include "acmacs-base/float.hh"

#include "acmacs-base/json-reader.hh"
#include "acmacs-base/json-writer.hh"
namespace jsw = json_writer;

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
    enum class Keys {Unknown, show, show_line, show_label_in_time_series, show_map, name, label};

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
              case Keys::show_label_in_time_series:
                  mField.show_label_in_time_series = b;
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
    {"show_label_in_time_series", Keys::show_label_in_time_series},
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
                show_tracked_sera, serum_circle_color, tracked_serum_outline_color, serum_circle_thickness, tracked_serum_outline_width,
                map_title_color, map_title_offset, map_title_size,
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
              case Keys::tracked_serum_outline_color:
                  mTarget.antigenic_maps.tracked_serum_outline_color.from_string(str, length);
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
              case Keys::tracked_serum_outline_width:
                  mTarget.antigenic_maps.tracked_serum_outline_width = d;
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
              case Keys::show_tracked_sera:
                  mTarget.antigenic_maps.show_tracked_sera = b;
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
    {"show_tracked_sera", Keys::show_tracked_sera},
    {"serum_circle_color", Keys::serum_circle_color},
    {"tracked_serum_outline_color", Keys::tracked_serum_outline_color},
    {"serum_circle_thickness", Keys::serum_circle_thickness},
    {"tracked_serum_outline_width", Keys::tracked_serum_outline_width},
    {"map_title_color", Keys::map_title_color},
    {"map_title_offset", Keys::map_title_offset},
    {"map_title_size", Keys::map_title_size},
    {"mapped_antigens_section_line_color", Keys::mapped_antigens_section_line_color},
    {"mapped_antigens_section_line_width", Keys::mapped_antigens_section_line_width},
    {"mark_antigens", Keys::mark_antigens},
};

// ----------------------------------------------------------------------

class TitleDrawSettingsHandler : public HandlerBase
{
 private:
    enum class Keys {Unknown, title, color, size, style, offset};

 public:
    inline TitleDrawSettingsHandler(Settings& aSettings, TitleDrawSettings& aField) : HandlerBase{aSettings}, mKey(Keys::Unknown), mField(aField) {}

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
              case Keys::title:
                  mField.title.assign(str, length);
                  break;
              case Keys::color:
                  mField.color.from_string(str, length);
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
                  mField.size = d;
                  break;
              default:
                  HandlerBase::Double(d);
                  break;
            }
            return nullptr;
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

    inline virtual HandlerBase* StartObject()
        {
            HandlerBase* result = nullptr;
            switch (mKey) {
              case Keys::style:
                  result = new SettingsTextStyleHandler(mTarget, mField.style);
                  break;
              default:
                  result = HandlerBase::StartObject();
                  break;
            }
            return result;
        }

 private:
    Keys mKey;
    TitleDrawSettings& mField;
    static const std::map<std::string, Keys> key_mapper;

}; // class TitleDrawSettingsHandler

const std::map<std::string, TitleDrawSettingsHandler::Keys> TitleDrawSettingsHandler::key_mapper {
    {"title", Keys::title},
    {"color", Keys::color},
    {"size", Keys::size},
    {"style", Keys::style},
    {"offset", Keys::offset}
};

// ----------------------------------------------------------------------

class SettingsRootHandler : public HandlerBase
{
 private:
    enum class Keys { Unknown, version, signature_page, tree, time_series, clades, hz_sections, mapped_antigens, antigenic_maps, title };

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
                  case Keys::title:
                      result = new TitleDrawSettingsHandler(mTarget, mTarget.title);
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
    {"title", Keys::title},
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
                  << jsw::key("number_strains_threshold?") << "do not show aa transition label if number_strains (leaf nodes) for the branch is less than this value"
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
                  << jsw::key("name?") << "empty for default settings"
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

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TreeDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("hide_isolated_before") << aSettings.hide_isolated_before
                  << jsw::key("hide_if_cumulative_edge_length_bigger_than") << aSettings.hide_if_cumulative_edge_length_bigger_than
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
                  << jsw::key("layout") << aSettings.layout_to_string()
                  << jsw::key("top") << aSettings.top
                  << jsw::key("bottom") << aSettings.bottom
                  << jsw::key("left") << aSettings.left
                  << jsw::key("right") << aSettings.right
                  << jsw::key("tree_margin_right") << aSettings.tree_margin_right
                  << jsw::key("mapped_antigens_margin_right") << aSettings.mapped_antigens_margin_right
                  << jsw::key("time_series_width") << aSettings.time_series_width
                  << jsw::key("clades_width") << aSettings.clades_width
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
                  << jsw::key("name") << aSettings.name
                  << jsw::key("show") << aSettings.show
                  << jsw::key("show_line") << aSettings.show_line
                  << jsw::key("show_label_in_time_series") << aSettings.show_label_in_time_series
                  << jsw::key("show_map") << aSettings.show_map
                  << jsw::key("label") << aSettings.label
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const HzSections& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("vertical_gap") << aSettings.vertical_gap
                  << jsw::key("line_color") << aSettings.line_color
                  << jsw::key("line_width") << aSettings.line_width
                  << jsw::key("ts_label_size") << aSettings.ts_label_size
                  << jsw::key("ts_label_style") << aSettings.ts_label_style
                  << jsw::key("ts_label_color") << aSettings.ts_label_color
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

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const MarkAntigenSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("show") << aSettings.show
                  << jsw::key("name") << aSettings.name
                  << jsw::key("scale") << aSettings.scale
                  << jsw::key("aspect") << aSettings.aspect
                  << jsw::key("rotation") << aSettings.rotation
                  << jsw::key("fill_color") << aSettings.fill_color
                  << jsw::key("outline_color") << aSettings.outline_color
                  << jsw::key("outline_width") << aSettings.outline_width
                  << jsw::key("label") << aSettings.label
                  << jsw::key("label_color") << aSettings.label_color
                  << jsw::key("label_offset") << aSettings.label_offset
                  << jsw::key("label_size") << aSettings.label_size
                  << jsw::key("label_line_color") << aSettings.label_line_color
                  << jsw::key("label_line_width") << aSettings.label_line_width
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const AntigenicMapsDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("layout") << aSettings.layout
                  << jsw::key("width") << aSettings.width
                  << jsw::key("columns") << aSettings.columns
                  << jsw::key("gap") << aSettings.gap
                  << jsw::key("transformation") << aSettings.transformation
                  << jsw::key("viewport") << aSettings.viewport
                  << jsw::key("background_color") << aSettings.background_color
                  << jsw::key("border_color") << aSettings.border_color
                  << jsw::key("border_width") << aSettings.border_width
                  << jsw::key("grid_line_width") << aSettings.grid_line_width
                  << jsw::key("grid_line_color") << aSettings.grid_line_color
                  << jsw::key("serum_scale") << aSettings.serum_scale
                  << jsw::key("reference_antigen_scale") << aSettings.reference_antigen_scale
                  << jsw::key("test_antigen_scale") << aSettings.test_antigen_scale
                  << jsw::key("vaccine_antigen_scale") << aSettings.vaccine_antigen_scale
                  << jsw::key("tracked_antigen_scale") << aSettings.tracked_antigen_scale
                  << jsw::key("serum_outline_width") << aSettings.serum_outline_width
                  << jsw::key("reference_antigen_outline_width") << aSettings.reference_antigen_outline_width
                  << jsw::key("test_antigen_outline_width") << aSettings.test_antigen_outline_width
                  << jsw::key("vaccine_antigen_outline_width") << aSettings.vaccine_antigen_outline_width
                  << jsw::key("sequenced_antigen_outline_width") << aSettings.sequenced_antigen_outline_width
                  << jsw::key("serum_outline_color") << aSettings.serum_outline_color
                  << jsw::key("reference_antigen_outline_color") << aSettings.reference_antigen_outline_color
                  << jsw::key("test_antigen_outline_color") << aSettings.test_antigen_outline_color
                  << jsw::key("test_antigen_fill_color") << aSettings.test_antigen_fill_color
                  << jsw::key("vaccine_antigen_outline_color") << aSettings.vaccine_antigen_outline_color
                  << jsw::key("sequenced_antigen_outline_color") << aSettings.sequenced_antigen_outline_color
                  << jsw::key("sequenced_antigen_fill_color") << aSettings.sequenced_antigen_fill_color
                  << jsw::key("tracked_antigen_outline_width") << aSettings.tracked_antigen_outline_width
                  << jsw::key("tracked_antigen_outline_color") << aSettings.tracked_antigen_outline_color
                  << jsw::key("tracked_antigen_colored_by_clade") << aSettings.tracked_antigen_colored_by_clade
                  << jsw::key("tracked_antigen_color") << aSettings.tracked_antigen_color
                  << jsw::key("reassortant_rotation") << aSettings.reassortant_rotation
                  << jsw::key("egg_antigen_aspect") << aSettings.egg_antigen_aspect
                  << jsw::key("serum_circle_color") << aSettings.serum_circle_color
                  << jsw::key("tracked_serum_outline_color") << aSettings.tracked_serum_outline_color
                  << jsw::key("serum_circle_thickness") << aSettings.serum_circle_thickness
                  << jsw::key("tracked_serum_outline_width") << aSettings.tracked_serum_outline_width
                  << jsw::key("map_title_color") << aSettings.map_title_color
                  << jsw::key("map_title_offset") << aSettings.map_title_offset
                  << jsw::key("map_title_size") << aSettings.map_title_size
                  << jsw::key("mapped_antigens_section_line_color") << aSettings.mapped_antigens_section_line_color
                  << jsw::key("mapped_antigens_section_line_width") << aSettings.mapped_antigens_section_line_width
                  << jsw::key("mark_antigens") << aSettings.mark_antigens
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const TitleDrawSettings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("title") << aSettings.title
                  << jsw::key("color") << aSettings.color
                  << jsw::key("size") << aSettings.size
                  << jsw::key("style") << aSettings.style
                  << jsw::key("offset") << aSettings.offset
                  << jsw::end_object;
}

// ----------------------------------------------------------------------

template <typename RW> inline jsw::writer<RW>& operator <<(jsw::writer<RW>& writer, const Settings& aSettings)
{
    return writer << jsw::start_object
                  << jsw::key("  version") << SETTINGS_VERSION
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
    jsw::export_to_json(aSettings, SETTINGS_VERSION, aFilename, aIndent);

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
