#include <cmath>

#include "acmacs-base/debug.hh"
#include "acmacs-base/float.hh"
#include "acmacs-base/read-file.hh"

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

void write_settings(const Settings& aSettings, std::string aFilename)
{
    std::cout << "INFO: writing settings to " << aFilename << '\n';
    aSettings.write_to_file(aFilename);

} // write_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
