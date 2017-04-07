#include <iostream>
#include <string>
// #include <cstdlib>

#pragma GCC diagnostic push
#include "acmacs-base/boost-diagnostics.hh"
#include "boost/program_options.hpp"
#pragma GCC diagnostic pop

#include "acmacs-draw/surface-cairo.hh"
#include "chart-draw.hh"
#include "antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

class Options
{
 public:
    std::string output_filename;
    std::string chart_filename;
};

int get_args(int argc, const char *argv[], Options& aOptions);

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    Options options;
    int exit_code = get_args(argc, argv, options);
    if (exit_code == 0) {
        try {
            auto chart = std::unique_ptr<sdb::Chart>{dynamic_cast<sdb::Chart*>(sdb::read_chart_from_sdb(options.chart_filename))};
            auto viewport = chart->viewport(nullptr);
            std::cout << viewport << std::endl;

            DrawSerum draw_serum;
            DrawVaccineAntigen draw_vaccine;
            DrawReferenceAntigen draw_reference;
            DrawTestAntigen draw_test;
            std::vector<const DrawPoint*> draw_points(chart->points().size(), nullptr);
            for (size_t point_no = 0; point_no < chart->points().size(); ++point_no) {
                const auto& p = chart->points()[point_no];
                if (p.attributes.antigen) {
                    if (p.attributes.vaccine) {
                        draw_points[point_no] = &draw_vaccine;
                    }
                    else if (p.attributes.reference) {
                        draw_points[point_no] = &draw_reference;
                    }
                    else {
                        draw_points[point_no] = &draw_test;
                    }
                }
                else {
                    draw_points[point_no] = &draw_serum;
                }
            }

            AntigenicMapsDrawSettings settings;
            settings.test_antigen_outline_color.from_string("black");
            settings.test_antigen_fill_color.from_string("green");
            settings.reference_antigen_outline_color.from_string("black");
            settings.vaccine_antigen_outline_color.from_string("black");
            settings.serum_outline_color.from_string("black");

            double width = 600;
            auto surface = std::make_unique<PdfCairo>(options.output_filename, width, width);
            surface->viewport(viewport);
            surface->background("white");
            surface->grid(Scaled{1}, "grey80", Pixels{0.5});

            size_t drawn = 0;
            for (size_t level = 0; level < 10 && drawn < draw_points.size(); ++level) {
                for (size_t point_no = 0; point_no < draw_points.size(); ++point_no) {
                    if (draw_points[point_no]->level() == level) {
                        draw_points[point_no]->draw(*surface, chart->points()[point_no], chart->plot_style().style(point_no), settings);
                        ++drawn;
                    }
                }
            }

            surface->border("black", Pixels{1});
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
            ("chart", value<std::string>(&aOptions.chart_filename), "path to chart for signature page")
            ("output,o", value<std::string>(&aOptions.output_filename)->required(), "output pdf")
            ;
    positional_options_description pos_opt;
    pos_opt.add("chart", 1);
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
