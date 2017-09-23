#include <algorithm>
#include <iomanip>
#include <random>

#include "acmacs-base/range.hh"
#include "tree-draw.hh"
#include "tree.hh"
#include "tree-iterate.hh"
#include "acmacs-draw/surface.hh"
#include "coloring.hh"

// ----------------------------------------------------------------------

TreeDraw::TreeDraw(Surface& aSurface, Tree& aTree, TreeDrawSettings& aSettings, HzSections& aHzSections)
    : mSurface(aSurface), mTree(aTree), mSettings(aSettings), mHzSections(aHzSections)
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
    const std::string color_nodes = mSettings.color_nodes.get_value_ref();
    if (color_nodes == "black")
        mColoring = std::unique_ptr<Coloring>(new ColoringBlack());
    else if (color_nodes == "continent")
        mColoring = std::unique_ptr<Coloring>(new ColoringByContinent());
    else {
        try {
            mColoring = std::unique_ptr<Coloring>(new ColoringByPos(std::stoul(color_nodes)));
        }
        catch (std::exception&) {
            throw std::runtime_error("Unrecognized TreeDrawSettings.color_nodes: " + color_nodes);
        }
    }

} // TreeDraw::make_coloring

// ----------------------------------------------------------------------

void TreeDraw::init_settings(const Clades* aClades)
{
    mInitializeSettings = true;

      // set_line_no();    // line_no is necessary to sort detected hz sections, but set_line_no is called by SignaturePageDraw::init_settings(), so this call is redundant
    mHzSections.detect_hz_lines_for_clades(mTree, aClades, true);
    // mHzSections.auto_detect(mTree, aClades); // makes no sense

} // TreeDraw::init_settings

// ----------------------------------------------------------------------

void TreeDraw::detect_hz_lines_for_clades(const Clades* aClades, bool aForce)
{
    mHzSections.detect_hz_lines_for_clades(mTree, aClades, aForce);

} // TreeDraw::detect_hz_lines_for_clades

// ----------------------------------------------------------------------

void TreeDraw::prepare(const LocDb& aLocDb)
{
    mTree.set_continents(aLocDb);
    ladderize();
    mTree.make_aa_transitions();

    size_t number_of_hz_sections = prepare_hz_sections();
    if (number_of_hz_sections == 0)
        number_of_hz_sections = 1;
    const auto& canvas_size = mSurface.viewport().size;
    mHorizontalStep = canvas_size.width / mTree.width();
    mVerticalStep = (canvas_size.height - (number_of_hz_sections - 1) * mHzSections.vertical_gap) / static_cast<double>(mTree.height() + 2); // +2 to add space at the top and bottom
    set_vertical_pos();

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
    // std::cout << "TREE-mod: unhide" << std::endl;
    // unhide();                   // reset previous mods
    for (const auto& mod: mSettings.mods) {
        const auto mod_mod = static_cast<std::string>(mod.mod);
        if (mod_mod == "root") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.s1 << std::endl;
            mTree.re_root(mod.s1);
        }
        else if (mod_mod == "hide-isolated-before") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.s1 << std::endl;
            hide_isolated_before(mod.s1);
        }
        else if (mod_mod == "hide-if-cumulative-edge-length-bigger-than") {
            std::cout << "TREE-mod: " << mod_mod << " " << mod.d1 << std::endl;
            hide_if_cumulative_edge_length_bigger_than(mod.d1);
        }
        else if (mod_mod == "before2015-58P-or-146I-or-559I") {
            std::cout << "TREE-mod: " << mod_mod << std::endl;
            hide_before2015_58P_or_146I_or_559I();
        }
        else if (mod_mod == "hide-between") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\"" << std::endl;
            hide_between(mod.s1, mod.s2);
        }
        else if (mod_mod == "hide-one") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\"" << std::endl;
            hide_one(mod.s1);
        }
        else if (mod_mod == "mark-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\" " << mod.d1 << std::endl;
            mark_with_line(mod.s1, static_cast<std::string>(mod.s2), Pixels{mod.d1});
        }
        else if (mod_mod == "mark-clade-with-line") {
            std::cout << "TREE-mod: " << mod_mod << " \"" << mod.clade << "\" \"" << mod.color << "\" " << mod.line_width << std::endl;
            mark_clade_with_line(mod.clade, static_cast<std::string>(mod.color), Pixels{mod.line_width});
        }
        else if (mod_mod.empty() || mod_mod[0] == '?') {
              // commented out mod
        }
        else
            throw std::runtime_error("Unrecognized tree mod: " + mod_mod);
    }
    return !mSettings.mods.empty();

} // TreeDraw::apply_mods

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    std::cout << "Tree surface: " << mSurface.viewport() << std::endl;
    const double line_width = mSettings.line_width;
    mLineWidth = mSettings.force_line_width ? line_width : std::min(line_width, mVerticalStep * 0.5);
      // std::cout << "Tree line width: " << mLineWidth.valeu() << "  Settings: " << mSettings.line_width << "  vertical_step/2: " << mVerticalStep * 0.5 << std::endl;
    fit_labels_into_viewport();

    double vertical_gap = 0;
    draw_node(mTree, 0 /*mLineWidth / 2*/, vertical_gap, mSettings.root_edge);
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
        if (aNode.data.date() < "2015-01-01") {
            const std::string aa = aNode.data.amino_acids();
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
        std::cout << "leaf nodes hidden: " << hidden << std::endl;
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
        throw std::runtime_error("tree hide_between: no nodes hidden");
    std::cout << "leaf nodes hidden: " << hidden << std::endl;

} // TreeDraw::hide_one

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
        std::cout << "leaf nodes marked: " << marked << std::endl;

} // TreeDraw::mark_with_line

// ----------------------------------------------------------------------

void TreeDraw::mark_clade_with_line(std::string aClade, Color aColor, Pixels aLineWidth)
{
    size_t marked = 0;
    auto mark_leaf = [aClade,&aColor,&aLineWidth,&marked](Node& aNode) {
        if (aNode.data.has_clade(aClade)) {
            aNode.draw.mark_with_line = aColor;
            aNode.draw.mark_with_line_width = aLineWidth;
            ++marked;
        }
    };
    tree::iterate_leaf(mTree, mark_leaf);
    if (marked == 0)
        std::cerr << "WARNING: not found to mark with line for clade: " << aClade << '\n';
    else
        std::cout << "Clade " << aClade << " leaf nodes marked: " << marked << std::endl;

} // TreeDraw::mark_clade_with_line

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
//     std::cout << "TREE: hide_leaves " << aForce << std::endl;
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
    std::cout << "TREE-lines: " << current_line << std::endl;

} // TreeDraw::set_line_no

// ----------------------------------------------------------------------

void TreeDraw::set_vertical_pos()
{
    double vertical_pos = mVerticalStep;
    bool topmost_node = true;
    auto set_leaf_vertical_pos = [&](Node& aNode) {
        if (aNode.draw.shown) {
            if (aNode.draw.hz_section_index != NodeDrawData::HzSectionNoIndex && mHzSections.sections[aNode.draw.hz_section_index].show) {
                std::cout << "TREE-hz-section: " << aNode.draw.hz_section_index << " " << aNode.seq_id << std::endl;
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
    mHzSections.sort(mTree);

    size_t number_of_hz_sections = 0;
      // const Node& first_leaf = find_first_leaf(mTree);
    for (auto section_index: mHzSections.section_order) {
        const auto section = mHzSections.sections[section_index];
        if (section.show) {
            Node* section_start = mTree.find_leaf_by_seqid(section.name);
            if (section_start) {
                if (section_start->draw.shown) {
                    section_start->draw.hz_section_index = section_index;
                    ++number_of_hz_sections;
                }
                else {
                    std::cerr << "WARNING: HzSection ignored because its node is hidden: " << section.name << std::endl;
                }
            }
            else {
                std::cerr << "WARNING: HzSection seq_id not found: " << section.name << std::endl;
            }
        }
    }
    // if (number_of_hz_sections == 0)
    //     number_of_hz_sections = 1;
    std::cerr << "HZ sections: " << number_of_hz_sections << std::endl;
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

    for (double label_offset = max_label_offset(); label_offset > canvas_width; label_offset = max_label_offset()) {
        const double scale = std::min(canvas_width / label_offset, 0.99); // to avoid too much looping
          // std::cerr << "Canvas:" << canvas_width << " label_right:" << label_offset << " scale: " << scale << std::endl;
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
    tree::iterate_leaf(mTree, label_offset);
    return max_label_right;

} // TreeDraw::max_label_offset

// ----------------------------------------------------------------------

void TreeDraw::draw_node(const Node& aNode, double aOriginX, double& aVerticalGap, double aEdgeLength)
{
    if (aNode.draw.shown) {
        Size size{(aEdgeLength < 0.0 ? aNode.edge_length : aEdgeLength) * mHorizontalStep, 0};
        const double right = aOriginX + size.width;

        if (aNode.is_leaf()) {
            const std::string text = aNode.display_name();
            const auto tsize = mSurface.text_size(text, mFontSize, mSettings.label_style);
            const Location text_origin{right + mNameOffset, aNode.draw.vertical_pos + tsize.height / 2};
            mSurface.text(text_origin, text, mColoring->color(aNode), mFontSize, mSettings.label_style);
            if (text_origin.x < 0 || text_origin.y < 0) {
                std::cerr << "WARNING: bad origin for a node label: " << text_origin << ' ' << text << " mNameOffset:" << mNameOffset << " aOriginX:" << aOriginX << '\n';
            }
            if (!aNode.draw.vaccine_label.empty()) {
                const auto& settings = mSettings.vaccine(aNode.draw.vaccine_label);
                Size label_offset{-20, 20}; // TODO: settings
                const Location label_origin = text_origin + label_offset;
                mSurface.text(label_origin, aNode.draw.vaccine_label, settings.label_color, Pixels{settings.label_size}, settings.label_style);
                const auto vlsize = mSurface.text_size(aNode.draw.vaccine_label, Pixels{settings.label_size}, settings.label_style);
                const auto line_origin = label_origin + Size(vlsize.width / 2, label_offset.height > 0 ? - vlsize.height : 0);
                mSurface.line(line_origin, text_origin, settings.line_color, Pixels{settings.line_width});
            }
            if (!aNode.draw.mark_with_line.empty()) {
                // mSurface.line({text_origin.x + tsize.width, text_origin.y}, {mSurface.viewport().size.width, text_origin.y}, aNode.draw.mark_with_line, aNode.draw.mark_with_line_width);
                mSurface.line({mSurface.viewport().size.width - 10, text_origin.y}, {mSurface.viewport().size.width, text_origin.y}, aNode.draw.mark_with_line, aNode.draw.mark_with_line_width);
            }
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
            mSurface.line({right, top}, {right, bottom}, mSettings.line_color, mLineWidth, Surface::LineCap::Square);
        }
        mSurface.line({aOriginX, aNode.draw.vertical_pos}, {right, aNode.draw.vertical_pos}, mSettings.line_color, mLineWidth);
        draw_aa_transition(aNode, {aOriginX, aNode.draw.vertical_pos}, right);
    }

} // TreeDraw::draw_node

// ----------------------------------------------------------------------

void TreeDraw::draw_aa_transition(const Node& aNode, const Location& aOrigin, double aRight)
{
    auto& settings = mSettings.aa_transition;
    const auto& first_leaf = find_first_leaf(aNode);
    if (settings.show && !aNode.data.aa_transitions.empty() && aNode.data.number_strains >= settings.number_strains_threshold) {
        if (auto labels = aNode.data.aa_transitions.make_labels(settings.show_empty_left); !labels.empty()) {
            const auto& branch_settings = settings.per_branch.settings_for_label(labels, first_leaf.seq_id);
            if (branch_settings.show) {
                const auto longest_label = std::max_element(labels.begin(), labels.end(), [](const auto& a, const auto& b) { return a.first.size() < b.first.size(); });
                const auto longest_label_size = mSurface.text_size(longest_label->first, Pixels{branch_settings.size}, branch_settings.style);
                const auto node_line_width = aRight - aOrigin.x;
                Size offset(node_line_width > longest_label_size.width ? (node_line_width - longest_label_size.width) / 2 : (node_line_width - longest_label_size.width),
                            longest_label_size.height * branch_settings.interline);
                offset += branch_settings.label_offset;
                Location origin = aOrigin + offset;
                for (const auto& label: labels) {
                    const auto label_width = mSurface.text_size(label.first, Pixels{branch_settings.size}, branch_settings.style).width;
                    const Location label_xy(origin.x + (longest_label_size.width - label_width) / 2, origin.y);
                    mSurface.text(label_xy, label.first, branch_settings.color, Pixels{branch_settings.size}, branch_settings.style);
                    if (settings.show_node_for_left_line && label.second) {
                        mSurface.line(Location{},
                                      Location(mHorizontalStep * label.second->data.cumulative_edge_length, mVerticalStep * label.second->draw.line_no),
                                      settings.node_for_left_line_color, Pixels{settings.node_for_left_line_width});
                    }
                    origin.y += longest_label_size.height * branch_settings.interline;
                }

                const Location connection_line_start{(aOrigin.x + aRight) / 2, aOrigin.y};
                Location connection_line_end;
                const Size label_offset = branch_settings.label_offset;
                if (label_offset.height > 5)
                    connection_line_end.set(aOrigin.x + longest_label_size.width / 2 + offset.width, aOrigin.y - longest_label_size.height + offset.height);
                else if (label_offset.height < -5)
                    connection_line_end.set(aOrigin.x + longest_label_size.width / 2 + offset.width, aOrigin.y + offset.height);
                else if (label_offset.width > 0)
                    connection_line_end.set(aOrigin.x + offset.width, aOrigin.y + offset.height - longest_label_size.height / 2);
                else
                    connection_line_end.set(aOrigin.x + longest_label_size.width + offset.width, aOrigin.y + offset.height - longest_label_size.height / 2);
                if (distance(connection_line_start, connection_line_end) > 10) {
                    mSurface.line(connection_line_start, connection_line_end, branch_settings.label_connection_line_color, mLineWidth /*branch_settings.label_connection_line_width*/);
                }

                if (mInitializeSettings) {
                    settings.per_branch.by_aa_label.emplace_back().set_label_disabled_offset(labels.label(), first_leaf.seq_id, settings.per_branch.label_offset);
                }

                std::cout << "AA transitions: " << labels.label() << " --> " << aNode.data.number_strains << " strains  label pos: " << origin << " first-leaf-seq-id:" << first_leaf.seq_id << '\n';
            }
        }
    }

} // TreeDraw::draw_aa_transition

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
        Surface& legend_surface = mSurface.subsurface(static_cast<Size>(mSettings.legend.offset), Scaled{mSettings.legend.width}, legend->size(), false);
        legend->draw(legend_surface, mSettings.legend);
          // legend_surface->border("red", 1);
    }

} // TreeDraw::draw_legend

// ----------------------------------------------------------------------

void HzSections::sort(const Tree& aTree)
{
    node_refs = decltype(node_refs){sections.size()};

    auto set_first_node = [this](const Node& node) {
        auto sec = std::find_if(sections.begin(), sections.end(), [&node](const auto& s) -> bool { return static_cast<std::string>(s.name) == node.seq_id; });
        if (sec != sections.end()) {
            node_refs[static_cast<size_t>(sec - sections.begin())].first = &node;
        }
    };
    tree::iterate_leaf(aTree, set_first_node);

      // remove not found sections before sorting (e.g. having no name or not found name)
    std::vector<size_t> to_remove;
    for (size_t sec_no = 0; sec_no < node_refs.size(); ++sec_no) {
        if (node_refs[sec_no].first == nullptr) {
            if (!sections[sec_no].name.empty())
                std::cerr << "WARNING: HZ section removed (leaf node not found): " << sections[sec_no].name << std::endl;
            to_remove.push_back(sec_no);
        }
        else if (!node_refs[sec_no].first->draw.shown) {
            std::cerr << "WARNING: HZ section removed (leaf node not shown): " << sections[sec_no].name << std::endl;
            to_remove.push_back(sec_no);
        }
    }
    for (auto sec_p = to_remove.rbegin(); sec_p != to_remove.rend(); ++sec_p) {
        sections.erase(*sec_p);
        node_refs.erase(node_refs.begin() + static_cast<decltype(node_refs)::difference_type>(*sec_p));
    }
    assert(sections.size() == node_refs.size());

    fill_with_indexes(section_order, node_refs.size());
    std::sort(section_order.begin(), section_order.end(), [this](const auto& a, const auto& b) -> bool { return node_refs[a].first->draw.line_no < node_refs[b].first->draw.line_no; });

      // if the first section does not start with the topmost node, prepend section list with the new section
    const Node* first_leaf = aTree.find_leaf_by_line_no(TreeDraw::sFirstLineNo);
    if (sections.empty() || node_refs[section_order.front()].first != first_leaf) {
        auto new_sec = sections.emplace_back();
        node_refs.emplace_back(first_leaf);
        new_sec.name = first_leaf->seq_id;
        new_sec.show = true;
        new_sec.show_line = false;
        new_sec.show_map = false;
        section_order.insert(section_order.begin(), node_refs.size() - 1);
    }

    for (size_t order_index = 1; order_index < section_order.size(); ++order_index) {
        node_refs[section_order[order_index - 1]].last = aTree.find_leaf_by_line_no(node_refs[section_order[order_index]].first->draw.line_no - 1);
    }
    if (!node_refs.empty())
        node_refs[section_order.back()].last = &find_last_leaf(aTree);

    size_t section_no = 0;
    for (auto section_index: section_order) {
        const auto section = sections[section_index];
        if (section.show and section.show_map) {
            node_refs[section_index].index.assign(1, 'A' + static_cast<char>(section_no));
            ++section_no;
        }
    }

} // HzSections::sort

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

//         std::vector<const Node*> nodes;
//         aTree.leaf_nodes_sorted_by_distance_from_previous(nodes);

//           // size_t i = 0;
//           // std::cout << "HZ lines detection (distance_from_previous):" << std::endl;
//           // for (const auto node: nodes) {
//           //     std::cout << std::fixed << std::setprecision(8) << std::setw(14) << node->data.distance_from_previous << ' ' << node->seq_id
//           //               << (node->draw.shown ? "" : " *HIDDEN*")
//           //               << std::endl;
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
        auto sec = sections.emplace_back();
        sec.name = find_first_leaf(aTree).seq_id;
        sec.show_line = false;

        if (aClades) {
            for (const auto& clade: *aClades) {
                if (clade.second.shown()) {
                    for (const auto& s: clade.second.seq_ids()) {
                        std::cerr << "DEBUG: clade: " << clade.first << ' ' << s << DEBUG_LINE_FUNC << '\n';
                        auto sec2 = sections.emplace_back();
                        sec2.name = s.first;
                        sec2.show_line = false;
                        const Node* last_node_of_clade = aTree.find_leaf_by_seqid(s.second);
                        if (const Node* next_node = aTree.find_leaf_by_line_no(last_node_of_clade->draw.line_no + 1); next_node) {
                            auto sec3 = sections.emplace_back();
                            sec3.name = next_node->seq_id;
                            sec3.show_line = false;
                        }
                    }
                }
            }
        }
    }

} // HzSections::detect_hz_lines_for_clades

// ----------------------------------------------------------------------

const AATransitionIndividualSettings& AATransitionPerBranchDrawSettings::settings_for_label(const AA_TransitionLabels& aLabels, std::string aFirstLeafSeqid) const
{
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#endif
    static rjson::value settings{rjson::object{}};
    static AATransitionIndividualSettings result{settings};
#pragma GCC diagnostic pop

    settings = data();

    auto match_entry = [label = aLabels.label(), aFirstLeafSeqid](const auto& entry) {
                           return entry.label == label && (entry.first_leaf_seq_id.empty() || entry.first_leaf_seq_id == aFirstLeafSeqid);
                       };
    if (auto found = std::find_if(std::begin(by_aa_label), std::end(by_aa_label), match_entry);
        found != std::end(by_aa_label)) {
        settings.update((*found).data());
    }

    if (scatter_label_offset > 0) {
        std::random_device rand;
        constexpr const auto rand_size = static_cast<double>(rand.max() - rand.min());
        const rjson::array& old_label_offset = settings.get_ref("label_offset", rjson::array{0.0, 0.0});
        settings.set_field("label_offset", rjson::array{static_cast<double>(rand()) * scatter_label_offset * 2 / rand_size - scatter_label_offset + static_cast<double>(old_label_offset[0]),
                        static_cast<double>(rand()) * scatter_label_offset * 2 / rand_size - scatter_label_offset + static_cast<double>(old_label_offset[1])});
    }
    return result;

} // AATransitionPerBranchDrawSettings::settings_for_label

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
