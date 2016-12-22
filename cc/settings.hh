#pragma once

#include "signature-page.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"

// ----------------------------------------------------------------------

class Settings
{
 public:
    inline Settings() = default;

    SignaturePageDrawSettings signature_page;
    TreeDrawSettings tree_draw;
    TimeSeriesDrawSettings time_series;

}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename);
void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent = 2);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
