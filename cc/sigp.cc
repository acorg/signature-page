#include <iostream>
#include <string>
// #include <cstdlib>

#pragma GCC diagnostic push
#include "acmacs-base/boost-diagnostics.hh"
#include "boost/program_options.hpp"
#pragma GCC diagnostic pop

#include "signature-page.hh"
#include "settings.hh"

// ----------------------------------------------------------------------

class Options
{
 public:
    std::string settings_filename;
    std::string init_settings_filename;
    std::string tree_filename;
    std::string output_filename;
    std::string seqdb_filename;
    std::string chart_filename;
      //bool layout_tree;  // for init settings only
};

int get_args(int argc, const char *argv[], Options& aOptions);

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    Options options;
    int exit_code = get_args(argc, argv, options);
    if (exit_code == 0) {
        try {
            SignaturePageDraw signature_page;
            signature_page.load_settings(options.settings_filename);
            if (!options.init_settings_filename.empty())
                signature_page.init_layout(options.chart_filename.empty());
            signature_page.make_surface(options.output_filename);
            signature_page.tree(options.tree_filename, options.seqdb_filename);
            if (!options.chart_filename.empty())
                signature_page.chart(options.chart_filename);
            if (!options.init_settings_filename.empty()) {
                signature_page.init_settings();
                write_settings(signature_page.settings(), options.init_settings_filename);
            }
            signature_page.prepare();
            signature_page.draw();
        }
        catch (std::exception& err) {
            std::cerr << err.what() << std::endl;
            exit_code = 1;
        }
    }
    return exit_code;
}

// ----------------------------------------------------------------------

int get_args(int argc, const char *argv[], Options& aOptions)
{
    using namespace boost::program_options;
    options_description desc("Options");
    desc.add_options()
            ("help", "Print help messages")
            ("seqdb", value<std::string>(&aOptions.seqdb_filename)/* ->required() */, "path to seqdb")
            ("settings,s", value<std::string>(&aOptions.settings_filename), "signature page drawing settings (json) filename")
            ("init-settings", value<std::string>(&aOptions.init_settings_filename), "initialize signature page drawing settings (json) filename")
              // ("init-tree", bool_switch(&aOptions.layout_tree)->default_value(false), "initialize with tree layout")
            ("tree", value<std::string>(&aOptions.tree_filename)->required(), "path to tree to draw")
            ("chart", value<std::string>(&aOptions.chart_filename)->required(), "path to chart for signature page")
            ("output,o", value<std::string>(&aOptions.output_filename)->required(), "output pdf")
            ;
    positional_options_description pos_opt;
    pos_opt.add("tree", 1);
    pos_opt.add("output", 1);

    variables_map vm;
    try {
        store(command_line_parser(argc, argv).options(desc).positional(pos_opt).run(), vm);
        if (vm.count("help")) {
            std::cerr << desc << std::endl;
            return 1;
        }
        notify(vm);
        return 0;
    }
    catch(required_option& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
          // std::cerr << "Usage: " << argv[0] << " <tree.json> <output.pdf>" << std::endl;
        return 2;
    }
    catch(error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        return 3;
    }

} // get_args

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
