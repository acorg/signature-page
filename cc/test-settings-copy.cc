#include <iostream>
#include <cstdlib>

#include "settings.hh"
#include "acmacs-base/read-file.hh"

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <settings.json>" << std::endl;
        return 1;
    }
    try {
        Settings settings;
        read_settings(settings, argv[1]);
        acmacs_base::TempFile output;
        write_settings(settings, output);
          // std::cout << static_cast<std::string>(output) << std::endl;
        if (std::system(("/usr/bin/diff '" + static_cast<std::string>(output) + "' '" + argv[1] + "'").c_str()))
            throw std::runtime_error("FAILED");
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 2;
    }
    return 0;
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
