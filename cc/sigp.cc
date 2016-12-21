#include <iostream>
#include <string>
// #include <cstdlib>

#pragma GCC diagnostic push
#include "acmacs-base/boost-diagnostics.hh"
#include "boost/program_options.hpp"
#pragma GCC diagnostic pop

#include "surface-cairo.hh"
#include "signature-page.hh"


// ----------------------------------------------------------------------

int get_args(int argc, const char *argv[], std::string& aSettingsFilename, std::string& aTreeFilename, std::string& aOutputPdf, std::string& aSeqdbFilename);

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    std::string settings_filename, tree_filename, output_filename, seqdb_filename;
    int exit_code = get_args(argc, argv, settings_filename, tree_filename, output_filename, seqdb_filename);
    if (exit_code == 0) {
        try {
            PdfCairo surface(output_filename, 500, 850);
            SignaturePageDraw signature_page(surface);
            signature_page.settings(settings_filename);
            signature_page.tree(tree_filename, seqdb_filename);
            signature_page.prepare();
            signature_page.draw();

            // const double offset = 100;
            // // const double scale = (surface.width() - offset * 4) / surface.width();
            // // std::cout << "Sub scale:" << scale << std::endl;
            // std::unique_ptr<Surface> sub{surface.subsurface({offset, offset}, surface.size() - Size(offset * 2, offset * 2), surface.size().width, false)};
            // sub->border(0xA0FFA000, 10);
            // draw(settings, *sub, tree);
        }
        catch (std::exception& err) {
            std::cerr << err.what() << std::endl;
            exit_code = 1;
        }
    }
    return exit_code;
}

// ----------------------------------------------------------------------

int get_args(int argc, const char *argv[], std::string& aSettingsFilename, std::string& aTreeFilename, std::string& aOutputPdf, std::string& aSeqdbFilename)
{
    using namespace boost::program_options;
    options_description desc("Options");
    desc.add_options()
            ("help", "Print help messages")
            ("seqdb", value<std::string>(&aSeqdbFilename)/* ->required() */, "path to seqdb")
            ("settings,s", value<std::string>(&aSettingsFilename), "signature page drawing settings (json) filename")
            ("tree", value<std::string>(&aTreeFilename)->required(), "path to tree to draw")
            ("output,o", value<std::string>(&aOutputPdf)->required(), "output pdf")
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
