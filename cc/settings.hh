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

class SettingsMod : public acmacs::settings::v1::object
{
 public:
    using acmacs::settings::v1::object::object;

    acmacs::settings::v1::field<std::string>       name{this, "N"};
    acmacs::settings::v1::field<std::string>       name_commented{this, "?N"};
    acmacs::settings::v1::field<acmacs::Offset>    offset{this, "offset"};
    acmacs::settings::v1::field<double>            size{this, "size"};
    acmacs::settings::v1::field<Color>             color{this, "color"};
    acmacs::settings::v1::field<acmacs::TextStyle> style{this, "style"};
    acmacs::settings::v1::field<std::string>       text{this, "text"};
};

// ----------------------------------------------------------------------

constexpr const char* SETTINGS_VERSION_4 = "signature-page-settings-v4";

class Settings : public acmacs::settings::v1::toplevel
{
 public:
    Settings();

    acmacs::settings::v1::field<std::string>                       version{this, "  version", SETTINGS_VERSION_4};
    mutable acmacs::settings::v1::field<std::string>               timestamp{this, " timestamp"};
    acmacs::settings::v1::field_array_of<SettingsMod>              mods{this, "mods"};
    acmacs::settings::v1::field_object<SignaturePageDrawSettings>  signature_page{this, "signature_page"};
    acmacs::settings::v1::field_object<TitleDrawSettings>          title{this, "title"};
    acmacs::settings::v1::field_object<TreeDrawSettings>           tree_draw{this, "tree"};
    acmacs::settings::v1::field_object<TimeSeriesDrawSettings>     time_series{this, "time_series"};
    acmacs::settings::v1::field_object<CladesDrawSettings>         clades{this, "clades"};
    acmacs::settings::v1::field_object<HzSections>                 hz_sections{this, "hz_sections"};
    acmacs::settings::v1::field_object<MappedAntigensDrawSettings> mapped_antigens{this, "mapped_antigens"};
    acmacs::settings::v1::field_object<AAAtPosDrawSettings>        aa_at_pos{this, "aa_at_pos"};
    acmacs::settings::v1::field_object<AntigenicMapsDrawSettings>  antigenic_maps{this, "antigenic_maps"};

    void upgrade();             // upgrade to the new version in case old version data provided

}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string_view aFilename);
void write_settings(const Settings& aSettings, std::string_view aFilename);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
