#include <iostream>

#include "settings.hh"

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <settings.json>" << std::endl;
        return 1;
    }
    Settings settings;
    write_settings(settings, argv[1]);
    return 0;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
