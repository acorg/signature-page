#include <iostream>
#include <cstdlib>

#include "settings.hh"
#include "acmacs-base/read-file.hh"

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <settings.json> [<output-settings.json>]" << std::endl;
        return 1;
    }
    try {
        Settings settings;
        read_settings(settings, argv[1]);
        std::string output;
        if (argc > 2)
            output = argv[2];
        else
            output = static_cast<std::string>(acmacs::file::temp{".json"});
        write_settings(settings, output);
          // std::cout << static_cast<std::string>(output) << std::endl;
        if (std::system((std::string{"/usr/bin/diff -b '"} + argv[1] + "' '" + static_cast<std::string>(output) + "'").c_str()))
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
