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
    // std::cerr << "DEBUG: AntigenicMapsDrawBase::init_settings" << '\n';
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
    layout().init_settings();
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
                result.set(acmacs::Location2D{viewport[0], viewport[1]}, acmacs::Size{viewport[2], viewport[3]});
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

AntigenicMapsDrawSettings::AntigenicMapsDrawSettings(acmacs::settings::base& parent)
    : acmacs::settings::object(parent)
{
    add_viewport_mod();

    auto m2 = mods.append();
    m2->name = "point_scale";
    m2->scale = 1.0;
    m2->outline_scale = 1.0;
    auto m3 = mods.append();
    m3->name = "rotate";
    m3->degrees = 0.0;
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
    {
        auto m16 = mods.append();
        m16->name = "antigens";
        m16->select->vaccine->type = "previous";
        m16->select->vaccine->passage = "cell";
        m16->fill = BLUE;
        m16->outline = WHITE;
        m16->size = 15.0;
        m16->order = "raise";
        m16->report = false;
        m16->label->offset = acmacs::Offset{0, 1};
        m16->label->name_type = "abbreviated_location_with_passage_type";
        m16->label->size = 9.0;
    }
    {
        auto m17 = mods.append();
        m17->name = "antigens";
        m17->select->vaccine->type = "previous";
        m17->select->vaccine->passage = "egg";
        m17->fill = BLUE;
        m17->outline = WHITE;
        m17->size = 15.0;
        m17->order = "raise";
        m17->report = false;
        m17->label->offset = acmacs::Offset{0, 1};
        m17->label->name_type = "abbreviated_location_with_passage_type";
        m17->label->size = 9.0;
    }
    {
        auto m18 = mods.append();
        m18->name_commented = "antigens";
        m18->select->vaccine->type = "previous";
        m18->select->vaccine->passage = "reassortant";
        m18->fill = BLUE;
        m18->outline = WHITE;
        m18->size = 15.0;
        m18->order = "raise";
        m18->report = false;
        m18->label->offset = acmacs::Offset{0, 1};
        m18->label->name_type = "abbreviated_location_with_passage_type";
        m18->label->size = 9.0;
    }
    {
        auto m19 = mods.append();
        m19->name = "antigens";
        m19->select->vaccine->type = "current";
        m19->select->vaccine->passage = "cell";
        m19->fill = RED;
        m19->outline = WHITE;
        m19->size = 15.0;
        m19->order = "raise";
        m19->report = false;
        m19->label->offset = acmacs::Offset{0, 1};
        m19->label->name_type = "abbreviated_location_with_passage_type";
        m19->label->size = 9.0;
    }
    {
        auto m20 = mods.append();
        m20->name = "antigens";
        m20->select->vaccine->type = "current";
        m20->select->vaccine->passage = "egg";
        m20->fill = RED;
        m20->outline = WHITE;
        m20->size = 15.0;
        m20->order = "raise";
        m20->report = false;
        m20->label->offset = acmacs::Offset{0, 1};
        m20->label->name_type = "abbreviated_location_with_passage_type";
        m20->label->size = 9.0;
    }
    {
        auto m21 = mods.append();
        m21->name = "antigens";
        m21->select->vaccine->type = "current";
        m21->select->vaccine->passage = "reassortant";
        m21->fill = GREEN;
        m21->outline = WHITE;
        m21->size = 15.0;
        m21->order = "raise";
        m21->report = false;
        m21->label->offset = acmacs::Offset{0, 1};
        m21->label->name_type = "abbreviated_location_with_passage_type";
        m21->label->size = 9.0;
    }
    {
        auto m22 = mods.append();
        m22->name = "antigens";
        m22->select->vaccine->type = "surrogate";
        m22->select->vaccine->passage = "cell";
        m22->fill = "pink";
        m22->outline = WHITE;
        m22->size = 15.0;
        m22->order = "raise";
        m22->report = false;
        m22->label->offset = acmacs::Offset{0, 1};
        m22->label->name_type = "abbreviated_location_with_passage_type";
        m22->label->size = 9.0;
    }
    {
        auto m23 = mods.append();
        m23->name = "antigens";
        m23->select->vaccine->type = "surrogate";
        m23->select->vaccine->passage = "egg";
        m23->fill = "pink";
        m23->outline = WHITE;
        m23->size = 15.0;
        m23->order = "raise";
        m23->report = false;
        m23->label->offset = acmacs::Offset{0, 1};
        m23->label->name_type = "abbreviated_location_with_passage_type";
        m23->label->size = 9.0;
    }
    {
        auto m24 = mods.append();
        m24->name = "antigens";
        m24->select->vaccine->type = "surrogate";
        m24->select->vaccine->passage = "reassortant";
        m24->fill = "pink";
        m24->outline = WHITE;
        m24->size = 15.0;
        m24->order = "raise";
        m24->report = false;
        m24->label->offset = acmacs::Offset{0, 1};
        m24->label->name_type = "abbreviated_location_with_passage_type";
        m24->label->size = 9.0;
    }

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
    auto make_setting_list = [&aViewport](acmacs::settings::field_array<double>& target) -> void {
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
    if (auto vpmod = mods.find_if([](const auto& mod) -> bool { return mod.name == "viewport"; }); !vpmod) {
        auto mod = mods.append();
        mod->name = "viewport";
        mod->rel.set(rel.begin(), rel.end());
    }
    else {
        (*vpmod)->rel.set(rel.begin(), rel.end());
    }

} // AntigenicMapsDrawSettings::viewport_rel

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
