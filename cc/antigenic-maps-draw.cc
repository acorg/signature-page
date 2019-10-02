#include <typeinfo>

#include "acmacs-chart-2/factory-import.hh"
#include "tree-draw.hh"
#include "signature-page.hh"
#include "antigenic-maps-layout.hh"
#include "ace-antigenic-maps-draw.hh"

// ----------------------------------------------------------------------

AntigenicMapsDrawBase::AntigenicMapsDrawBase(acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, TimeSeriesDraw& aTimeSeriesDraw, AntigenicMapsDrawSettings& aSettings)
    : mSurface(aSurface), mTree(aTree), mHzSections(aHzSections),
      mSignaturePageDrawSettings(aSignaturePageDrawSettings), mTimeSeriesDraw(aTimeSeriesDraw), mSettings(aSettings)
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

void AntigenicMapsDrawBase::init_settings(const SettingsInitializer& settings_initilizer)
{
    // std::cerr << "DEBUG: AntigenicMapsDrawBase::init_settings" << '\n';
    const size_t number_sections = hz_sections().shown_maps();
    switch (number_sections) {
      case 0:
      case 1:
      case 2:
          settings().columns = 1;
          break;
      case 3:
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
    const size_t maps_per_column = number_sections / settings().columns + ((number_sections % settings().columns) == 0 ? 0 : 1);
    const double map_width = 150; // height is not available at this moment mSurface.viewport().size.height / (maps_per_column + settings().gap * (maps_per_column - 1));
    signature_page_settings().antigenic_maps_width = map_width * settings().columns + (settings().columns - 1) * settings().gap;
    switch (settings().columns) {
      case 2:
          switch (maps_per_column) {
            case 2:
              signature_page_settings().antigenic_maps_width = 579;
              break;
          }
          break;
      case 3:
          signature_page_settings().antigenic_maps_width = 579;
          break;
    }

    std::cout << "INFO: antigenic maps: columns:" << settings().columns << " maps_per_column:" << maps_per_column << " map_width:" << map_width << " width:" << signature_page_settings().antigenic_maps_width << '\n';

    if (!mLayout)
        make_layout();
    layout().init_settings(settings_initilizer);
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
    // std::cerr << "DEBUG: AntigenicMapsDrawBase::prepare" << '\n';
    if (!mLayout)
        make_layout();
    layout().prepare();

} // AntigenicMapsDrawBase::prepare

// ----------------------------------------------------------------------

AntigenicMapsDrawBase* make_antigenic_maps_draw(std::string aChartFilename, acmacs::surface::Surface& aSurface, Tree& aTree, HzSections& aHzSections, SignaturePageDrawSettings& aSignaturePageDrawSettings, TimeSeriesDraw& aTimeSeriesDraw, AntigenicMapsDrawSettings& aSettings)
{
    std::string error;
    try {
        return new AntigenicMapsDraw(aSurface, aTree, std::make_shared<acmacs::chart::ChartModify>(acmacs::chart::import_from_file(aChartFilename, acmacs::chart::Verify::None, report_time::no)), aHzSections, aSignaturePageDrawSettings, aTimeSeriesDraw, aSettings);
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

acmacs::Viewport AntigenicMapMod::get_viewport(const acmacs::Viewport& aOrigViewport) const
{
    acmacs::Viewport result = aOrigViewport;

    try {
        switch (rel.size()) {
            case 3:
                result.set({aOrigViewport.left() + rel[0], aOrigViewport.top() + rel[1]}, aOrigViewport.size.width + rel[2]);
                break;
            case 0:
                break;
            default:
                throw std::exception{};
        }
    }
    catch (std::exception&) {
        std::cerr << "ERROR: cannot convert json to array (viewport rel): " << rel << '\n';
        throw;
    }

    try {
        switch (viewport.size()) {
            case 3:
                result.set({viewport[0], viewport[1]}, viewport[2]);
                break;
            case 4:
                result.set(acmacs::PointCoordinates(viewport[0], viewport[1]), acmacs::Size{viewport[2], viewport[3]});
                break;
            case 0:
                break;
            default:
                throw std::exception{};
        }
    }
    catch (std::exception&) {
        std::cerr << "ERROR: cannot convert json to array (viewport): " << viewport << '\n';
        throw;
    }

    return result;

} // AntigenicMapMod::get_viewport

// ----------------------------------------------------------------------

AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(acmacs::settings::v1::base& parent) : acmacs::settings::v1::object(parent)
{
    add_viewport_mod();

    auto m2 = mods.append();
    m2->name = "point_scale";
    m2->scale = 1.0;
    m2->outline_scale = 1.0;
    {
        auto m = mods.append();
        m->name_commented = "flip";
        m->direction = "ns";
    }
    {
        auto m3 = mods.append();
        m3->name = "rotate";
        m3->degrees = 0.0;
    }
    auto m4 = mods.append();
    m4->name = "sera";
    m4->outline = "grey88";
    m4->outline_width = 0.5;
    m4->size = 5.0;
    auto m5 = mods.append();
    m5->name = "reference_antigens";
    m5->fill = TRANSPARENT;
    m5->outline = "grey88";
    m5->outline_width = 0.5;
    m5->size = 5.0;
    auto m6 = mods.append();
    m6->name = "test_antigens";
    m6->fill = "grey88";
    m6->outline = "grey88";
    m6->outline_width = 0.5;
    m6->size = 3.0;
    auto m7 = mods.append();
    m7->name = "sequenced_antigens";
    m7->fill = "grey63";
    m7->outline = WHITE;
    m7->outline_width = 0.5;
    m7->size = 3.0;
    auto m8 = mods.append();
    m8->name = "tracked_antigens";
    m8->fill = "green3";
    m8->outline = WHITE;
    m8->outline_width = 0.5;
    m8->size = 5.0;
    auto m9 = mods.append();
    m9->name = "?tracked_sera";
    m9->fill = TRANSPARENT;
    m9->outline = BLACK;
    m9->outline_width = 0.5;
    m9->size = 5.0; // tracked sera above tracked antigens!
    auto m10 = mods.append();
    m10->name = "?tracked_serum_circles";
    m10->outline = BLACK;
    m10->outline_width = 0.5; // tracked serum circles above tracked antigens!
    auto m11 = mods.append();
    m11->name = "?serum_circle";
    m11->map = "A";
    m11->serum = "A(H3N2)/SOUTH CAROLINA/4/2017 CDC 2017-106";
    m11->outline = BLACK;
    m11->outline_width = 0.5;
    m11->fill = TRANSPARENT;
    m11->radius_line = TRANSPARENT;
    m11->radius_line_width = 1.0;
    m11->radius_line_dash = "nodash";
    m11->serum_size = 5.0;
    m11->serum_outline = BLACK;
    m11->serum_outline_width = 0.5;
    auto m12 = mods.append();
    m12->name = "title";
    m12->text_color = BLACK;
    m12->text_size = 12.0;
    m12->padding = 3.0;
    m12->offset = acmacs::Offset{0.0, 0.0};
    m12->weight = "normal";
    m12->slant = "normal";
    m12->font_family = "san serif";
    auto m13 = mods.append();
    m13->name = "background";
    m13->color = WHITE;
    auto m14 = mods.append();
    m14->name = "grid";
    m14->color = "grey80";
    m14->line_width = 1.0;
    auto m15 = mods.append();
    m15->name = "border";
    m15->color = BLACK;
    m15->line_width = 1.0;

    // vaccine spec via acmacs-map-draw/ModAntigens, since 2018-01-19
    const auto add_vaccine = [this](bool shown, std::string type, std::string passage, Color fill, Color outline, bool report) {
        auto mod = mods.append();
        if (shown)
            mod->name = "antigens";
        else
            mod->name_commented = "antigens";
        mod->select->vaccine->type = type;
        mod->select->vaccine->passage = passage;
        mod->fill = fill;
        mod->outline = outline;
        mod->report = report;

        mod->size = 15.0;
        mod->order = "raise";
        mod->label->offset = acmacs::Offset{0, 1};
        mod->label->name_type = "abbreviated_location_with_passage_type";
        mod->label->size = 9.0;
    };

    add_vaccine(true, "previous", "cell", BLUE, WHITE, true);
    add_vaccine(true, "previous", "egg", BLUE, WHITE, true);
    add_vaccine(false, "previous", "reassortant", BLUE, WHITE, true);
    add_vaccine(true, "current", "cell", RED, WHITE, true);
    add_vaccine(true, "current", "egg", RED, WHITE, true);
    add_vaccine(true, "current", "reassortant", GREEN, WHITE, true);
    add_vaccine(true, "surrogate", "cell", PINK, WHITE, true);
    add_vaccine(true, "surrogate", "egg", PINK, WHITE, true);
    add_vaccine(true, "surrogate", "reassortant", PINK, WHITE, true);

} // AntigenicMapsDrawSettings::AntigenicMapsDrawSettings

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::add_viewport_mod()
{
    auto m1 = mods.append();
    m1->name = "viewport";
    m1->rel.set({0, 0, 0});

} // AntigenicMapsDrawSettings::add_viewport_mod

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::viewport(const acmacs::Viewport& aViewport)
{
    auto make_setting_list = [&aViewport](acmacs::settings::v1::field_array<double>& target) -> void {
        if (float_equal(aViewport.size.width, aViewport.size.height))
            target.set({aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width});
        else
            target.set({aViewport.origin.x(), aViewport.origin.y(), aViewport.size.width, aViewport.size.height});
    };
    // std::cerr << "DEBUG: AntigenicMapsDrawSettings::viewport" << '\n';
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name == "viewport"; }); !vpmod) {
        auto mod = mods.append();
        mod->name = "viewport";
        make_setting_list(mod->viewport_commented);
    }
    else {
        make_setting_list((*vpmod)->viewport_commented);
    }

} // AntigenicMapsDrawSettings::viewport

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::viewport_rel(const std::vector<double>& rel)
{
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name.get_or("*") == "viewport"; }); !vpmod) {
        auto mod = mods.append();
        mod->name = "viewport";
        mod->rel.set(rel.begin(), rel.end());
    }
    else {
        (*vpmod)->rel.set(rel.begin(), rel.end());
    }

} // AntigenicMapsDrawSettings::viewport_rel

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::rotate_degrees(double degrees)
{
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name.get_or("*") == "rotate"; }); !vpmod) {
        auto mod = mods.append();
        mod->name = "rotate";
        mod->degrees = degrees;
    }
    else {
        (*vpmod)->degrees = degrees;
    }

} // AntigenicMapsDrawSettings::rotate_degrees

// ----------------------------------------------------------------------

void AntigenicMapsDrawSettings::flip(std::string direction)
{
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name.get_or("*") == "flip" || mod.name.get_or("*") == "?flip" || mod.name_commented.get_or("*") == "flip"; }); !vpmod) {
        auto mod = mods.append();
        mod->name = "flip";
        mod->direction = direction;
    }
    else {
        (*vpmod)->name = "flip"; // in case it was commented out
        (*vpmod)->direction = direction;
    }

} // AntigenicMapsDrawSettings::flip

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
