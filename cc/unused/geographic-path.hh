#pragma once

#include "surface.hh"

// ----------------------------------------------------------------------

namespace geographic_map
{
    constexpr const double size[2] = {1261, 632};
    extern Path path;
    Path continent_path(std::string aContinent);

} // namespace geographic_map

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
