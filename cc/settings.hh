#pragma once

#include "signature-page.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "clades-draw.hh"
#include "mapped-antigens-draw.hh"
#include "aa-at-pos-draw.hh"
#include "antigenic-maps-draw.hh"
#include "title-draw.hh"

// ----------------------------------------------------------------------

class SettingsMod : public acmacs::settings::object
{
 public:
    using acmacs::settings::object::object;

    acmacs::settings::field<std::string>       name{this, "N"};
    acmacs::settings::field<std::string>       name_commented{this, "?N"};
    acmacs::settings::field<acmacs::Offset>    offset{this, "offset"};
    acmacs::settings::field<double>            size{this, "size"};
    acmacs::settings::field<Color>             color{this, "color"};
    acmacs::settings::field<acmacs::TextStyle> style{this, "style"};
    acmacs::settings::field<std::string>       text{this, "text"};
};

// ----------------------------------------------------------------------

constexpr const char* SETTINGS_VERSION_4 = "signature-page-settings-v4";

class Settings : public acmacs::settings::toplevel
{
 public:
    Settings();

    acmacs::settings::field<std::string>                       version{this, "  version", SETTINGS_VERSION_4};
    acmacs::settings::field_array_of<SettingsMod>              mods{this, "mods"};
    acmacs::settings::field_object<SignaturePageDrawSettings>  signature_page{this, "signature_page"};
    acmacs::settings::field_object<TitleDrawSettings>          title{this, "title"};
    acmacs::settings::field_object<TreeDrawSettings>           tree_draw{this, "tree"};
    acmacs::settings::field_object<TimeSeriesDrawSettings>     time_series{this, "time_series"};
    acmacs::settings::field_object<CladesDrawSettings>         clades{this, "clades"};
    acmacs::settings::field_object<HzSections>                 hz_sections{this, "hz_sections"};
    acmacs::settings::field_object<MappedAntigensDrawSettings> mapped_antigens{this, "mapped_antigens"};
    acmacs::settings::field_object<AAAtPosDrawSettings>        aa_at_pos{this, "aa_at_pos"};
    acmacs::settings::field_object<AntigenicMapsDrawSettings>  antigenic_maps{this, "antigenic_maps"};

    void upgrade();             // upgrade to the new version in case old version data provided

}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename);
void write_settings(const Settings& aSettings, std::string aFilename);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
