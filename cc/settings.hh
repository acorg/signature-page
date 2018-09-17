#pragma once

#include "signature-page.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "clades-draw.hh"
#include "mapped-antigens-draw.hh"
#include "aa-at-pos-draw.hh"
#include "antigenic-maps-draw.hh"
#include "title-draw.hh"
#include "rjson-serialize.hh"

// ----------------------------------------------------------------------

class Settings : public rjson::v1::field_container_toplevel
{
 public:
    Settings();

    rjson::field_get_set<std::string> version;
    SignaturePageDrawSettings signature_page;
    TitleDrawSettings title;
    TreeDrawSettings tree_draw;
    TimeSeriesDrawSettings time_series;
    CladesDrawSettings clades;
    HzSections hz_sections;
    MappedAntigensDrawSettings mapped_antigens;
    AAAtPosDrawSettings aa_at_pos;
    AntigenicMapsDrawSettings antigenic_maps;

    void upgrade();             // upgrade to the new version in case old version data provided


    // void distribute_parsed_value();


}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename);
void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent = 2);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
