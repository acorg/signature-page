#include "acmacs-base/log.hh"
#include "antigenic-maps-layout.hh"
#include "chart-draw.hh"
//#include "tree.hh"
#include "tree-draw.hh"
#include "signature-page.hh"
#include "settings-initializer.hh"

// ----------------------------------------------------------------------

AntigenicMapsLayoutDraw::~AntigenicMapsLayoutDraw()
{
} // AntigenicMapsLayoutDraw::~AntigenicMapsLayoutDraw

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDraw::init_settings(const SettingsInitializer& settings_initilizer)
{
    settings().viewport(mAntigenicMapsDraw.chart().calculate_viewport());
    settings().viewport_rel(settings_initilizer.viewport_rel());
    settings().rotate_degrees(settings_initilizer.rotate_degrees());
    if (const auto flip = settings_initilizer.flip(); flip)
        settings().flip(*flip);

} // AntigenicMapsLayoutDraw::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDraw::prepare()
{
    find_sequenced_antigens();
    prepare_apply_mods();

      // std::cerr << "DEBUG: AntigenicMapsLayoutDraw::prepare\n";
    // const acmacs::Viewport& viewport = mAntigenicMapsDraw.chart().viewport();
    // settings().viewport(viewport);
    fmt::print("INFO: Using viewport: {}\n", mAntigenicMapsDraw.chart().viewport());

} // AntigenicMapsLayoutDraw::prepare

// ----------------------------------------------------------------------

const acmacs::Viewport& AntigenicMapsLayoutDraw::viewport() const
{
    return mAntigenicMapsDraw.chart().viewport();
}

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDraw::apply_mods_before(acmacs::surface::Surface& aSurface)
{
    settings().mods.for_each([&aSurface](const auto& mod, size_t /*mod_no*/) {
        if (mod.name.is_set_or_has_default()) {
            if (mod.name == "background") {
                const Color color = mod.color.get_or(WHITE);
                const auto& v = aSurface.viewport();
                aSurface.rectangle_filled(v.origin, v.size, color, Pixels{0}, color);
            }
            else if (mod.name == "grid") {
                aSurface.grid(Scaled{1}, mod.color.get_or(Color{"grey80"}), Pixels{mod.line_width.get_or(1.0)});
            }
        }
    });

} // AntigenicMapsLayoutDraw::apply_mods_before

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDraw::apply_mods_after(acmacs::surface::Surface& aSurface)
{
    settings().mods.for_each([&aSurface](const auto& mod, size_t /*mod_no*/) {
        if (mod.name.is_set_or_has_default()) {
            if (mod.name == "border") {
                const auto& v = aSurface.viewport();
                aSurface.rectangle(v.origin, v.size, mod.color.get_or(BLACK), Pixels{mod.line_width.get_or(1.0) * 2});
            }
        }
    });

} // AntigenicMapsLayoutDraw::apply_mods_after

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDraw::find_sequenced_antigens()
{
    size_t hz_section_index = NodeDrawData::HzSectionNoIndex;
    auto find_antigens = [this,&hz_section_index](const Node& aNode) {
        if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex)
            hz_section_index = aNode.draw.hz_section_index;
        if (aNode.draw.chart_antigen_index)
            mSequencedAntigens[*aNode.draw.chart_antigen_index] = sequenced_antigen_t{hz_section_index, &aNode};
    };

    tree::iterate_leaf(mAntigenicMapsDraw.tree(), find_antigens);

    std::vector<size_t> antigens_per_section(20);
    for (auto ag_sec : mSequencedAntigens)
        ++antigens_per_section[ag_sec.second.section_index];
    AD_DEBUG("antigens_per_section: {}", antigens_per_section);

} // AntigenicMapsLayoutDraw::find_sequenced_antigens

// ----------------------------------------------------------------------

AntigenicMapsLayout::~AntigenicMapsLayout()
{
} // AntigenicMapsLayout::~AntigenicMapsLayout

// ----------------------------------------------------------------------

void LabelledGridBase::draw(acmacs::surface::Surface& aMappedAntigensDrawSurface, bool report_antigens_in_hz_sections)
{
    // std::cerr << "DEBUG: LabelledGridBase::draw antigenic_maps_width " << layout_draw().signature_page_settings().antigenic_maps_width << '\n';

    layout_draw().prepare_chart_for_all_sections();
    const AntigenicMapsDrawSettings& settings = layout_draw().settings();
    acmacs::surface::Surface& surface = layout_draw().surface();
    // std::cerr << "DEBUG: Maps " << surface << '\n';

    const double map_width = (surface.viewport().size.width - static_cast<double>(settings.columns - 1) * settings.gap) / static_cast<double>(settings.columns);

    size_t shown_maps = 0, row = 0, column = 0;
    for (const auto section_index: layout_draw().hz_sections().section_order) {
        const auto& section = layout_draw().hz_sections().sections[section_index];
        if (section->show && section->show_map) {
            acmacs::surface::Surface& map_surface = surface.subsurface({static_cast<double>(column) * (map_width + settings.gap), static_cast<double>(row) * (map_width + settings.gap)},
                                                                       Scaled{map_width}, layout_draw().viewport(), true);
            const std::string map_letter = layout_draw().hz_sections().node_refs[section_index].index;
            std::cout << "===============================\nINFO: MAP " << section_index << ' ' << map_letter << ' ' << section->name << '\n';
            // std::cerr << "DEBUG: Map " << map_surface << " origin_offset: " << map_surface.origin_offset() << "  scale: " << map_surface.scale() << '\n';
            layout_draw().prepare_drawing_chart(section_index, map_letter, report_antigens_in_hz_sections);
            layout_draw().draw_chart(map_surface, section_index);
            draw_mapped_antigens_section(section_index, aMappedAntigensDrawSurface);
            ++column;
            if (column >= settings.columns) {
                ++row;
                column = 0;
            }
            ++shown_maps;
        }
        else {
            std::cout << "===============================\nINFO: MAP " << section_index << ' ' << section->name << ": not shown" << '\n';
        }
    }

    if (shown_maps) {
        const double antigenic_maps_width = layout_draw().signature_page_settings().antigenic_maps_width;
        const size_t rows = shown_maps / settings.columns + ((shown_maps % settings.columns) ? 1 : 0);
        const double maps_height = map_width * static_cast<double>(rows) + static_cast<double>(rows - 1) * settings.gap;
        const double suggested_surface_width = antigenic_maps_width * surface.viewport().size.height / maps_height;
        std::cout << "Map area height: " << maps_height << '\n';
        if (std::abs((antigenic_maps_width - suggested_surface_width) / antigenic_maps_width) > 1) {
            std::cout << "Change signature_page.antigenic_maps_width from " << antigenic_maps_width << " to " << suggested_surface_width << '\n'
                      << "  If you use --init-settings changed width will be saved there" << '\n';
            layout_draw().signature_page_settings().antigenic_maps_width = suggested_surface_width;
        }
        else
            std::cout << "antigenic_maps.width is OK: " << antigenic_maps_width << " vs. suggested " << suggested_surface_width << '\n';
    }
    else {
        std::cerr << "WARNING: no maps shown!\n";
    }

} // LabelledGridBase::draw

// ----------------------------------------------------------------------

void LabelledGridBase::draw_mapped_antigens_section(size_t aSectionIndex, acmacs::surface::Surface& aMappedAntigensDrawSurface)
{
    const AntigenicMapsDrawSettings& settings = layout_draw().settings();
    const auto& section = layout_draw().hz_sections().node_refs[aSectionIndex];

    const double left = 0, right = aMappedAntigensDrawSurface.viewport().size.width;
    const double top = section.first->draw.vertical_pos, bottom = section.last->draw.vertical_pos;
    aMappedAntigensDrawSurface.line({left, top}, {right, top}, settings.mapped_antigens_section_line_color, Pixels{*settings.mapped_antigens_section_line_width}, acmacs::surface::LineCap::Square);
    aMappedAntigensDrawSurface.line({left, bottom}, {right, bottom}, settings.mapped_antigens_section_line_color, Pixels{*settings.mapped_antigens_section_line_width}, acmacs::surface::LineCap::Square);
    aMappedAntigensDrawSurface.line({right, top}, {right, bottom}, settings.mapped_antigens_section_line_color, Pixels{*settings.mapped_antigens_section_line_width}, acmacs::surface::LineCap::Square);

} // LabelledGridBase::draw_mapped_antigens_section

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
