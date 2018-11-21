#include <set>
#include <map>
#include <tuple>

#include "clades-draw.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "settings-initializer.hh"

// ----------------------------------------------------------------------

acmacs::settings::const_array_element<CladeDrawSettings> CladesDrawSettings::for_clade(std::string name) const
{
    if (auto found = clades.find_if([&name](const CladeDrawSettings& c) -> bool { return static_cast<std::string>(c.name) == name; }); found) {
        return *found;
    }
    if (auto found = clades.find_if([](const auto& c) -> bool { return c.name.empty(); }); found) {
        return *found;
    }
    std::cerr << "DEBUG: " << clades << DEBUG_LINE_FUNC << '\n';
    throw std::runtime_error("ERROR: no clade entry with empty name (for default settings)");

} // CladesDrawSettings::for_clade

// ----------------------------------------------------------------------

void CladeData::extend(const Node& node, size_t section_inclusion_tolerance)
{
    if (!sections.empty() && node.draw.line_no <= (sections.back().last->draw.line_no + section_inclusion_tolerance + 1)) {
        sections.back().last = &node;
    }
    else {
        sections.emplace_back(&node);
    }

} // CladeData::extend

// ----------------------------------------------------------------------

void CladeData::remove_small_sections(size_t section_exclusion_tolerance)
{
    auto exclude = [&](const auto& s) -> bool {
        return (s.last->draw.line_no - s.first->draw.line_no) < section_exclusion_tolerance;
    };
    sections.erase(std::remove_if(sections.begin(), sections.end(), exclude), sections.end());

} // CladeData::remove_small_sections

// ----------------------------------------------------------------------

std::ostream& operator << (std::ostream& out, const CladeSection& section)
{
    return out << section.first->seq_id << ':' << section.first->draw.line_no << ".." << section.last->seq_id << ':' << section.last->draw.line_no;

} // operator << CladeSection

// ----------------------------------------------------------------------

std::ostream& operator << (std::ostream& out, const CladeData& clade)
{
    for (const auto& section: clade.sections) {
        out << ' ' << section;
    }
    return out;

} // operator << CladeData

// ----------------------------------------------------------------------

void CladesDraw::prepare()
{
    collect();

} // CladesDraw::prepare

// ----------------------------------------------------------------------

void CladesDraw::collect()
{
    if (mClades.empty()) {
          // extract clades from aTree
        auto scan = [this](const Node& aNode) {
            if (aNode.draw.shown) {
                const auto* node_clades = aNode.data.clades();
                if (node_clades) {
                    for (auto& c: *node_clades) {
                        auto p = mClades.emplace(c, aNode);
                        if (!p.second) { // the clade is already present, extend its range
                            const auto for_clade = mSettings.for_clade(c);
                            // std::cerr << "DEBUG: CladesDraw::collect name \"" << for_clade->name << "\" for clade \"" << c << '"' << DEBUG_LINE_FUNC << '\n';
                            // std::cerr << "DEBUG: for_clade " << *for_clade << DEBUG_LINE_FUNC << '\n';
                            // std::cerr << "DEBUG: CladesDraw::collect section_exclusion_tolerance " << for_clade->section_exclusion_tolerance << DEBUG_LINE_FUNC << '\n';
                            p.first->second.extend(aNode, for_clade->section_inclusion_tolerance);
                        }
                    }
                }
            }
        };
        tree::iterate_leaf(mTree, scan);

          // remove small sections
        for (auto& clade: mClades)
            clade.second.remove_small_sections(mSettings.for_clade(clade.first)->section_exclusion_tolerance);

        for (const auto& [name, data]: mClades) {
            const auto for_clade = mSettings.for_clade(name);
            std::cout << "INFO: Clade: " << name << data << " inclusion:" << for_clade->section_inclusion_tolerance << " exclusion:" << for_clade->section_exclusion_tolerance << '\n';
        }

        assign_slots();
    }

} // CladesDraw::collect

// ----------------------------------------------------------------------

void CladesDraw::assign_slots()
{
    std::set<CladeData::slot_type> used_slots;
      // slots forced in settings
    for (auto& clade: mClades) {
        const CladeData::slot_type forced = mSettings.for_clade(clade.first)->slot;
        if (forced != CladeDrawSettings::NoSlot) {
            clade.second.slot = forced;
            used_slots.insert(forced);
        }
    }

    CladeData::slot_type slot = 0;
    // while (used_slots.count(slot))
    //     ++slot;

    for (auto& clade: mClades) {
        if (clade.second.slot == CladeDrawSettings::NoSlot && mSettings.for_clade(clade.first)->show) {
            clade.second.slot = slot;
            used_slots.insert(slot);
            // ++slot;
            // while (used_slots.count(slot))
            //     ++slot;
        }
    }

} // CladesDraw::assign_slots

// ----------------------------------------------------------------------

void CladesDraw::init_settings(const SettingsInitializer& settings_initilizer)
{
    collect();
    settings_initilizer.update(mSettings);
    for (auto& clade : mClades)
        settings_initilizer.update(mSettings, clade);

} // CladesDraw::init_settings

// ----------------------------------------------------------------------

void CladesDraw::draw()
{
    auto draw_lines = mTimeSeriesDraw.origin_in_parent().x() < mSurface.origin_in_parent().x() ? &CladesDraw::draw_right : &CladesDraw::draw_left;

    for (const auto& name_clade: mClades) {
        const auto& clade = name_clade.second;
        const auto for_clade = mSettings.for_clade(name_clade.first);
        if (clade.shown() && for_clade->show) {
            for (const auto& section: clade.sections) {
                const double top = section.first->draw.vertical_pos - mTreeDraw.vertical_step() / 2;
                const double bottom = section.last->draw.vertical_pos + mTreeDraw.vertical_step() / 2;
                const double label_height = mSurface.text_size("W", Pixels{for_clade->label_size}, for_clade->label_style).height;
                double label_vpos{0};
                switch (static_cast<CladeDrawSettingsLabelPosition>(for_clade->label_position)) {
                  case CladeDrawSettingsLabelPosition::middle:
                      label_vpos = (top + bottom + label_height) / 2.0;
                      break;
                  case CladeDrawSettingsLabelPosition::bottom:
                      label_vpos = bottom;
                      break;
                  case CladeDrawSettingsLabelPosition::top:
                      label_vpos = top + label_height;
                      break;
                }
                (this->*draw_lines)(clade.slot, name_clade.first, top, bottom, label_vpos, *for_clade);
            }
        }
    }

} // CladesDraw::draw

// ----------------------------------------------------------------------

void CladesDraw::draw_right(CladeData::slot_type aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade)
{
    const auto x = (aSlot + 1) * mSettings.slot_width;
    mSurface.double_arrow({x, top}, {x, bottom}, for_clade.arrow_color, Pixels{for_clade.line_width}, Pixels{for_clade.arrow_width});
    std::string name = for_clade.display_name.empty() ? aCladeName : for_clade.display_name;
    mSurface.text(acmacs::Location2D{x, label_vpos} + for_clade.label_offset, name, for_clade.label_color, Pixels{for_clade.label_size}, for_clade.label_style, Rotation{for_clade.label_rotation});
    const double left = mTimeSeriesDraw.size().width;
    mSurface.line({x, top}, {-left, top}, for_clade.separator_color, Pixels{for_clade.separator_width});
    mSurface.line({x, bottom}, {-left, bottom}, for_clade.separator_color, Pixels{for_clade.separator_width});

} // CladesDraw::draw_right

// ----------------------------------------------------------------------

void CladesDraw::draw_left(CladeData::slot_type aSlot, std::string aCladeName, double top, double bottom, double label_vpos, const CladeDrawSettings& for_clade)
{
    const auto x = mSurface.viewport().size.width - (aSlot + 1) * mSettings.slot_width;
    mSurface.double_arrow({x, top}, {x, bottom}, for_clade.arrow_color, Pixels{for_clade.line_width}, Pixels{for_clade.arrow_width});
    std::string name = for_clade.display_name.empty() ? aCladeName : for_clade.display_name;
    const double label_width = mSurface.text_size(name, Pixels{for_clade.label_size}, for_clade.label_style).width;
    const acmacs::Offset label_offset{for_clade.label_offset};
    mSurface.text(acmacs::Location2D{x, label_vpos} + acmacs::Offset{- label_offset.x() - label_width, label_offset.y()},
                  name, for_clade.label_color, Pixels{for_clade.label_size}, for_clade.label_style, Rotation{for_clade.label_rotation});
    const double right = mSurface.viewport().size.width + mTimeSeriesDraw.size().width;
    mSurface.line({x, top}, {right, top}, for_clade.separator_color, Pixels{for_clade.separator_width});
    mSurface.line({x, bottom}, {right, bottom}, for_clade.separator_color, Pixels{for_clade.separator_width});

} // CladesDraw::draw_left

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
