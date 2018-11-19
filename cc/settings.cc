#include <cmath>

#include "acmacs-base/debug.hh"
#include "acmacs-base/float.hh"
#include "acmacs-base/read-file.hh"
#include "acmacs-base/date.hh"

#include "settings.hh"
using namespace std::string_literals;

// ----------------------------------------------------------------------

Settings::Settings()
{
    auto mod = mods.append();
    mod->name_commented = "text";
    mod->color = BLACK;
    mod->offset = acmacs::Offset{100.0, 100.0};
    mod->size = 12.0;
    mod->style = acmacs::TextStyle{};
    mod->text = "Text to show";

} // Settings::Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: reading settings from " << aFilename << '\n';
    aSettings.update_from_file(aFilename);
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

class PrettyHandlerSigpSettings : public rjson::PrettyHandler
{
 public:
    PrettyHandlerSigpSettings() = default;

    using rjson::PrettyHandler::is_simple;

    bool is_simple(const rjson::object& val, dive a_dive) const override
    {
        return val.empty() || (a_dive == dive::yes && val.all_of([this](const auto& kv) -> bool {
                   return std::find(simple_keys_.begin(), simple_keys_.end(), kv.first) != simple_keys_.end() ? true : is_simple(kv.second, dive::no);
               }));
    }

    std::vector<rjson::object::content_t::const_iterator> sorted(const rjson::object& val) const override
    {
        auto result = rjson::PrettyHandler::sorted(val);
        std::sort(result.begin(), result.end(), [](const auto& e1, const auto& e2) -> bool { return replace_key(e1->first) < replace_key(e2->first); });
        return result;
    }

 private:
    static const std::map<std::string, std::string> top_level_keys_for_sorting_;
    static std::vector<std::string> simple_keys_;

    static std::string replace_key(std::string key)
        {
            if (auto found = top_level_keys_for_sorting_.find(key); found != top_level_keys_for_sorting_.end())
                return found->second;
            else
                return key;
        }
};

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

const std::map<std::string, std::string> PrettyHandlerSigpSettings::top_level_keys_for_sorting_ = {
    // top level
    {"signature_page", "0 signature_page"},
    {"time_series", "1 time_series"},
    {"title", "2 title"},
    {"tree", "3 tree"},

    // by_aa_label
    {"label_offset", "0 label_offset"},
    {"label", "1 label"},

    // time series
    {"begin", "0 begin"},
    {"end", "1 end"},
};

std::vector<std::string> PrettyHandlerSigpSettings::simple_keys_ = {
    "label_offset",
    "offset",
    "triggering_clades"
};

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------

void write_settings(const Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: writing settings to " << aFilename << '\n';
    aSettings.timestamp = current_date_time();
    aSettings.write_to_file(aFilename, rjson::emacs_indent::yes, PrettyHandlerSigpSettings{});

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
