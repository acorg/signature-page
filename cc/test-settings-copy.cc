#include <iostream>

#include "settings.hh"
#include "acmacs-base/read-file.hh"

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <settings.json>" << std::endl;
        return 1;
    }
    Settings settings;
    read_settings(settings, argv[1]);
    acmacs_base::TempFile output;
    write_settings(settings, output);
    std::cout << static_cast<std::string>(output) << std::endl;
    return 0;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
