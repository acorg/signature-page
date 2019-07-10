#include <iostream>
#include <fstream>
#include <string>

#include "acmacs-base/argv.hh"
#include "acmacs-base/read-file.hh"
#include "acmacs-base/quicklook.hh"
#include "seqdb/seqdb.hh"

#include "signature-page.hh"
#include "settings.hh"

// ----------------------------------------------------------------------

using namespace acmacs::argv;
struct Options : public argv
{
    Options(int a_argc, const char* const a_argv[], on_error on_err = on_error::exit) : argv() { parse(a_argc, a_argv, on_err); }

    option<str> db_dir{*this, "db-dir"};
    option<str> seqdb{*this, "seqdb"};

    option<str_array> settings_files{*this, 's'};
    option<str>       init_settings{*this, 'i', "init-settings"};
    option<bool>      no_whocc{*this, "no-whocc", desc{"init settings without whocc defaults (clades, hz sections)"}};
    option<str>       report_cumulative{*this, "report-cumulative"};
    option<bool>      report_hz_section_antigens{*this, "report-hz-section-antigens"};
    option<bool>      show_aa_at_pos{*this, "show-aa-at-pos", desc{"show aa_at_pos section if --init-settings was used"}};
    option<size_t>    aa_at_pos_hz_section_threshold{*this, "aa-at-pos-hz-section-threshold", dflt{100UL}, desc{"if --init-settings and --show-aa-at-pos, detect hz sections with this threshold"}};
    option<size_t>    aa_at_pos_small_section_threshold{*this, "aa-at-pos-small-section-threshold", dflt{3UL}, desc{"if --init-settings and --show-aa-at-pos, elminate small sections having no more leaf nodes than this value"}};
    option<bool>      not_show_hz_sections{*this, "not-show-hz-sections"};
    option<bool>      hz_sections_report{*this, "hz-sections-report"};
    option<str>       report_first_node_of_subtree{*this, "report-first-node-of-subtree", desc{"filename or - to report subtree data"}};
    option<size_t>    subtree_threshold{*this, "subtree-threshold", desc{"min number of leaf nodes in a subtree for --report-first-node-of-subtree"}};
    option<str>       list_ladderized{*this, "list-ladderized"};
    option<bool>      no_draw{*this, "no-draw", desc{"do not generate pdf"}};
    option<str>       chart{*this, "chart", desc{"path to a chart for the signature page"}};
    option<bool>      ignore_seqdb_match_errors{*this, "ignore-seqdb-match-errors", desc{"for debugging"}};
    option<bool>      open{*this, "open"};
    option<bool>      verbose{*this, 'v', "verbose"};

    argument<str> tree_file{*this, arg_name{"tree.json[.xz]"}, mandatory};
    argument<str> output_pdf{*this, arg_name{"output.pdf"}, mandatory};
};

int main(int argc, const char* argv[])
{
    try {
        Options opt(argc, argv);
        seqdb::setup_dbs(opt.db_dir, opt.verbose ? seqdb::report::yes : seqdb::report::no);
        if (!opt.seqdb->empty())
            seqdb::setup(opt.seqdb, opt.verbose ? seqdb::report::yes : seqdb::report::no);

        {
            SignaturePageDraw signature_page;

            if (!opt.settings_files->empty()) {
                for (auto fn : *opt.settings_files) {
                    if (opt.verbose)
                        std::cerr << "DEBUG: reading settings from " << fn << '\n';
                    signature_page.load_settings(fn);
                }
            }

            signature_page.tree(opt.tree_file, opt.ignore_seqdb_match_errors ? seqdb::Seqdb::ignore_not_found::yes : seqdb::Seqdb::ignore_not_found::no);
            if (!opt.chart->empty())
                signature_page.chart(opt.chart);                                                                        // before make_surface!
            signature_page.make_surface(opt.output_pdf, !opt.init_settings->empty(), opt.show_aa_at_pos, !opt.no_draw); // before init_layout!
            if (!opt.init_settings->empty()) {
                signature_page.init_settings(opt.show_aa_at_pos, !opt.no_whocc);
            }
            signature_page.prepare(!opt.not_show_hz_sections);
            if (!opt.report_cumulative->empty()) {
                acmacs::file::ofstream out(opt.report_cumulative);
                signature_page.tree().report_cumulative_edge_length(out);
            }
            if (!opt.list_ladderized->empty()) {
                std::cout << "INFO: listing ladderized " << opt.list_ladderized << '\n';
                acmacs::file::ofstream out(opt.list_ladderized);
                signature_page.tree().list_strains(out);
            }
            if (!opt.report_first_node_of_subtree->empty()) {
                std::cout << "INFO: reporting first-node-of-subtree to " << opt.report_first_node_of_subtree << '\n';
                acmacs::file::ofstream out(opt.report_first_node_of_subtree);
                signature_page.tree().report_first_node_of_subtree(out, opt.subtree_threshold);
            }
            if (!opt.no_draw)
                signature_page.draw(opt.report_hz_section_antigens, !opt.init_settings->empty(), opt.aa_at_pos_hz_section_threshold, opt.aa_at_pos_small_section_threshold);
            if (!opt.init_settings->empty())
                signature_page.write_initialized_settings(opt.init_settings);
            if (opt.hz_sections_report)
                signature_page.tree_draw().hz_sections().report(std::cout);
            // if (!opt.hz_sections_report_html->empty())
            //     signature_page.tree_draw().hz_sections().report_html(opt.hz_sections_report_html);
        }

        if (!opt.no_draw)
            std::cout << "INFO: generated: " << opt.output_pdf << '\n';
        if (opt.open)
            acmacs::open(opt.output_pdf, 2);

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
