#pragma once

#include "tree-draw.hh"

// ----------------------------------------------------------------------

class Settings
{
 public:
    inline Settings() = default;

    TreeDrawSettings tree_draw;

}; // class Settings

// ----------------------------------------------------------------------

void read_settings(Settings& aSettings, std::string aFilename);
void write_settings(const Settings& aSettings, std::string aFilename, size_t aIndent = 2);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
