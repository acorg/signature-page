#pragma once

#include <utility>
#include <string>

// ----------------------------------------------------------------------

constexpr const double continent_map_size[2] = {686.475, 346.750};

std::pair<const double*, const double*> continent_map_path(std::string continent);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
