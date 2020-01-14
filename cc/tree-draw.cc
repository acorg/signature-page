#include <fstream>
#include <algorithm>
#include <iomanip>
#include <random>

#include "acmacs-base/timeit.hh"
#include "acmacs-base/range.hh"
#include "acmacs-draw/surface.hh"
#include "hidb-5/vaccines.hh"
#include "tree-draw.hh"
#include "tree.hh"
#include "coloring.hh"
#include "settings-initializer.hh"
#include "signature-page.hh"

// ----------------------------------------------------------------------

TreeDraw::TreeDraw(SignaturePageDraw& aSignaturePageDraw, acmacs::surface::Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings, HzSections& aHzSections)
    : mSignaturePageDraw(aSignaturePageDraw), mSurface(aSurface), mTree(aTree), mSettings(aSettings), mHzSections(aHzSections)
{
    make_coloring();
}

// ----------------------------------------------------------------------

// explicit destructor made here to allow proper mColoring destruction
TreeDraw::~TreeDraw()
{

} // TreeDraw::~TreeDraw

// ----------------------------------------------------------------------

void TreeDraw::make_coloring()
{
    const std::string color_nodes = mSettings.color_nodes;
    if (color_nodes == "black")
        mColoring = std::unique_ptr<Coloring>(new ColoringBlack());
    else if (color_nodes == "continent")
        mColoring = std::unique_ptr<Coloring>(new ColoringByContinent());
    else {
        try {
            auto coloring = std::make_unique<ColoringByPos>(std::stoul(std::string(color_nodes)));
            if (const std::map<std::string, std::string> color_for_aa = mSettings.color_for_aa; !color_for_aa.empty())
                coloring->color_for_aa(color_for_aa);
            mColoring = std::move(coloring);
        }
        catch (std::exception&) {
            throw std::runtime_error("Unrecognized TreeDrawSettings.color_nodes: " + std::string(color_nodes));
        }
    }

} // TreeDraw::make_coloring

// ----------------------------------------------------------------------

void TreeDraw::init_settings(const Clades* aClades, const SettingsInitializer& settings_initilizer, bool whocc_support)
{
    mInitializeSettings = true;
    if (hz_sections().sections.empty())
        mHzSections.detect_hz_lines_for_clades(mTree, aClades, true);
    mHzSections.vertical_gap = 15;
    tree().make_aa_transitions();
    settings_initilizer.update(*this, whocc_support);

} // TreeDraw::init_settings

// ----------------------------------------------------------------------

void TreeDraw::detect_hz_lines_for_clades(const Clades* aClades, bool aForce)
{
    mHzSections.detect_hz_lines_for_clades(mTree, aClades, aForce);

} // TreeDraw::detect_hz_lines_for_clades

// ----------------------------------------------------------------------

void TreeDraw::prepare()
{
    mTree.set_continents();
    ladderize();
    mTree.make_aa_transitions();

    size_t number_of_hz_sections = prepare_hz_sections();
    if (number_of_hz_sections == 0 || !mHzSections.show)
        number_of_hz_sections = 1;
    const auto& canvas_size = mSurface.viewport().size;
    mHorizontalStep = (canvas_size.width - mSettings.right_padding) / mTree.width();
    mVerticalStep = (canvas_size.height - (number_of_hz_sections - 1) * mHzSections.vertical_gap) / static_cast<double>(mTree.height() + 2); // +2 to add space at the top and bottom
    set_vertical_pos();

    // const auto [virus_type, lineage] = mTree.virus_type_lineage();
    // if (!virus_type.empty()) {
    //     // std::cerr << "DEBUG: vaccines " << virus_type << ' ' << lineage << '\n';
    //     const auto& vaccines = hidb::vaccine_names(virus_type, lineage);
    //     for (const auto& vac : vaccines) {
    //         std::cerr << "DEBUG: vaccine " << vac.name << '\n';
    //         if (const auto nodes = mTree.find_nodes_matching(vac.name); !nodes.empty()) {
    //             std::cerr << "DEBUG: vaccine " << vac.name << ' ' << nodes.size() << '\n';
    //             for (const auto* node : nodes)
    //                 std::cerr << "   " << node->seq_id << ' ' << node->draw.line_no << '\n';
    //         }
    //     }
    // }
    // else
    //     std::cerr << "WARNING: no virus_type inferred from the names in the tree\n";

} // TreeDraw::prepare

// ----------------------------------------------------------------------

void TreeDraw::ladderize()
{
    mTree.set_number_strains();
    mTree.ladderize(mSettings.ladderize);
    mTree.compute_cumulative_edge_length();
    if (apply_mods()) {
          // nodes were hidden:
        mTree.set_number_strains();
        mTree.ladderize(mSettings.ladderize);
        mTree.compute_cumulative_edge_length();
    }
    set_line_no();

} // TreeDraw::ladderize

// ----------------------------------------------------------------------

bool TreeDraw::apply_mods()
{
    (mSettings).mods.for_each([this] (const auto& mod, size_t mod_no) { // const_cast to support situation when mods was not set
        const auto mod_mod = static_cast<std::string>(mod.mod);
        if (mod_mod == "root") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.s1 << '\n';
            mTree.re_root(mod.s1);
        }
        else if (mod_mod == "hide-isolated-before") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.s1 << '\n';
            hide_isolated_before(mod.s1);
        }
        else if (mod_mod == "hide-if-cumulative-edge-length-bigger-than") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.d1 << '\n';
            hide_if_cumulative_edge_length_bigger_than(mod.d1);
        }
        else if (mod_mod == "before2015-58P-or-146I-or-559I") {
            std::cout << "TREE-mod: " << mod_mod << '\n';
            hide_before2015_58P_or_146I_or_559I();
        }
        else if (mod_mod == "hide-between") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\"" << '\n';
            hide_between(mod.s1, mod.s2);
        }
        else if (mod_mod == "hide-one") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\"" << '\n';
            hide_one(mod.s1);
        }
        else if (mod_mod == "hide-not-found-in-chart") {
            std::cout << "TREE-mod: " << mod_mod << '\n';
            hide_not_found_in_chart();
        }
        else if (mod_mod == "mark-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\" " << mod.d1 << '\n';
            mark_with_line(mod.s1, Color(mod.s2), Pixels{mod.d1});
        }
        else if (mod_mod == "mark-aa-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\" " << mod.d1 << '\n';
            mark_aa_with_line(mod.s1, Color(mod.s2), Pixels{mod.d1}, mod.report);
        }
        else if (mod_mod == "mark-clade-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.clade << "\" \"" << mod.color << "\" " << mod.line_width << '\n';
            mark_clade_with_line(mod.clade, Color(mod.color), Pixels{mod.line_width}, mod.report);
        }
        else if (mod_mod == "mark-country-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.country << "\" \"" << mod.color << "\" " << mod.line_width << '\n';
            mark_country_with_line(mod.country, Color(mod.color), Pixels{mod.line_width}, mod.report);
        }
        else if (mod_mod == "mark-location-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.location << "\" \"" << mod.color << "\" " << mod.line_width << '\n';
            mark_location_with_line(mod.location, Color(mod.color), Pixels{mod.line_width}, mod.report);
        }
        else if (mod_mod == "mark-having-serum-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.color << "\" " << mod.line_width << '\n';
            mark_having_serum_with_line(Color(mod.color), Pixels{mod.line_width});
        }
        else if (mod_mod == "mark-with-label") {
            mark_with_label(mod, mod_no);
        }
        else if (mod_mod.empty() || mod_mod[0] == '?') {
              // commented out mod
        }
        else
            throw std::runtime_error("Unrecognized tree mod: " + std::string(mod_mod));
    });
    return !mSettings.mods.empty();

} // TreeDraw::apply_mods

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    fmt::print("Tree surface: {}\n", mSurface.viewport());
    const double line_width = mSettings.line_width;
    mLineWidth = mSettings.force_line_width ? line_width : std::min(line_width, mVerticalStep * 0.5);
    fit_labels_into_viewport();

    double vertical_gap = 0;
    draw_node(mTree, 0 /*mLineWidth / 2*/, vertical_gap, mSettings.root_edge);
    report_aa_transitions();
    mColoring->report();
    draw_legend();

    // mark_nodes(aSurface);

} // TreeDraw::draw

// ----------------------------------------------------------------------

// void TreeDraw::unhide()
// {
//     auto show_leaf = [](Node& aNode) {
//         aNode.draw.shown = true;
//     };

//     tree::iterate_leaf(mTree, show_leaf);

// } // TreeDraw::unhide

// ----------------------------------------------------------------------

void TreeDraw::hide_isolated_before(std::string aDate)
{
    auto hide_show_leaf = [aDate](Node& aNode) {
        aNode.draw.shown &= aNode.data.date() >= aDate;
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);

} // TreeDraw::hide_isolated_before

// ----------------------------------------------------------------------

void TreeDraw::hide_if_cumulative_edge_length_bigger_than(double aThreshold)
{
    auto hide_show_leaf = [aThreshold](Node& aNode) {
        aNode.draw.shown &= aNode.data.cumulative_edge_length <= aThreshold;
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);

} // TreeDraw::hide_if_cumulative_edge_length_bigger_than

// ----------------------------------------------------------------------

void TreeDraw::hide_before2015_58P_or_146I_or_559I()
{
    auto hide_show_leaf = [](Node& aNode) {
        if (aNode.data.has_sequence() && aNode.data.date() < "2015-01-01") {
            const auto aa = aNode.data.amino_acids();
            if (aa[57] == 'P' || aa[145] == 'I' || aa[559] == 'I')
                aNode.draw.shown = false;
        }

    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);

} // TreeDraw::hide_before2015_58P_or_146I_or_559I

// ----------------------------------------------------------------------

void TreeDraw::hide_between(std::string aFirst, std::string aLast)
{
    bool hiding = false;
    bool cancelled = false;
    size_t hidden = 0;
    auto hide_show_leaf = [aFirst,aLast,&hiding,&hidden,&cancelled](Node& aNode) {
        if (!cancelled) {
            if (aNode.seq_id == aFirst) {
                  // std::cerr << "DEBUG: hide_between first " << aNode.draw.line_no << ' ' << aFirst << '\n';
                if (hiding) {
                    std::cerr << "WARNING: tree hide_between: first node found and hiding is active: " << aFirst << " (hiding cancelled)\n";
                    cancelled = true;
                      // throw std::runtime_error("tree hide_between: first node found and hiding is active: " + aFirst);
                }
                hiding = true;
                aNode.draw.shown = false;
                ++hidden;
            }
            else if (aNode.seq_id == aLast) {
                  // std::cerr << "DEBUG: hide_between last " << aNode.draw.line_no << ' ' << aLast << '\n';
                if (!hiding) {
                    std::cerr << "WARNING: tree hide_between: last node found and hiding is not active: " << aLast << " (hiding cancelled)\n";
                    cancelled = true;
                      //throw std::runtime_error("tree hide_between: last node found and hiding is not active: " + aLast);
                }
                hiding = false;
                aNode.draw.shown = false; // hide the last node
                ++hidden;
            }
            else if (hiding) {
                  // std::cerr << "DEBUG: hide_between hide " << aNode.draw.line_no << ' ' << aNode.seq_id << '\n';
                aNode.draw.shown = false;
                ++hidden;
            }
        }
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);
    if (!cancelled) {
        if (hiding)
            throw std::runtime_error("tree hide_between: last node not found: " + aLast);
        if (hidden == 0)
            throw std::runtime_error("tree hide_between: no nodes hidden");
        std::cout << "INFO: hide_between [" << aFirst << "] [" << aLast << "]: leaf nodes hidden: " << hidden << '\n';
    }
    else {
        std::cerr << "WARNING: node hiding cancelled\n";
    }

} // TreeDraw::hide_between

// ----------------------------------------------------------------------

void TreeDraw::hide_one(std::string aName)
{
    size_t hidden = 0;
    auto hide_show_leaf = [aName, &hidden](Node &aNode) {
        if (aNode.seq_id == aName) {
            aNode.draw.shown = false;
            ++hidden;
        }
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);
    if (hidden == 0)
        throw std::runtime_error("tree hide_one: no nodes hidden");
    std::cout << "INFO: hide_one " << aName << ": leaf nodes hidden: " << hidden << '\n';

} // TreeDraw::hide_one

// ----------------------------------------------------------------------

void TreeDraw::hide_not_found_in_chart()
{
    size_t hidden = 0;
    auto hide_show_leaf = [&hidden](Node &aNode) {
        if (!aNode.draw.chart_antigen_index) {
            aNode.draw.shown = false;
            ++hidden;
        }
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);
    if (hidden == 0)
        throw std::runtime_error("tree hide_not_found_in_chart: no nodes hidden");
    std::cout << "INFO: hide_not_found_in_chart: leaf nodes hidden: " << hidden << '\n';

} // TreeDraw::hide_not_found_in_chart

// ----------------------------------------------------------------------

void TreeDraw::mark_with_line(std::string aName, Color aColor, Pixels aLineWidth)
{
    size_t marked = 0;
    auto mark_leaf = [aName,&aColor,&aLineWidth,&marked](Node& aNode) {
        if (aNode.seq_id == aName) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
        }
    };
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: not found to mark with line: " << aName << '\n';
    else
        std::cout << "leaf nodes marked: " << marked << '\n';

} // TreeDraw::mark_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_aa_with_line(std::string aPos1AA, Color aColor, Pixels aLineWidth, bool aReport)
{
    size_t marked = 0;
    auto mark_leaf = [list_pos1_aa=acmacs::seqdb::extract_aa_at_pos1_eq_list(aPos1AA),leaf_no=0,&aColor,&aLineWidth,&marked,reported=false,aReport](Node& aNode) mutable {
        ++leaf_no;
        if (aNode.data.matches(list_pos1_aa)) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
            if (aReport) {
                std::cout << "  " << std::setw(4) << leaf_no << ' ' << aNode.seq_id << '\n';
                reported = true;
            }
        }
        else if (reported) {
            if (aReport)
                std::cout << "  --post-- " << leaf_no << ' ' << aNode.seq_id << "\n\n";
            reported = false;
        }
    };

    if (aReport)
        std::cout << aPos1AA << '\n';
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: no nodes found to mark with line for AA: " << aPos1AA << '\n';
    else
        std::cout << '"' << aPos1AA << "\" leaf nodes marked: " << marked << '\n';

} // TreeDraw::mark_aa_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_with_label(const TreeDrawMod& aMod, size_t mod_no)
{
    const auto warn = [&aMod](std::string msg) { std::cerr << "WARNING: cannot mark-with-label seq_id:\"" << aMod.seq_id << "\" name:\"" << aMod.name << "\": " << msg << '\n'; };

    std::vector<Node*> nodes;
    if (!aMod.seq_id.empty()) {
        if (Node* node = mTree.find_leaf_by_seqid(aMod.seq_id); node)
            nodes.push_back(node);
    }
    else if (!aMod.name.empty())
        nodes = mTree.find_nodes_matching(aMod.name);

    if (nodes.empty())
        warn("no nodes matched");
    for (Node* node : nodes) {
        if (node->draw.shown) {
            node->draw.mark_with_label = mod_no;
            std::cout << "INFO: mark-with-label: " << aMod.seq_id << " \"" << aMod.label << "\" --> \"" << node->seq_id << "\"\n"; // line_no:" << node->draw.line_no << '\n';
        }
        else {
            warn("node " + node->seq_id + " not shown");
        }
    }

} // TreeDraw::mark_with_label

// ----------------------------------------------------------------------

void TreeDraw::mark_clade_with_line(std::string aClade, Color aColor, Pixels aLineWidth, bool aReport)
{
    size_t marked = 0;
    bool reported = false;
    auto mark_leaf = [aClade,&aColor,&aLineWidth,&marked,&reported,aReport,leaf_no=0](Node& aNode) mutable {
        if (aNode.data.has_clade(aClade)) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
            if (aReport) {
                std::cout << "  " << leaf_no << ' ' << aNode.seq_id << '\n';
                reported = true;
            }
        }
        else if (reported) {
            if (aReport)
                std::cout << "      past-end: " << leaf_no << ' ' << aNode.seq_id << "\n\n";
            reported = false;
        }
        ++leaf_no;
    };

    if (aReport)
        std::cout << aClade << '\n';
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: no nodes found to mark with line for clade: " << aClade << '\n';
    else
        std::cout << "Clade " << aClade << " leaf nodes marked: " << marked << '\n';

} // TreeDraw::mark_clade_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_country_with_line(std::string aCountry, Color aColor, Pixels aLineWidth, bool aReport)
{
    size_t marked = 0;
    bool reported = false;
    auto mark_leaf = [aCountry,&aColor,&aLineWidth,&marked,&reported,aReport,leaf_no=0](Node& aNode) mutable {
        if (aNode.data.country() == aCountry) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
            if (aReport) {
                std::cout << "  " << leaf_no << ' ' << aNode.seq_id << '\n';
                reported = true;
            }
        }
        else if (reported) {
            if (aReport)
                std::cout << "      past-end: " << leaf_no << ' ' << aNode.seq_id << "\n\n";
            reported = false;
        }
        ++leaf_no;
    };

    if (aReport)
        std::cout << aCountry << '\n';
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: no nodes found to mark with line for country: " << aCountry << '\n';
    else
        std::cout << "Country " << aCountry << " leaf nodes marked: " << marked << '\n';


} // TreeDraw::mark_country_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_location_with_line(std::string aLocation, Color aColor, Pixels aLineWidth, bool aReport)
{
    size_t marked = 0;
    bool reported = false;
    auto mark_leaf = [aLocation,&aColor,&aLineWidth,&marked,&reported,aReport,leaf_no=0](Node& aNode) mutable {
        if (aNode.data.location() == aLocation) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
            if (aReport) {
                std::cout << "  " << leaf_no << ' ' << aNode.seq_id << '\n';
                reported = true;
            }
        }
        else if (reported) {
            if (aReport)
                std::cout << "      past-end: " << leaf_no << ' ' << aNode.seq_id << "\n\n";
            reported = false;
        }
        ++leaf_no;
    };

    if (aReport)
        std::cout << aLocation << '\n';
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: no nodes found to mark with line for location: " << aLocation << '\n';
    else
        std::cout << "Location " << aLocation << " leaf nodes marked: " << marked << '\n';


} // TreeDraw::mark_location_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_having_serum_with_line(Color aColor, Pixels aLineWidth)
{
    if (mSignaturePageDraw.has_antigenic_maps_draw()) {
        const auto& chart = mSignaturePageDraw.antigenic_maps_draw().chart().chart();
        chart.set_homologous(acmacs::chart::find_homologous::relaxed_strict);
        std::vector<bool> antigens_with_homologous_serum(chart.number_of_antigens(), false);
        auto sera = chart.sera();
        for (auto serum : *sera) {
            for (auto ag_no : serum->homologous_antigens())
                antigens_with_homologous_serum[ag_no] = true;
        }
        std::vector<std::string> marked;
        auto mark_leaf = [&antigens_with_homologous_serum, &aColor, &aLineWidth, &marked](Node& aNode) {
            if (aNode.draw.chart_antigen_index && antigens_with_homologous_serum[*aNode.draw.chart_antigen_index]) {
                aNode.draw.mark_with_line = aColor;
                aNode.draw.mark_with_line_width = aLineWidth;
                marked.push_back(aNode.seq_id);
            }
        };
        tree::iterate_leaf(mTree, mark_leaf);
        if (marked.empty())
            std::cerr << "WARNING: no nodes found to mark with line for antigens in chart having serum\n";
        else {
            std::cout << "INFO: leafs having serum marked: " << marked.size() << '\n';
            for (const auto& name : marked)
                std::cout << "    " << name << '\n';
        }
    }

} // TreeDraw::mark_having_serum_with_line

// ----------------------------------------------------------------------

void TreeDraw::hide_branch(Node& aNode)
{
    aNode.draw.shown = false;
    for (const auto& node: aNode.subtree) {
        if (node.draw.shown) {
            aNode.draw.shown = true;
            break;
        }
    }

} // TreeDraw::hide_branch

// ----------------------------------------------------------------------

// void TreeDraw::hide_leaves(bool aForce)
// {
//     std::cout << "TREE: hide_leaves " << aForce << '\n';
//     if (aForce || !hiding_leaves_done) {
//         auto hide_show_leaf = [this](Node& aNode) {
//             aNode.draw.shown = true; // ! (aNode.data.date() < mSettings.hide_isolated_before || aNode.data.cumulative_edge_length > mSettings.hide_if_cumulative_edge_length_bigger_than || this->hide_leaf_if(aNode));
//         };

//         auto hide_show_branch = [](Node& aNode) {
//             aNode.draw.shown = false;
//             for (const auto& node: aNode.subtree) {
//                 if (node.draw.shown) {
//                     aNode.draw.shown = true;
//                     break;
//                 }
//             }
//         };

//           // mTree.compute_cumulative_edge_length();
//         tree::iterate_leaf_post(mTree, hide_show_leaf, hide_show_branch);
//         hiding_leaves_done = true;
//     }

// } // TreeDraw::hide_leaves

// ----------------------------------------------------------------------

void TreeDraw::set_line_no()
{
    size_t current_line = sFirstLineNo;    // line of the first node is 1, we have 1 line space at the top and bottom of the tree
    auto set_line_no = [&current_line](Node& aNode) {
        if (aNode.draw.shown) {
            aNode.draw.line_no = current_line;
            ++current_line;
        }
    };
    tree::iterate_leaf(mTree, set_line_no);
    if (auto& last_leaf = find_last_leaf(mTree); last_leaf.draw.line_no == 0) // last leaf is perhaps hidden but we need its line_no later to figure out correct shown tree height
        last_leaf.draw.line_no = current_line - 1;
    std::cout << "INFO: TREE-lines: " << (current_line - 1) << '\n';

} // TreeDraw::set_line_no

// ----------------------------------------------------------------------

void TreeDraw::set_vertical_pos()
{
    // std::cerr << "DEBUG: set_vertical_pos" << '\n';
    double vertical_pos = mVerticalStep;
    bool topmost_node = true;
    auto set_leaf_vertical_pos = [&](Node& aNode) {
        if (aNode.draw.shown) {
            if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex && mHzSections.show && mHzSections.sections[aNode.draw.hz_section_index]->show) {
                std::cout << "INFO: TREE-hz-section: " << aNode.draw.hz_section_index << " " << aNode.seq_id << '\n'; // ' ' << *mHzSections.sections[aNode.draw.hz_section_index] << '\n';
                if (!topmost_node)
                    vertical_pos += mHzSections.vertical_gap;
            }
            aNode.draw.vertical_pos = vertical_pos;
            vertical_pos += mVerticalStep;
            topmost_node = false;
        }
    };
    tree::iterate_leaf(mTree, set_leaf_vertical_pos);

    auto set_intermediate_vertical_pos = [&](Node& aNode) {
        if (aNode.draw.shown) {
            double top = -1, bottom = -1;
            for (const auto& subnode: aNode.subtree) {
                if (subnode.draw.shown) {
                    bottom = subnode.draw.vertical_pos;
                    if (top < 0)
                        top = bottom;
                }
            }
            aNode.draw.vertical_pos = (top + bottom) / 2;
        }
    };
    tree::iterate_post(mTree, set_intermediate_vertical_pos);

} // TreeDraw::set_vertical_pos

// ----------------------------------------------------------------------

size_t TreeDraw::prepare_hz_sections()
{
    mHzSections.convert_aa_transitions(mTree); // to name based hz sections
    mHzSections.sort(mTree);

    size_t number_of_hz_sections = 0;
      // const Node& first_leaf = find_first_leaf(mTree);
    for (auto section_index: mHzSections.section_order) {
        const auto section = mHzSections.sections[section_index];
        if (section->show) {
            Node* section_start = mTree.find_leaf_by_seqid(section->name);
            if (section_start) {
                if (section_start->draw.shown) {
                    section_start->draw.hz_section_index = section_index;
                    ++number_of_hz_sections;
                }
                else {
                    std::cerr << "WARNING: HzSection ignored because its node is hidden: " << section->name << '\n';
                }
            }
            else {
                std::cerr << "WARNING: HzSection seq_id not found: " << section->name << '\n';
            }
        }
    }
    // if (number_of_hz_sections == 0)
    //     number_of_hz_sections = 1;
    // std::cout << "INFO: HZ sections: " << number_of_hz_sections << ' ' << mHzSections.section_order.size() << '\n';
    mHzSections.report(std::cout);
    return number_of_hz_sections;

} // TreeDraw::prepare_hz_sections

// ----------------------------------------------------------------------

void TreeDraw::calculate_name_offset()
{
    const auto tsize = mSurface.text_size("W", mFontSize, mSettings.label_style);
    mNameOffset = mSettings.name_offset * tsize.width;

} // TreeDraw::calculate_name_offset

// ----------------------------------------------------------------------

void TreeDraw::fit_labels_into_viewport()
{
    mFontSize = mVerticalStep;

    const double canvas_width = mSurface.viewport().size.width;
    fmt::print("INFO: viewport: {}\n", mSurface.viewport());

    // Timeit ti("DEBUG: TreeDraw::fit_labels_into_viewport: ");
    for (double label_offset = max_label_offset(); label_offset > canvas_width; label_offset = max_label_offset()) {
        const double scale = std::min(canvas_width / label_offset, 0.99); // to avoid too much looping
        fmt::print("INFO: canvas:{} label_right:{} scale:{}\n", canvas_width, label_offset, scale);
        mHorizontalStep *= scale;
        mFontSize *= scale;
    }

} // TreeDraw::fit_labels_into_viewport

// ----------------------------------------------------------------------

double TreeDraw::max_label_offset()
{
    calculate_name_offset();

    double max_label_origin = 0, max_label_right = 0;
    auto label_offset = [&](Node& node) {
        if (node.draw.shown) {
            const double label_origin = node.data.cumulative_edge_length * mHorizontalStep + mNameOffset;
            max_label_origin = std::max(max_label_origin, label_origin);
            max_label_right = std::max(max_label_right, label_origin + this->text_width(node.display_name()));
        }
    };

    // Timeit ti("TreeDraw::max_label_offset: ");
    tree::iterate_leaf(mTree, label_offset);
    return max_label_right;

} // TreeDraw::max_label_offset

// ----------------------------------------------------------------------

void TreeDraw::draw_node(const Node& aNode, double aOriginX, double& aVerticalGap, double aEdgeLength)
{
    if (aNode.draw.shown) {
        acmacs::Size size{(aEdgeLength < 0.0 ? aNode.edge_length : aEdgeLength) * mHorizontalStep, 0};
        const double right = aOriginX + size.width;

        if (aNode.is_leaf()) {
            const std::string text = aNode.display_name();
            const auto tsize = mSurface.text_size(text, mFontSize, mSettings.label_style);
            const acmacs::PointCoordinates text_origin(right + mNameOffset, aNode.draw.vertical_pos + tsize.height / 2);
            mSurface.text(text_origin, text, mColoring->color(aNode), mFontSize, mSettings.label_style);
            if (text_origin.x() < 0 || text_origin.y() < 0)
                fmt::print(stderr, "WARNING: bad origin for a node label: {} \"{}\" mNameOffset:{} aOriginX:{}\n", text_origin, text, mNameOffset, aOriginX);

            if (!aNode.draw.mark_with_line.empty()) {
                // mSurface.line({text_origin.x() + tsize.width, text_origin.y}, {mSurface.viewport().size.width, text_origin.y}, aNode.draw.mark_with_line, aNode.draw.mark_with_line_width);
                mSurface.line({mSurface.viewport().size.width - 10, text_origin.y()}, {mSurface.viewport().size.width, text_origin.y()}, aNode.draw.mark_with_line, aNode.draw.mark_with_line_width);
            }
            draw_mark_with_label(aNode, text_origin);
        }
        else {
            double top = -1, bottom = -1;
            for (auto& node: aNode.subtree) {
                if (node.draw.shown) {
                    draw_node(node, right, aVerticalGap);
                    if (top < 0)
                        top = node.draw.vertical_pos;
                    if (node.draw.vertical_pos > bottom)
                        bottom = node.draw.vertical_pos;
                }
            }
            mSurface.line({right, top}, {right, bottom}, mSettings.line_color, mLineWidth, acmacs::surface::LineCap::Square);
        }
        mSurface.line({aOriginX, aNode.draw.vertical_pos}, {right, aNode.draw.vertical_pos}, mSettings.line_color, mLineWidth);
        draw_aa_transition(aNode, {aOriginX, aNode.draw.vertical_pos}, right);
    }

} // TreeDraw::draw_node

// ----------------------------------------------------------------------

void TreeDraw::draw_aa_transition(const Node& aNode, const acmacs::PointCoordinates& aOrigin, double aRight)
{
    auto& settings = mSettings.aa_transition;
    const auto& first_leaf = find_first_leaf(aNode);
    if (settings->show && !aNode.data.aa_transitions.empty() && aNode.data.number_strains >= settings->number_strains_threshold) {
        if (auto labels = aNode.data.aa_transitions.make_labels(settings->show_empty_left); !labels.empty()) {
            if (const auto /*not ref! */ branch_settings = settings->per_branch->settings_for_label(labels, first_leaf.seq_id); branch_settings.show) {
                const auto longest_label = std::max_element(labels.begin(), labels.end(), [](const auto& a, const auto& b) { return a.first.size() < b.first.size(); });
                const auto longest_label_size = mSurface.text_size(longest_label->first, Pixels{branch_settings.size}, branch_settings.style);
                const auto node_line_width = aRight - aOrigin.x();
                acmacs::Offset offset{node_line_width > longest_label_size.width ? (node_line_width - longest_label_size.width) / 2 : (node_line_width - longest_label_size.width),
                                      longest_label_size.height * branch_settings.interline};
                offset += branch_settings.label_offset;
                acmacs::PointCoordinates origin = aOrigin + offset;
                if (branch_settings.label_absolute_x.has_value())
                    origin.x(*branch_settings.label_absolute_x);
                acmacs::TextStyle label_style = branch_settings.style;
                Color label_color = branch_settings.color;

                // if the first leaf node of this node has hz line with map and that map has label the same as label of this node, show this node label in bold
                const auto section_label_matches = [](std::string section_label, const auto& labels_to_match) -> bool {
                    return std::any_of(std::begin(labels_to_match), std::end(labels_to_match), [&section_label](const auto& label) { return label.first == section_label; });
                };
                if (const auto section = mHzSections.find_section(find_first_leaf(aNode).seq_id); section && (*section)->show_map && section_label_matches((*section)->label, labels)) {
                    label_style.weight = acmacs::FontWeight{acmacs::FontWeight::Bold};
                    // label_color = BLUE;
                }

                acmacs::Rectangle label_box(origin.x(), origin.y() - longest_label_size.height, origin.x() + longest_label_size.width, origin.y());
                for (const auto& label: labels) {
                    const auto label_width = mSurface.text_size(label.first, Pixels{branch_settings.size}, label_style).width;
                    const acmacs::PointCoordinates label_xy(origin.x() + (longest_label_size.width - label_width) / 2, origin.y());
                    mSurface.text(label_xy, label.first, label_color, Pixels{branch_settings.size}, label_style);
                    if (settings->show_node_for_left_line && label.second) {
                        mSurface.line(acmacs::PointCoordinates::zero2D,
                                      acmacs::PointCoordinates(mHorizontalStep * label.second->data.cumulative_edge_length, mVerticalStep * label.second->draw.line_no),
                                      settings->node_for_left_line_color, Pixels{settings->node_for_left_line_width});
                    }
                    label_box.bottom_right.y(origin.y());
                    origin.y(origin.y() + longest_label_size.height * branch_settings.interline);
                }

                const acmacs::PointCoordinates connection_line_start((aOrigin.x() + aRight) / 2, aOrigin.y());
                acmacs::PointCoordinates connection_line_end(acmacs::number_of_dimensions_t{2});
                if (label_box.top_left.y() < aOrigin.y()) {
                    if (label_box.bottom_right.y() < aOrigin.y())
                        connection_line_end = label_box.bottom_middle();
                    else if (label_box.bottom_right.x() < connection_line_start.x())
                        connection_line_end = label_box.middle_right();
                    else
                        connection_line_end = label_box.middle_left();
                }
                else {
                    connection_line_end = label_box.top_middle();
                }

                if (distance(connection_line_start, connection_line_end) > 10)
                    mSurface.line(connection_line_start, connection_line_end, branch_settings.label_connection_line_color, mLineWidth /*branch_settings.label_connection_line_width*/);

                if (mInitializeSettings)
                    settings->per_branch->by_aa_label.append()->set_label_disabled_offset(labels.label(), first_leaf.seq_id, settings->per_branch->label_offset);

                aa_transitions_.push_back({first_leaf.seq_id, labels.label(), origin, aNode.data.number_strains});
                // std::cout << "AA transitions: " << labels.label() << " --> " << aNode.data.number_strains << " strains  label pos: " << origin << " first-leaf-seq-id:" << first_leaf.seq_id << '\n';
            }
        }
    }

} // TreeDraw::draw_aa_transition

// ----------------------------------------------------------------------

void TreeDraw::draw_mark_with_label(const Node& aNode, const acmacs::PointCoordinates& aTextOrigin)
{
    if (aNode.draw.mark_with_label.has_value()) {
        // std::cerr << "DEBUG: draw mark_with_label " << aNode.seq_id << ' ' << *aNode.draw.mark_with_label << '\n';
        const auto settings = mSettings.find_mod(*aNode.draw.mark_with_label);
        if (!last_marked_with_label_.has_value() || std::get<std::string>(*last_marked_with_label_) != *settings->label || (aNode.draw.line_no - std::get<size_t>(*last_marked_with_label_)) > settings->collapse) {
            fmt::print("INFO: draw_mark_with_label {} line:{} label:{} collapse:{}\n", aNode.seq_id, aNode.draw.line_no, *settings->label, *settings->collapse);
            const acmacs::Offset label_offset = settings->label_offset.get_or(acmacs::Offset{0.0, 0.0});
            acmacs::PointCoordinates label_origin = aTextOrigin + label_offset;
            if (settings->label_absolute_x.is_set_or_has_default())
                label_origin.x(settings->label_absolute_x);
            mSurface.text(label_origin, *settings->label, Color{settings->label_color}, Pixels{settings->label_size}, settings->label_style);
            const auto vlsize = mSurface.text_size(*settings->label, Pixels{settings->label_size}, acmacs::TextStyle{});
            const auto line_origin = label_origin + acmacs::Offset{vlsize.width / 2, label_offset.y() > 0 ? -vlsize.height : 0};
            mSurface.line(line_origin, aTextOrigin, Color{settings->line_color}, Pixels{settings->line_width});
            last_marked_with_label_ = std::tuple(aNode.draw.line_no, *settings->label);
        }
    }

} // TreeDraw::draw_mark_with_label

// ----------------------------------------------------------------------

const Legend* TreeDraw::coloring_legend() const
{
    if (!mColoringLegend && mColoring)
        mColoringLegend = std::unique_ptr<Legend>(mColoring->legend());
    return mColoringLegend.get();

} // TreeDraw::coloring_legend

// ----------------------------------------------------------------------

void TreeDraw::draw_legend()
{
    const Legend* legend = coloring_legend();
    if (legend) {
        acmacs::surface::Surface& legend_surface = mSurface.subsurface(static_cast<acmacs::Offset>(mSettings.legend->offset), Scaled{mSettings.legend->width}, legend->size(), false);
        legend->draw(legend_surface, mSettings.legend);
          // legend_surface->border("red", 1);
    }

} // TreeDraw::draw_legend

// ----------------------------------------------------------------------

void TreeDraw::report_aa_transitions()
{
    if (!aa_transitions_.empty()) {
        std::cerr << "DEBUG: TreeDraw::report_aa_transitions\n";
        std::sort(aa_transitions_.begin(), aa_transitions_.end()); // , [](const auto& e1, const auto& e2) -> bool { return e1.origin.y() < e2.origin.y(); });
        const auto transition_width = static_cast<int>(
            std::max_element(aa_transitions_.begin(), aa_transitions_.end(), [](const auto& e1, const auto& e2) -> bool { return e1.transition.size() < e2.transition.size(); })->transition.size());
        const auto node_width = static_cast<int>(
            std::max_element(aa_transitions_.begin(), aa_transitions_.end(), [](const auto& e1, const auto& e2) -> bool { return e1.first_leaf.size() < e2.first_leaf.size(); })->first_leaf.size());
        std::cout << "AA transitions:\n";
        for (const auto& tr : aa_transitions_) {
            std::cout << "  {\"label\": " << std::setw(transition_width + 4) << std::left << string::concat('"', tr.transition, "\",")
                      << "\"label_offset\": [-40, 20], "
                      << "\"first_leaf_seq_id\": " << std::setw(node_width + 4) << std::left << string::concat('"', tr.first_leaf, "\",")
                      << "\"?strains\":" << std::setw(4) << std::right << tr.number_strains
                      << ", \"?origin\": [" << std::setprecision(0) << tr.origin.x() << ',' << tr.origin.y() << "]},\n";
        }
    }

} // TreeDraw::report_aa_transitions

// ----------------------------------------------------------------------

void TreeDrawSettings::remove_for_signature_page_settings()
{
    ladderize.remove();
    ladderize_help.remove();
    // mods.remove();
    // mods_help.remove();
    force_line_width.remove();
    line_width.remove();
    root_edge.remove();
    line_color.remove();
    label_style.remove();
    name_offset.remove();
    color_nodes.remove();
      // legend position changes in sig page // legend.remove();
    aa_transition->remove_for_signature_page_settings();

} // TreeDrawSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

void AATransitionIndividualSettings::remove_for_tree_settings()
{
    show.remove();
    size.remove();
    color.remove();
    style.remove();
    interline.remove();
    label_connection_line_width.remove();
    label_connection_line_color.remove();

} // AATransitionIndividualSettings::remove_for_tree_settings

// ----------------------------------------------------------------------

void AATransitionIndividualSettings::remove_for_signature_page_settings()
{
    show.remove();
    size.remove();
    color.remove();
    style.remove();
    interline.remove();
    label_connection_line_width.remove();
    label_connection_line_color.remove();

} // AATransitionIndividualSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

void AATransitionPerBranchDrawSettings::remove_for_tree_settings()
{
    show.remove();
    size.remove();
    color.remove();
    style.remove();
    interline.remove();
    label_connection_line_width.remove();
    label_connection_line_color.remove();
    for (size_t index = 0; index < by_aa_label.size(); ++index)
        by_aa_label[index]->remove_for_tree_settings();

} // AATransitionPerBranchDrawSettings::remove_for_tree_settings

// ----------------------------------------------------------------------

void AATransitionPerBranchDrawSettings::remove_for_signature_page_settings()
{
    show.remove();
    size.remove();
    color.remove();
    style.remove();
    interline.remove();
    label_offset.remove();
    scatter_label_offset.remove();
    scatter_label_offset_help.remove();
    label_connection_line_width.remove();
    label_connection_line_color.remove();
    for (size_t index = 0; index < by_aa_label.size(); ++index)
        by_aa_label[index]->remove_for_signature_page_settings();

} // AATransitionPerBranchDrawSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

void AATransitionDrawSettings::remove_for_signature_page_settings()
{
    show.remove();
    number_strains_threshold.remove();
    number_strains_threshold_help.remove();
    show_empty_left.remove();
    show_node_for_left_line.remove();
    node_for_left_line_color.remove();
    node_for_left_line_width.remove();
    per_branch->remove_for_signature_page_settings();

} // AATransitionDrawSettings::remove_for_signature_page_settings

// ----------------------------------------------------------------------

void HzSections::convert_aa_transitions(Tree& tree) // to name based hz sections
{
    std::cerr << "DEBUG: HzSections::convert_aa_transitions\n";
    std::vector<size_t> to_remove;
    std::vector<std::pair<const Node*, std::string>> to_add;
    sections.for_each([&tree, &to_remove, &to_add](auto& section, size_t section_index) {
        if (!section.aa_transition.empty()) {
            // std::cerr << "DEBUG:   section " << section_index << ' ' << section.name << ' ' << section.aa_transition << '\n';
            tree::iterate_pre(tree, [&to_add, &section](const Node& node) {
                if (node.data.aa_transitions.contains(section.aa_transition) && node.data.number_strains > 200)
                    to_add.emplace_back(&node, *section.aa_transition);
            });
            to_remove.push_back(section_index);
        }
    });
    for (auto sec_p = to_remove.rbegin(); sec_p != to_remove.rend(); ++sec_p)
        sections.erase(*sec_p);
    for (const auto& node_to_add : to_add)
        add(tree, find_first_leaf(*node_to_add.first), find_last_leaf(*node_to_add.first), true, node_to_add.second, 0);

    // sections.for_each([](auto& section, size_t) {
    //     std::cerr << rjson::to_string(section.get(), rjson::show_empty_values::no) << '\n';
    // });

} // HzSections::convert_aa_transitions

// ----------------------------------------------------------------------

void HzSections::sort(const Tree& aTree)
{
    // std::cerr << "DEBUG: HzSections::sort\n";
    // sections.for_each([](auto& section, size_t section_index) {
    //     std::cerr << "    section " << section_index << ' ' << section.name << ' ' << section.aa_transition << '\n';
    // });

    node_refs.resize(sections.size());

    auto set_first_node = [this](const Node& node) {
        if (auto sec_no = sections.find_index_if([&node](const auto& s) -> bool { return s.name == node.seq_id; }); sec_no)
            node_refs[*sec_no].first = &node;
    };
    tree::iterate_leaf(aTree, set_first_node);

    // remove not found sections before sorting (e.g. having no name or not found name)
    std::vector<size_t> to_remove;
    for (size_t sec_no = 0; sec_no < node_refs.size(); ++sec_no) {
        if (node_refs[sec_no].first == nullptr) {
            if (sections[sec_no]->name != "")
                std::cerr << "WARNING: HZ section removed (leaf node not found): " << sections[sec_no]->name << '\n';
            to_remove.push_back(sec_no);
        }
        else if (!node_refs[sec_no].first->draw.shown) {
            std::cerr << "WARNING: HZ section removed (leaf node not shown): " << sections[sec_no]->name << '\n';
            to_remove.push_back(sec_no);
        }
    }
    for (auto sec_p = to_remove.rbegin(); sec_p != to_remove.rend(); ++sec_p) {
        sections.erase(*sec_p);
        node_refs.erase(node_refs.begin() + static_cast<decltype(node_refs)::difference_type>(*sec_p));
    }
    assert(sections.size() == node_refs.size());

    acmacs::fill_with_indexes(section_order, node_refs.size());
    std::sort(section_order.begin(), section_order.end(), [this](const auto& a, const auto& b) -> bool { return node_refs[a].first->draw.line_no < node_refs[b].first->draw.line_no; });

    // if the first section does not start with the topmost node, prepend section list with the new section
    const Node* first_leaf = aTree.find_leaf_by_line_no(TreeDraw::sFirstLineNo);
    if (sections.empty() || node_refs[section_order.front()].first != first_leaf) {
        auto new_sec = sections.append();
        node_refs.emplace_back(first_leaf);
        new_sec->name = first_leaf->seq_id;
        new_sec->show = true;
        new_sec->show_line = false;
        new_sec->show_map = false;
        section_order.insert(section_order.begin(), node_refs.size() - 1);
    }

    for (size_t order_index = 1; order_index < section_order.size(); ++order_index) {
        node_refs[section_order[order_index - 1]].last = aTree.find_leaf_by_line_no(node_refs[section_order[order_index]].first->draw.line_no - 1);
    }
    if (!node_refs.empty())
        node_refs[section_order.back()].last = &find_last_leaf(aTree);

    size_t section_no = 0;
    for (auto section_index : section_order) {
        const auto& section = sections[section_index];
        if (section->show && section->show_map) {
            node_refs[section_index].index.assign(1, 'A' + static_cast<char>(section_no));
            ++section_no;
        }
    }

} // HzSections::sort

// ----------------------------------------------------------------------

void HzSections::report(std::ostream& out) const
{
    out << "INFO: hz sections " << section_order.size() << '\n';
    for (auto section_index: section_order) {
        out << "  " << std::setw(4) << std::right << node_refs[section_index].first->draw.line_no << ' ' << node_refs[section_index].first->seq_id;
        // const auto& section = *sections[section_index];
        // if (!section.triggering_clades.empty()) {
        //     out << "   clades: " << section.triggering_clades;
        // }
        // if (!section.triggering_aa_pos.empty()) {
        //     out << "   aa-at: " << section.triggering_aa_pos;
        // }
        out << '\n';
    }

    for (auto section_index: section_order)
        out << rjson::to_string(sections[section_index]->get(), rjson::show_empty_values::no) << ",\n";

} // HzSections::report

// ----------------------------------------------------------------------

void HzSections::report_html(std::string filename) const
{
    std::ofstream out(filename.c_str());
    out << "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\" />\n<title>HZ sections report</title>\n</head>\n<body>\n"
        << "\n"
        << "</body>\n</html>\n";

} // HzSections::report_html

// ----------------------------------------------------------------------

// void HzSections::auto_detect(Tree& aTree, const Clades* aClades)
// {
//     if (sections.empty()) {
//         auto sec = sections.emplace_back();
//         sec.name = find_first_leaf(aTree).seq_id;
//         sec.show_line = false;

//         if (aClades) {
//             for (const auto& clade: *aClades) {
//                 if (clade.second.shown()) {
//                     std::vector<std::pair<std::string, std::string>> seq_ids;
//                     clade.second.seq_ids(seq_ids);
//                     for (const auto& s: seq_ids) {
//                         auto sec2 = sections.emplace_back();
//                         sec2.name = s.first;
//                         sec2.show_line = false; // false -> do not show line
//                         const Node* last_node_of_clade = aTree.find_leaf_by_seqid(s.second);
//                         const Node* next_node = aTree.find_leaf_by_line_no(last_node_of_clade->draw.line_no + 1);
//                         if (next_node) {
//                             auto sec3 = sections.emplace_back();
//                             sec3.name = next_node->seq_id;
//                             sec3.show_line = false;
//                         }
//                     }
//                 }
//             }
//         }

//         const size_t number_of_sections_to_detect = sections.size() + 5;

//         const auto nodes = aTree.leaf_nodes_sorted_by_distance_from_previous();

//           // size_t i = 0;
//           // std::cout << "HZ lines detection (distance_from_previous):" << '\n';
//           // for (const auto node: nodes) {
//           //     std::cout << std::fixed << std::setprecision(8) << std::setw(14) << node->data.distance_from_previous << ' ' << node->seq_id
//           //               << (node->draw.shown ? "" : " *HIDDEN*")
//           //               << '\n';
//           //     if (++i > 10)
//           //         break;
//           // }

//         for (size_t node_no = 0; sections.size() <= number_of_sections_to_detect && node_no < nodes.size(); ++node_no) {
//             if (nodes[node_no]->draw.shown) {
//                 auto sec4 = sections.emplace_back();
//                 sec4.name = nodes[node_no]->seq_id;
//                 sec4.show_line = true;
//             }
//         }

//           // remove sections having the same seq_id
//         std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.name < b.name; });
//         sections.erase(std::unique(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.name == b.name; }), sections.end());

//           // detect first node for each section
//         for (auto& section: sections)
//             section.first = aTree.find_leaf_by_seqid(section.name);
//           // sort sections by line_no of the beginning
//         std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.first->draw.line_no < b.first->draw.line_no; });
//     }

// } // HzSections::auto_detect

// ----------------------------------------------------------------------

void HzSections::detect_hz_lines_for_clades(Tree& aTree, const Clades* aClades, bool aForce)
{
    if (aForce)
        sections.clear();
    if (sections.empty()) {
        add(find_first_leaf(aTree).seq_id, false, "first-leaf", 0, true);

        if (aClades) {
            for (const auto& clade : *aClades) {
                if (clade.second.shown()) {
                    for (const auto& sect : clade.second.sections) {
                        if (clade.first != "GLY" && clade.first != "NO-GLY") {
                            // std::cerr << "DEBUG: clade: " << clade.first << ' ' << s << DEBUG_LINE_FUNC << '\n';
                            add(aTree, *sect.first, *sect.last, false, clade.first, 0);
                        }
                    }
                }
            }
        }
    }

} // HzSections::detect_hz_lines_for_clades

// ----------------------------------------------------------------------

acmacs::settings::v1::array_element<HzSection> HzSections::add(std::string seq_id, bool show_line, std::string clade, size_t aa_pos, bool first_in_clade)
{
    // std::cerr << "DEBUG: hz sections " << sections.size() << DEBUG_LINE_FUNC << '\n';
    const std::string clade_tag = string::concat(clade, ':', first_in_clade ? "first" : "last");
    // if (auto found = sections.find_if([&seq_id](const auto& sect) { return sect.name == seq_id; }); !found) {
    if (auto found = find_section(seq_id); !found) {
        // std::cerr << "DEBUG: add hz section: " << seq_id << '\n';
        auto sec = sections.append();
        sec->name = seq_id;
        sec->show_line = show_line;
        if (!clade.empty()) {
            sec->triggering_clades.append(clade_tag);
            if (first_in_clade)
                sec->label = clade;
        }
        if (aa_pos > 0)
            sec->triggering_aa_pos.append(aa_pos);
        return sec;
    }
    else {
        if (!clade.empty())
            (*found)->triggering_clades.append(clade_tag);
        if (aa_pos > 0)
            (*found)->triggering_aa_pos.append(aa_pos);
        // std::cerr << "DEBUG: hz section found: " << seq_id << ' ' << (*found)->triggering_clades << ' ' << (*found)->triggering_aa_pos << '\n';
        return *found;
    }

} // HzSections::add

// ----------------------------------------------------------------------

void HzSections::add(const Tree& tree, const Node& first, const Node& last, bool show_line, std::string clade, size_t aa_pos)
{
    add(first.seq_id, show_line, clade, aa_pos, true);
    if (const Node* next_node = tree.find_leaf_by_line_no(last.draw.line_no + 1); next_node)
        add(next_node->seq_id, show_line, clade, aa_pos, false);

} // HzSections::add

// ----------------------------------------------------------------------

acmacs::settings::v1::array_element<HzSection> HzSections::add(std::string aa_transition, bool show_line)
{
    auto sec = sections.append();
    sec->aa_transition = aa_transition;
    sec->show_line = show_line;
    sec->triggering_clades.append(aa_transition);
    return sec;

} // HzSections::add

// ----------------------------------------------------------------------

AATransitionIndividualSettingsForLabel::AATransitionIndividualSettingsForLabel(const AATransitionPerBranchDrawSettings& src)
    : show(src.show), size(src.size), color(src.color),
      style(src.style), interline(src.interline), label_offset(src.label_offset),
      label_connection_line_width(src.label_connection_line_width), label_connection_line_color(src.label_connection_line_color)
{

} // AATransitionIndividualSettingsForLabel::AATransitionIndividualSettingsForLabel

// ----------------------------------------------------------------------

void AATransitionIndividualSettingsForLabel::update(const AATransitionIndividualSettings& src)
{
    if (src.show.is_set_or_has_default())
        show = src.show;
    if (src.size.is_set_or_has_default())
        size = src.size;
    if (src.color.is_set_or_has_default())
        color = src.color;
    if (src.style.is_set_or_has_default())
        style = src.style;
    if (src.interline.is_set_or_has_default())
        interline = src.interline;
    if (src.label_offset.is_set_or_has_default())
        label_offset = src.label_offset;
    if (src.label_absolute_x.is_set_or_has_default())
        label_absolute_x = src.label_absolute_x;
    if (src.label_connection_line_width.is_set_or_has_default())
        label_connection_line_width = src.label_connection_line_width;
    if (src.label_connection_line_color.is_set_or_has_default())
        label_connection_line_color = src.label_connection_line_color;

} // AATransitionIndividualSettingsForLabel::update

// ----------------------------------------------------------------------

AATransitionIndividualSettingsForLabel AATransitionPerBranchDrawSettings::settings_for_label(const AA_TransitionLabels& aLabels, std::string aFirstLeafSeqid) const
{
    AATransitionIndividualSettingsForLabel result(*this);
    auto match_entry = [label = aLabels.label(), aFirstLeafSeqid](const AATransitionIndividualSettings& entry) {
                           return entry.label == label && (entry.first_leaf_seq_id == "" || entry.first_leaf_seq_id == aFirstLeafSeqid);
                       };
    if (auto found = by_aa_label.find_if(match_entry); found) {
        result.update(*found);
    }

    if (scatter_label_offset > 0.0) {
        std::random_device rand;
        constexpr const auto rand_size = static_cast<double>(rand.max() - rand.min());
        const acmacs::PointCoordinates old_label_offset = result.label_offset;
        result.label_offset.x(static_cast<double>(rand()) * scatter_label_offset * 2 / rand_size - scatter_label_offset + old_label_offset.x());
        result.label_offset.y(static_cast<double>(rand()) * scatter_label_offset * 2 / rand_size - scatter_label_offset + old_label_offset.y());
    }
    return result;

} // AATransitionPerBranchDrawSettings::settings_for_label

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
