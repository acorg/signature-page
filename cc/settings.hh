#pragma once

#include "acmacs-base/rjson.hh"

#include "signature-page.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "clades-draw.hh"
#include "mapped-antigens-draw.hh"
#include "antigenic-maps-draw.hh"
#include "title-draw.hh"

// ----------------------------------------------------------------------

class Settings
{
 public:
    inline Settings()
        : mData{rjson::object{}}
        {
            distribute_parsed_value();
        }

    SignaturePageDrawSettings signature_page;
    TitleDrawSettings title;
    TreeDrawSettings tree_draw;
    TimeSeriesDrawSettings time_series;
    CladesDrawSettings clades;
    HzSections hz_sections;
    MappedAntigensDrawSettings mapped_antigens;
    AntigenicMapsDrawSettings antigenic_maps;

    void use_json(rjson::value&& aValue);
    void upgrade();             // upgrade to the new version in case old version data provided

    inline std::string to_json() const { return mData.to_json(); }

    // inline void indentation(const char* /*str*/, size_t /*length*/) {}
    // void set_version(const char* str, size_t length);
    // std::string version;

 private:
    rjson::value mData;

    void distribute_parsed_value();

}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename);
void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent = 2);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
