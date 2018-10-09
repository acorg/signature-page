#include <set>
#include <map>
#include <tuple>

#include "clades-draw.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"

// ----------------------------------------------------------------------

CladeDrawSettings CladesDrawSettings::for_clade(std::string name) const
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
                              // std::cerr << "DEBUG: CladesDraw::collect for_clade " << for_clade << '\n';
                            std::cerr << "DEBUG: CladesDraw::collect name " << for_clade.name << '\n';
                            std::cerr << "DEBUG: CladesDraw::collect section_exclusion_tolerance " << for_clade.section_exclusion_tolerance << '\n';
                            p.first->second.extend(aNode, mSettings.for_clade(c).section_inclusion_tolerance);
                        }
                    }
                }
            }
        };
        tree::iterate_leaf(mTree, scan);

          // remove small sections
        for (auto& clade: mClades)
            clade.second.remove_small_sections(mSettings.for_clade(clade.first).section_exclusion_tolerance);

        for (const auto& [name, data]: mClades) {
            std::cout << "INFO: Clade: " << name << data << " inclusion:" << mSettings.for_clade(name).section_inclusion_tolerance << " exclusion:" << mSettings.for_clade(name).section_exclusion_tolerance << '\n';
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
        const CladeData::slot_type forced = mSettings.for_clade(clade.first).slot;
        if (forced != CladeDrawSettings::NoSlot) {
            clade.second.slot = forced;
            used_slots.insert(forced);
        }
    }

    CladeData::slot_type slot = 0;
    // while (used_slots.count(slot))
    //     ++slot;

    for (auto& clade: mClades) {
        if (clade.second.slot == CladeDrawSettings::NoSlot && mSettings.for_clade(clade.first).show) {
            clade.second.slot = slot;
            used_slots.insert(slot);
            // ++slot;
            // while (used_slots.count(slot))
            //     ++slot;
        }
    }

} // CladesDraw::assign_slots

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

using clade_update_t = std::tuple<std::string, int, bool>; // display_name, slot, show
using clades_update_t = std::map<std::string, clade_update_t>; // name -> [display_name, slot]
static const clades_update_t sCladesFixer = {
      // H3
    {"GLY", {"GLY", 0, false}},
    {"NO-GLY", {"NO-GLY", 0, false}},
    {"3C.3", {"3C.3", 0, true}},
    {"3C.3A", {"3C.3a", 0, true}},
    {"3C.3B", {"3C.3b", 0, true}},
    {"3C.2A", {"3C.2a", 6, true}},
    {"2A1", {"2a1", 4, true}},
    {"2A1A", {"2a1a", 0, true}},
    {"2A1B", {"2a1b", 0, true}},
    {"2A2", {"2a2", 0, true}},
    {"2A3", {"2a3", 0, true}},
    {"2A4", {"2a4", 0, true}},
      // B/Vic
    {"DEL2017", {"DEL2017", 0, false}},
    {"TRIPLEDEL2017", {"TRIPLEDEL2017", 0, false}},

};

#pragma GCC diagnostic pop

void CladesDraw::init_settings()
{
    collect();
    for (auto& clade : mClades) {
        auto matcher = [&](const auto& c) { return static_cast<std::string>(c.name) == clade.first; };
        auto get_settings_clade = [&matcher, &clade, this]() {
            if (auto settings_clade = mSettings.clades.find_if(matcher); !settings_clade) {
                auto new_clade = mSettings.clades.append();
                new_clade->name = clade.first;
                return mSettings.clades.find_if(matcher);
            }
            else
                return settings_clade;
        };

        if (auto settings_clade = get_settings_clade(); settings_clade) {
            if (const auto clade_fixer = sCladesFixer.find(clade.first); clade_fixer != sCladesFixer.end()) {
                const auto [display_name, slot, show] = clade_fixer->second;
                (*settings_clade)->display_name = display_name;
                clade.second.slot = slot;
                (*settings_clade)->show = show;
            }
            (*settings_clade)->slot = clade.second.slot;
        }
    }

} // CladesDraw::init_settings

// ----------------------------------------------------------------------

void CladesDraw::draw()
{
    auto draw_lines = mTimeSeriesDraw.origin_in_parent().x() < mSurface.origin_in_parent().x() ? &CladesDraw::draw_right : &CladesDraw::draw_left;

    for (const auto& name_clade: mClades) {
        const auto& clade = name_clade.second;
        const auto& for_clade = mSettings.for_clade(name_clade.first);
        if (clade.shown() && for_clade.show) {
            for (const auto& section: clade.sections) {
                const double top = section.first->draw.vertical_pos - mTreeDraw.vertical_step() / 2;
                const double bottom = section.last->draw.vertical_pos + mTreeDraw.vertical_step() / 2;
                const double label_height = mSurface.text_size("W", Pixels{for_clade.label_size}, for_clade.label_style).height;
                double label_vpos{0};
                switch (static_cast<CladeDrawSettingsLabelPosition>(for_clade.label_position)) {
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
                (this->*draw_lines)(clade.slot, name_clade.first, top, bottom, label_vpos, for_clade);
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
