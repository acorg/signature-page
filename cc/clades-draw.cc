#include "clades-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

void CladeData::extend(const Node& node)
{
    if (!sections.empty() && node.draw.line_no == (sections.back().last->draw.line_no + 1)) {
        sections.back().last = &node;
    }
    else {
        sections.emplace_back(&node);
    }

} // CladeData::extend

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
      // extract clades from aTree
    auto scan = [this](const Node& aNode) {
        if (aNode.draw.shown) {
            const auto* node_clades = aNode.data.clades();
            if (node_clades) {
                for (auto& c: *node_clades) {
                    auto p = mClades.emplace(c, aNode);
                    if (!p.second) { // the clade is already present, extend its range
                        p.first->second.extend(aNode);
                    }
                }
            }
        }
    };
    tree::iterate_leaf(mTree, scan);

    for (auto& c: mClades) {
        std::cerr << "Clade: " << c.first << c.second << std::endl;
          //add_clade(c.second.first, c.second.second, c.first, c.first);
    }

    assign_slots();

} // CladesDraw::prepare

// ----------------------------------------------------------------------

void CladesDraw::add_clade(const std::pair<std::string, size_t>& aBegin, const std::pair<std::string, size_t>& aEnd, std::string aLabel, std::string aId)
{
      // std::cerr << "Clade: " << aLabel << "   " << aBegin.first << ' ' << aBegin.second << " --> " << aEnd.first << ' ' << aEnd.second << std::endl;
    // Settingsclade clade(aBegin.first, aEnd.first, aBegin.second, aEnd.second, aLabel, aId);
    // const auto found = std::find_if(aSettings.per_clade.begin(), aSettings.per_clade.end(), [&](const auto& e) -> bool { return aId == e.id; });
    // if (found != aSettings.per_clade.end())
    //     clade.update(*found);
    // else
    //     hide_old_clades(clade);
    // mClades.push_back(clade);

} // CladesDraw::add_clade

// ----------------------------------------------------------------------

void CladesDraw::hide_old_clades()
{
    //   // Hide currently  unused H3 clades by default
    // if (aClade.id == "gly" || aClade.id == "no-gly" || aClade.id == "3C3b")
    //     aClade.show = false;

} // CladesDraw::hide_old_clades

// ----------------------------------------------------------------------

void CladesDraw::assign_slots()
{
    // std::sort(mClades.begin(), mClades.end(), [](const auto& a, const auto& b) -> bool { return a.begin_line == b.begin_line ? a.end_line > b.end_line : a.begin_line < b.begin_line; });
    // decltype(mClades.front().slot) slot = 0;
    // size_t last_end = 0;
    // for (auto& clade: mClades) {
    //     if (clade.slot < 0 && clade.show) {
    //         if (last_end > clade.begin_line)
    //             ++slot;         // increase slot in case of overlapping
    //         clade.slot = slot;
    //         last_end = clade.end_line;
    //     }
    // }

} // CladesDraw::assign_slots

// ----------------------------------------------------------------------

void CladesDraw::draw()
{
    mSurface.border("violet", 10);

} // CladesDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
