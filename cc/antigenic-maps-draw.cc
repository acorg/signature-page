#include <typeinfo>

#include "acmacs-chart-2/factory-import.hh"
#include "tree-draw.hh"
#include "signature-page.hh"
#include "antigenic-maps-layout.hh"
#include "ace-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::AntigenicMapsDrawBase(acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
    : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections),
      mSignaturePageDrawSettings(aSignaturePageDrawSettings), mSettings(aSettings)
{

} // AntigenicMapsDrawBase::AntigenicMapsDrawBase

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::~AntigenicMapsDrawBase()
{
}

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::layout(AntigenicMapsLayout* aLayout)
{
    mLayout.reset(aLayout);

} // AntigenicMapsDrawBase::layout

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::init_settings()
{
    // std::cerr << "DEBUG: AntigenicMapsDrawBase::init_settings" << std::endl;
    const size_t number_sections = hz_sections().shown_maps();
    switch (number_sections) {
      case 0:
      case 1:
      case 2:
      case 3:
          settings().columns = 1;
          break;
      case 4:
      case 5:
      case 6:
          settings().columns = 2;
          break;
      case 7:
      case 8:
      case 9:
          settings().columns = 3;
          break;
      default:
          settings().columns = 3;
          break;
    }
    size_t maps_per_column = number_sections / settings().columns + ((number_sections % settings().columns) == 0 ? 0 : 1);
    const double map_width = 150; // height is not available at this moment mSurface.viewport().size.height / (maps_per_column + settings().gap * (maps_per_column - 1));
    signature_page_settings().antigenic_maps_width = map_width * settings().columns + (settings().columns - 1) * settings().gap;

    std::cout << "INFO: antigenic maps: columns:" << settings().columns << " maps_per_column:" << maps_per_column << " map_width:" << map_width << " width:" << signature_page_settings().antigenic_maps_width << std::endl;

    chart().init_settings();

} // AntigenicMapsDrawBase::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::draw(acmacs::surface::Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections)
{
    layout().draw(aMappedAntigensDrawSurface, report_antigens_in_hz_sections);

} // AntigenicMapsDrawBase::draw

// ----------------------------------------------------------------------

void AntigenicMapsDrawBase::prepare()
{
      // std::cerr << "DEBUG: AntigenicMapsDrawBase::prepare" << std::endl;
    if (static_cast<std::string>(settings().layout) == "labelled_grid") {
        make_layout();
    }
    else {
        throw std::runtime_error("Unrecognized antigenic maps layout: " + static_cast<std::string>(settings().layout));
    }
    layout().prepare();

} // AntigenicMapsDrawBase::prepare

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, AntigenicMapsDrawSettings& aSettings)
{
    std::string error;
    try {
        return new AntigenicMapsDraw(aSurface, aTree, std::make_shared<acmacs::chart::ChartModify>(acmacs::chart::import_from_file(aChartFilename, acmacs::chart::Verify::None, report_time::No)), aHzSections, aSignaturePageDrawSettings, aSettings);
    }
    catch (acmacs::chart::import_error& err) {
        error = err.what();
    }
    // try {
    //     return new sdb::AntigenicMapsDraw(aSurface, aTree, sdb::read_chart_from_sdb(aChartFilename), aHzSections, aSignaturePageDrawSettings, aSettings);
    // }
    // catch (ChartReadError& err) {
    //     error += std::string("\n[Not SDB] ") + err.what();
    // }
    throw ChartReadError(error);

} // make_antigenic_maps_draw

// ----------------------------------------------------------------------

AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(acmacs::settings::base& parent)
    : acmacs::settings::object(parent)
{
    mods.emplace_back().add("N", "viewport").add("rel", rjson::array{0, 0, 0});
    mods.emplace_back().add("N", "point_scale").add("scale", 1.0).add("outline_scale", 1.0);
    mods.emplace_back().add("N", "rotate").add("degrees", 0.0);
    mods.emplace_back().add("N", "sera").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "reference_antigens").add("fill", "transparent").add("outline", "grey88").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "test_antigens").add("fill", "grey88").add("outline", "grey88").add("outline_width", 0.5).add("size", 3.0);
    mods.emplace_back().add("N", "sequenced_antigens").add("fill", "grey63").add("outline", "white").add("outline_width", 0.5).add("size", 3.0);
    mods.emplace_back().add("N", "tracked_antigens").add("fill", "green3").add("outline", "white").add("outline_width", 0.5).add("size", 5.0);
    mods.emplace_back().add("N", "?tracked_sera").add("fill", "transparent").add("outline", "black").add("outline_width", 0.5).add("size", 5.0); // tracked sera above tracked antigens!
    mods.emplace_back().add("N", "?tracked_serum_circles").add("outline", "black").add("outline_width", 0.5); // tracked serum circles above tracked antigens!
    mods.emplace_back().add("N", "?serum_circle").add("map", "A").add("serum", "A(H3N2)/SOUTH CAROLINA/4/2017 CDC 2017-106")
            .add("outline", "black").add("outline_width", 0.5).add("fill", "transparent").add("radius_line", "transparent").add("radius_line_width", 1.0).add("radius_line_dash", "nodash")
            .add("serum_size", 5.0).add("serum_outline", "black").add("serum_outline_width", 0.5);
    mods.emplace_back().add("N", "title").add("text_color", "black").add("text_size", 12.0).add("padding", 3.0)
            .add("offset", rjson::array{0.0, 0.0}).add("weight", "normal").add("slant", "normal").add("font_family", "san serif");
    mods.emplace_back().add("N", "background").add("color", "white");
    mods.emplace_back().add("N", "grid").add("color", "grey80").add("line_width", 1.0);
    mods.emplace_back().add("N", "border").add("color", "black").add("line_width", 1.0);

      // vaccine spec via acmacs-map-draw/ModAntigens, since 2018-01-19
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "cell"}}}})
            .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "egg"}}}})
            .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("?N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "previous"}, {"passage", "reassortant"}}}})
            .add("fill", "blue").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});

    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "cell"}}}})
            .add("fill", "red").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "egg"}}}})
            .add("fill", "red").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "current"}, {"passage", "reassortant"}}}})
            .add("fill", "green").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});

    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "cell"}}}})
            .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "egg"}}}})
            .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
    mods.emplace_back().add("N", "antigens").add("select", rjson::object{{"vaccine", rjson::object{{"type", "surrogate"}, {"passage", "reassortant"}}}})
            .add("fill", "pink").add("outline", "white").add("size", 15.0).add("order", "raise").add("report", false)
            .add("label", rjson::object{{"offset", rjson::array{0, 1}}, {"name_type", "abbreviated_location_with_passage_type"}, {"size", 9.0}});
} // AntigenicMapsDrawSettings::AntigenicMapsDrawSettings

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::viewport(const acmacs::Viewport& aViewport)
{
    auto make_setting_list = [&aViewport]() -> rjson::value {
        if (float_equal(aViewport.size.width, aViewport.size.height))
            return rjson::array{aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width};
        else
            return rjson::array{aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width, aViewport.size.height};
    };
      // std::cerr << "DEBUG: AntigenicMapsDrawSettings::viewport" << std::endl;
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name() == "viewport"; }); !vpmod) {
        auto mod = mods.emplace_back();
        mod.set_field("N", "viewport");
        mod.set_field("viewport", make_setting_list());
    }
    else
        (*vpmod).set_field("viewport", make_setting_list());

} // AntigenicMapsDrawSettings::viewport

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
