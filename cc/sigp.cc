#include <iostream>
#include <string>

#include "acmacs-base/argc-argv.hh"
#include "seqdb/seqdb.hh"

#include "signature-page.hh"
#include "settings.hh"

using namespace std::string_literals;

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    try {
        argc_argv args(argc, argv, {
                {"--db-dir", ""},
                {"--seqdb", ""},

                {"-s", argc_argv::strings{}}, //value<std::vector<std::string>>(&aOptions.settings_filename), "signature page drawing settings (json) filename")
                {"--init-settings", ""}, // value<std::string>(&aOptions.init_settings_filename), "initialize signature page drawing settings (json) filename")
                {"--report-cumulative", false}, // bool_switch(&aOptions.report_cumulative)->default_value(false), "report cumulative edge lengths for leaf nodes of the tree")
                {"--report-hz-section_antigens", false}, // bool_switch(&aOptions.report_antigens_in_hz_sections)->default_value(false), "report antigens in each hz section")
                {"--list-ladderized", false}, // bool_switch(&aOptions.list_ladderized)->default_value(false), "list strain names after ladderizing")
                {"--no-draw", false}, // bool_switch(&aOptions.no_draw)->default_value(false), "do not generate pdf")
                {"--chart", ""}, // value<std::string>(&aOptions.chart_filename), "path to a chart for the signature page")

                {"-v", false},
                {"--verbose", false},
                {"-h", false},
                {"--help", false},
            });

          // {"tree", value<std::string>(&aOptions.tree_filename)->required(), "path to tree to draw")
          // ("output,o", value<std::string>(&aOptions.output_filename)->required(), "output pdf")

        if (args["-h"] || args["--help"] || args.number_of_arguments() != 2) {
            throw std::runtime_error("Usage: "s + args.program() + " [options] <tree.json> <output.pdf>\n" + args.usage_options());
        }
        const bool verbose = args["-v"] || args["--verbose"];
        seqdb::setup_dbs(args["--db-dir"], verbose);

        SignaturePageDraw signature_page;
        signature_page.load_settings(args["-s"]);
        signature_page.tree(args[0], args["--seqdb"]);
        if (args["--chart"])
            signature_page.chart(args["--chart"]); // before make_surface!
        signature_page.make_surface(args[1], args["--init-settings"], !args["--no-draw"]); // before init_layout!
        if (args["--init-settings"]) {
            signature_page.init_layout();
            signature_page.init_settings();
        }
        signature_page.prepare();
        if (args["--report-cumulative"])
            signature_page.tree().report_cumulative_edge_length(std::cout);
        if (args["--list-ladderized"])
            signature_page.tree().list_strains(std::cout);
        if (!args["--no-draw"])
            signature_page.draw(args["--report-hz-section_antigens"]);
        if (args["--init-settings"])
            signature_page.write_initialized_settings(args["--init-settings"]);

        return 0;
    }
    catch (std::exception& err) {
        std::cerr << "ERROR: " << err.what() << '\n';
        return 1;
    }
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
