#include "acmacs-map-draw/vaccines.hh"
#include "ace-antigenic-maps-draw.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void ChartDrawInterface::init_settings()
{

} // ChartDrawInterface::init_settings

// ----------------------------------------------------------------------

void AntigenicMapsDraw::make_layout()
{
    layout(new LabelledGrid(*this));

} // AntigenicMapsDraw::make_layout

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_apply_mods()
{
    // std::cerr << "DEBUG: [ace] AntigenicMapsLayoutDrawAce::prepare_apply_mods" << std::endl;
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "rotate_degrees") {
            chart_draw().rotate(mod.get("angle", 0.0) * M_PI / 180.0);
        }
        else if (name == "rotate_radians") {
            chart_draw().rotate(mod.get("angle", 0.0));
        }
        else if (name == "viewport") {
            chart_draw().viewport(mod.get_viewport());
        }
        else if (name == "title") {
            chart_draw().title()
                    .text_size(mod.get("text_size", 12.0))
                    .text_color(mod.get("text_color", "black"))
                    .weight(mod.get("weight", "normal"))
                    .slant(mod.get("slant", "normal"))
                    .font_family(mod.get("font_family", "san serif"))
                    .background("transparent")
                    .border_width(0)
                    .padding(mod.get("padding", 3.0))
                    .offset(mod.get("offset", Location{}))
                    .remove_all_lines();
        }
        // else if (name == "tracked_antigen") {
        //     mDrawTrackedAntigen.color(mod.get("color", "green3"));
        // }
    }
    chart_draw().calculate_viewport();

} // AntigenicMapsLayoutDrawAce::prepare_apply_mods

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections()
{
    chart_draw().prepare();
    chart_draw().mark_egg_antigens();
    chart_draw().mark_reassortant_antigens();
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "point_scale") {
            chart_draw().scale_points(mod.get("scale", 1.0), mod.get("outline_scale", 1.0));
        }
        if (name == "reference_antigens") {
            chart_draw().modify(chart().reference_antigen_indices(),
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 5.0)})
                              .fill(mod.get("fill", "transparent"))
                              .outline(mod.get("outline", "grey88"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
        else if (name == "test_antigens") {
            chart_draw().modify(chart().test_antigen_indices(),
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 3.0)})
                              .fill(mod.get("fill", "grey88"))
                              .outline(mod.get("outline", "grey88"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
    }

} // AntigenicMapsLayoutDrawAce::prepare_chart_for_all_sections

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::prepare_drawing_chart(size_t aSectionIndex)
{
    // reset tracked antigens and sera shown on the previous map
    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "sera") {
            chart_draw().modify_all_sera(PointStyleDraw(PointStyle::Empty)
                                       .size(Pixels{mod.get("size", 5.0)})
                                       .outline(mod.get("outline", "grey88"))
                                       .outline_width(Pixels{mod.get("outline_width", 0.5)}), false, true); // reset, lower sera
        }
        else if (name == "sequenced_antigens") {      // mark sequenced antigens (removes old tracked antigens marking)
            std::vector<size_t> sequenced_indices(sequenced_antigens().size());
            std::transform(sequenced_antigens().begin(), sequenced_antigens().end(), sequenced_indices.begin(), [](const auto& src) -> size_t { return src.first; });
            chart_draw().modify(sequenced_indices,
                              PointStyleDraw(PointStyle::Empty)
                              .size(Pixels{mod.get("size", 3.0)})
                              .fill(mod.get("fill", "grey63"))
                              .outline(mod.get("outline", "white"))
                              .outline_width(Pixels{mod.get("outline_width", 0.5)}));
        }
    }

    for (const auto& mod: settings().mods) {
        const std::string name = mod.name();
        if (name == "tracked_antigens") {
            std::vector<size_t> tracked_indices;
            tracked_antigens(tracked_indices, aSectionIndex);
            chart_draw().modify(tracked_indices,
                                PointStyleDraw(PointStyle::Empty)
                                .size(Pixels{mod.get("size", 5.0)})
                                .fill(mod.get("fill", "grey63"))
                                .outline(mod.get("outline", "white"))
                                .outline_width(Pixels{mod.get("outline_width", 0.5)}), true);
        }
        else if (name == "tracked_sera") {
            std::map<size_t, std::vector<size_t>> tracked_indices;
            tracked_sera(tracked_indices, aSectionIndex);
            std::cerr << "INFO: tracked_sera: " << tracked_indices << std::endl;
            chart_draw().modify_sera(tracked_indices.begin(), tracked_indices.end(),
                PointStyleDraw(PointStyle::Empty)
                .size(Pixels{mod.get("size", 5.0)})
                .outline(mod.get("outline", "black"))
                .outline_width(Pixels{mod.get("outline_width", 0.5)}), true);
        }
        else if (name == "tracked_serum_circles") {
            std::map<size_t, std::vector<size_t>> tracked_indices;
            tracked_sera(tracked_indices, aSectionIndex);
        }
        else if (name == "vaccines") {
            mark_vaccines(mod);
        }
    }

      // serum circles
      // marked antigens
      // tracked_antigen_colored_by_clade

    const HzSection& section = hz_sections().sections[aSectionIndex];
    std::string title = section.index + "."; // std::string(1, 'A' + static_cast<char>(aSectionNo)) + ".";
    if (!section.label.empty())
        title += " " + section.label;
    chart_draw().title().remove_all_lines().add_line(title);

} // AntigenicMapsLayoutDrawAce::prepare_drawing_chart

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_antigens(std::vector<size_t>& tracked_indices, size_t aSectionIndex) const
{
    for (const auto& sequenced_section: sequenced_antigens()) {
        if (sequenced_section.second == aSectionIndex)
            tracked_indices.push_back(sequenced_section.first);
    }

} // AntigenicMapsLayoutDrawAce::tracked_antigens

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::tracked_sera(std::map<size_t, std::vector<size_t>>& tracked_indices, size_t aSectionIndex) const
{
    const auto& chrt = chart();
    if (!mHomologousAntigenForSeraFound) {
        chrt.find_homologous_antigen_for_sera_const();
        mHomologousAntigenForSeraFound = true;
    }

    std::vector<size_t> tracked_antigen_indices;
    tracked_antigens(tracked_antigen_indices, aSectionIndex);

    for (size_t serum_no = 0; serum_no < chrt.number_of_sera(); ++serum_no) {
        const std::vector<size_t>& homologous_antigens_for_serum = chrt.serum(serum_no).homologous();
        for (size_t antigen_no: homologous_antigens_for_serum) {
            if (std::find(tracked_antigen_indices.begin(), tracked_antigen_indices.end(), antigen_no) != tracked_antigen_indices.end()) {
                tracked_indices[serum_no] = homologous_antigens_for_serum;
                break;
            }
        }
    }

} // AntigenicMapsLayoutDrawAce::tracked_sera

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::mark_vaccines(const AntigenicMapMod& vaccine_mod)
{
    try {
        const hidb::HiDb& hidb = mHiDbSet.get(chart_draw().chart().chart_info().virus_type());
        Vaccines vaccs{chart_draw().chart(), hidb};
        for (const SettingValue& mod_v: vaccine_mod.get_mods()) {
            const SettingDict& mod = boost::get<SettingDict>(mod_v);
            const std::string type = mod.get("type", ""), passage = mod.get("passage", ""), name = mod.get("name", "");
            std::unique_ptr<VaccineMatcher> matcher{vaccs.match(name, type, passage)};
            for (const auto& item: mod) {
                if (item.first == "size")
                    matcher->size(SettingValue_get(item.second, 0.0));
                else if (item.first == "fill")
                    matcher->fill(SettingValue_get(item.second, std::string{}));
                else if (item.first == "outline")
                    matcher->outline(SettingValue_get(item.second, std::string{}));
                else if (item.first == "outline_width")
                    matcher->outline_width(SettingValue_get(item.second, 0.0));
                else if (item.first == "aspect")
                    matcher->aspect(SettingValue_get(item.second, 0.0));
                else if (item.first == "rotation")
                    matcher->rotation(SettingValue_get(item.second, 0.0));
                else if (item.first == "no")
                    matcher->no(SettingValue_get(item.second, static_cast<size_t>(0)));
                else if (item.first == "show")
                    matcher->show(SettingValue_get(item.second, true));
                else if (item.first == "label")
                    add_label(std::shared_ptr<VaccineMatcherLabel>{matcher->label(chart_draw(), hidb.locdb())}, SettingValue_get(item.second, SettingDict{}));
                else if (item.first != "type" && item.first != "passage" && item.first != "name" && (item.first.empty() || (item.first.front() != '?' && item.first.back() != '?')))
                    std::cerr << "WARNING: mark_vaccines: unrecognized key \"" << item.first << '"' << std::endl;
            }
        }
          // std::cerr << "DEBUG: Vaccines:" << std::endl << vaccs.report(2) << std::endl;
        vaccs.plot(chart_draw());
    }
    catch (hidb::NoHiDb&) {
        std::cerr << "WARNING: cannot mark vaccines: no hidb for \"" << chart_draw().chart().chart_info().virus_type() << "\"" << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::mark_vaccines

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::add_label(std::shared_ptr<VaccineMatcherLabel> label, const SettingDict& data)
{
    for (const auto& item: data) {
        if (item.first == "size")
            label->size(SettingValue_get(item.second, 0.0));
        else if (item.first == "color")
            label->color(SettingValue_get(item.second, std::string{}));
        else if (item.first == "font_family")
            label->font_family(SettingValue_get(item.second, std::string{}));
        else if (item.first == "name_type")
            label->name_type(SettingValue_get(item.second, std::string{}));
        else if (item.first == "slant")
            label->slant(SettingValue_get(item.second, std::string{}));
        else if (item.first == "weight")
            label->weight(SettingValue_get(item.second, std::string{}));
        else if (item.first == "offset") {
            const SettingList& offset = SettingValue_get(item.second, SettingList{0.0, 1.0});
            label->offset(SettingValue_get(offset[0], 0.0), SettingValue_get(offset[1], 1.0));
        }
        else if (item.first.empty() || (item.first.front() != '?' && item.first.back() != '?'))
            std::cerr << "WARNING: mark_vaccines label: unrecognized key \"" << item.first << '"' << std::endl;
    }

} // AntigenicMapsLayoutDrawAce::add_label

// ----------------------------------------------------------------------

void AntigenicMapsLayoutDrawAce::draw_chart(Surface& aSurface, size_t aSectionIndex)
{
    apply_mods_before(aSurface);

      // aSurface.viewport(viewport());

    chart_draw().draw(aSurface);

    apply_mods_after(aSurface);

} // AntigenicMapsLayoutDrawAce::draw_chart

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
