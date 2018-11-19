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
        return val.empty() || (a_dive == dive::yes && val.all_of([this](const auto& kv) -> bool { return kv.first == "label_offset" ? true : is_simple(kv.second, dive::no); }));
    }

    std::vector<rjson::object::content_t::const_iterator> sorted(const rjson::object& val) const override
    {
        auto result = rjson::PrettyHandler::sorted(val);
        if (!result.empty()) {
            if (result.front()->first == "first_leaf_seq_id")
                std::sort(result.begin(), result.end(), [](const auto& e1, const auto& e2) -> bool { return e1->first > e2->first; });
        }
        return result;
    }
};

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
