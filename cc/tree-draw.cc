#include <algorithm>
#include <iomanip>

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
    if (mSettings.color_nodes == "black")
        mColoring = std::unique_ptr<Coloring>(new ColoringBlack());
    else if (mSettings.color_nodes == "continent")
        mColoring = std::unique_ptr<Coloring>(new ColoringByContinent());
    else {
        try {
            mColoring = std::unique_ptr<Coloring>(new ColoringByPos(std::stoul(mSettings.color_nodes)));
        }
        catch (std::exception&) {
            throw std::runtime_error("Unrecognized TreeDrawSettings.color_nodes: " + mSettings.color_nodes);
        }
    }

} // TreeDraw::make_coloring

// ----------------------------------------------------------------------

void TreeDraw::init_settings(const Clades* aClades)
{
      // set_line_no();    // line_no is necessary to sort detected hz sections, but set_line_no is called by SignaturePageDraw::init_settings(), so this call is redundant
    mHzSections.auto_detect(mTree, aClades);

} // TreeDraw::init_settings

// ----------------------------------------------------------------------

void TreeDraw::prepare(const LocDb& aLocDb)
{
    mTree.set_continents(aLocDb);
    mTree.set_number_strains();
    mTree.ladderize(mSettings.ladderize);
    mTree.compute_cumulative_edge_length();
    if (apply_mods()) {
          // nodes were hidden:
        mTree.set_number_strains();
        mTree.ladderize(mSettings.ladderize);
        mTree.compute_cumulative_edge_length();
    }
    mTree.make_aa_transitions();
    set_line_no();

    const size_t number_of_hz_sections = prepare_hz_sections();
    const auto& canvas_size = mSurface.viewport().size;
    mHorizontalStep = canvas_size.width / mTree.width();
    mVerticalStep = (canvas_size.height - (number_of_hz_sections - 1) * mHzSections.vertical_gap) / static_cast<double>(mTree.height() + 2); // +2 to add space at the top and bottom
    set_vertical_pos();

} // TreeDraw::prepare

// ----------------------------------------------------------------------

bool TreeDraw::apply_mods()
{
    for (const auto& mod: mSettings.mods) {
        if (mod.mod == "root") {
            std::cout << "TREE-mod: " << mod.mod << " " << mod.s1 << std::endl;
            mTree.re_root(mod.s1);
        }
        else if (mod.mod == "hide-isolated-before") {
            std::cout << "TREE-mod: " << mod.mod << " " << mod.s1 << std::endl;
            hide_isolated_before(mod.s1);
        }
        else if (mod.mod == "hide-if-cumulative-edge-length-bigger-than") {
            std::cout << "TREE-mod: " << mod.mod << " " << mod.d1 << std::endl;
            hide_if_cumulative_edge_length_bigger_than(mod.d1);
        }
        else if (mod.mod == "before2015-58P-or-146I-or-559I") {
            std::cout << "TREE-mod: " << mod.mod << std::endl;
            hide_before2015_58P_or_146I_or_559I();
        }
        else if (mod.mod == "hide-between") {
            std::cout << "TREE-mod: " << mod.mod << " \"" << mod.s1 << "\" \"" << mod.s2 << "\"" << std::endl;
            hide_between(mod.s1, mod.s2);
        }
        else if (mod.mod.empty() || mod.mod[0] == '?') {
              // commented out mod
        }
        else
            throw std::runtime_error("Unrecognized tree mod: " + mod.mod);
    }
    return !mSettings.mods.empty();

} // TreeDraw::apply_mods

// ----------------------------------------------------------------------

void TreeDraw::draw()
{
    std::cout << "Tree surface: " << mSurface.viewport() << std::endl;
    mLineWidth = mSettings.force_line_width ? mSettings.line_width : std::min(mSettings.line_width, mVerticalStep * 0.5);
      // std::cout << "Tree line width: " << mLineWidth.valeu() << "  Settings: " << mSettings.line_width << "  vertical_step/2: " << mVerticalStep * 0.5 << std::endl;
    fit_labels_into_viewport();

    double vertical_gap = 0;
    draw_node(mTree, 0 /*mLineWidth / 2*/, vertical_gap, mSettings.root_edge);
    mColoring->report();
    draw_legend();

    // mark_nodes(aSurface);

} // TreeDraw::draw

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
    size_t hidden = 0;
    auto hide_show_leaf = [aFirst,aLast,&hiding,&hidden](Node& aNode) {
        if (aNode.seq_id == aFirst) {
            if (hiding)
                throw std::runtime_error("tree hide_between: first node found and hiding is active: " + aFirst);
            hiding = true;
            aNode.draw.shown = false;
            ++hidden;
        }
        else if (aNode.seq_id == aLast) {
            if (!hiding)
                throw std::runtime_error("tree hide_between: last node found and hiding is not active: " + aLast);
            hiding = false;
            aNode.draw.shown = false; // hide the last node
            ++hidden;
        }
        else if (hiding) {
            aNode.draw.shown = false;
            ++hidden;
        }
    };

    tree::iterate_leaf_post(mTree, hide_show_leaf, hide_branch);
    if (hiding)
        throw std::runtime_error("tree hide_between: last node not found: " + aLast);
    if (hidden == 0)
        throw std::runtime_error("tree hide_between: no nodes hidden");
    std::cout << "leaf nodes hidden: " << hidden << std::endl;

} // TreeDraw::hide_between

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
    size_t current_line = 1;    // line of the first node is 1, we have 1 line space at the top and bottom of the tree
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
    size_t section_index = 0;
    for (const auto& section: mHzSections.sections) {
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
        ++section_index;
    }
    if (number_of_hz_sections == 0)
        number_of_hz_sections = 1;
    std::cerr << "HZ sections: " << number_of_hz_sections << std::endl;
    return number_of_hz_sections;

} // TreeDraw::prepare_hz_sections

// ----------------------------------------------------------------------

void TreeDraw::calculate_name_offset()
{
    const auto tsize = mSurface.text_size("W", mFontSize, mSettings.label_style);
    mNameOffset  = mSettings.name_offset * tsize.width;

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
            if (!aNode.draw.vaccine_label.empty()) {
                const auto& settings = mSettings.vaccine(aNode.draw.vaccine_label);
                Size label_offset{-20, 20}; // TODO: settings
                const Location label_origin = text_origin + label_offset;
                mSurface.text(label_origin, aNode.draw.vaccine_label, settings.label_color, Pixels{settings.label_size}, settings.label_style);
                const auto vlsize = mSurface.text_size(aNode.draw.vaccine_label, Pixels{settings.label_size}, settings.label_style);
                const auto line_origin = label_origin + Size(vlsize.width / 2, label_offset.height > 0 ? - vlsize.height : 0);
                mSurface.line(line_origin, text_origin, settings.line_color, Pixels{settings.line_width});
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
    const auto& settings = mSettings.aa_transition;
    if (settings.show && !aNode.data.aa_transitions.empty() && aNode.data.number_strains >= settings.number_strains_threshold) {
        std::vector<std::pair<std::string, const Node*>> labels;
        aNode.data.aa_transitions.make_labels(labels, settings.show_empty_left);
        if (!labels.empty()) {
            const auto& branch_settings = settings.per_branch;
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
            const Location connection_line_end{aOrigin.x + longest_label_size.width / 2 + offset.width, aOrigin.y - longest_label_size.height + offset.height};
            if (distance(connection_line_start, connection_line_end) > 10) {
                mSurface.line(connection_line_start, connection_line_end, branch_settings.label_connection_line_color, mLineWidth /*branch_settings.label_connection_line_width*/);
            }

            std::cout << "AA transitions: ";
            std::transform(labels.begin(), labels.end(), std::ostream_iterator<std::string>(std::cout, " "), [](const auto& e) -> std::string { return e.first; });
            std::cout << " --> " << /* aNode.branch_id << */ " number_strains: " << aNode.data.number_strains << std::endl;
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
        Surface& legend_surface = mSurface.subsurface(mSettings.legend.offset, Scaled{mSettings.legend.width}, legend->size(), false);
        legend->draw(legend_surface, mSettings.legend);
          // legend_surface->border("red", 1);
    }

} // TreeDraw::draw_legend

// ----------------------------------------------------------------------

void HzSections::sort(const Tree& aTree)
{
    auto set_first_node = [this](const Node& node) {
        auto sec = std::find_if(sections.begin(), sections.end(), [&node](const auto& s) -> bool { return s.name == node.seq_id; });
        if (sec != sections.end()) {
            sec->first = &node;
        }
    };

    tree::iterate_leaf(aTree, set_first_node);

      // remove not found sections before sorting (e.g. having no name or not found name)
    auto remove_section = [](const auto& a) -> bool {
        const bool remove = a.first == nullptr;
        if (remove)
            std::cerr << "WARNING: HZ section removed (leaf node not found): " << a.name << std::endl;
        return remove;
    };
    sections.erase(std::remove_if(sections.begin(), sections.end(), remove_section), sections.end());

    std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.first->draw.line_no < b.first->draw.line_no; });

    for (size_t sec_index = 1; sec_index < sections.size(); ++sec_index) {
        sections[sec_index - 1].last = aTree.find_leaf_by_line_no(sections[sec_index].first->draw.line_no - 1);
    }
    sections.back().last = &find_last_leaf(aTree);

    size_t section_index = 0;
    for (auto& section: sections) {
        if (section.show and section.show_map) {
            section.index.assign(1, 'A' + static_cast<char>(section_index));
            ++section_index;
        }
    }

} // HzSections::sort

// ----------------------------------------------------------------------

void HzSections::auto_detect(Tree& aTree, const Clades* aClades)
{
    if (sections.empty()) {
        sections.emplace_back(find_first_leaf(aTree).seq_id, false); // false -> do not show line

        if (aClades) {
            for (const auto& clade: *aClades) {
                if (clade.second.shown()) {
                    std::vector<std::pair<std::string, std::string>> seq_ids;
                    clade.second.seq_ids(seq_ids);
                    for (const auto& s: seq_ids) {
                        sections.emplace_back(s.first, false); // false -> do not show line
                        const Node* last_node_of_clade = aTree.find_leaf_by_seqid(s.second);
                        const Node* next_node = aTree.find_leaf_by_line_no(last_node_of_clade->draw.line_no + 1);
                        if (next_node)
                            sections.emplace_back(next_node->seq_id, false); // false -> do not show line
                    }
                }
            }
        }

        const size_t number_of_sections_to_detect = sections.size() + 5;

        std::vector<const Node*> nodes;
        aTree.leaf_nodes_sorted_by_distance_from_previous(nodes);

          // size_t i = 0;
          // std::cout << "HZ lines detection (distance_from_previous):" << std::endl;
          // for (const auto node: nodes) {
          //     std::cout << std::fixed << std::setprecision(8) << std::setw(14) << node->data.distance_from_previous << ' ' << node->seq_id
          //               << (node->draw.shown ? "" : " *HIDDEN*")
          //               << std::endl;
          //     if (++i > 10)
          //         break;
          // }

        for (size_t node_no = 0; sections.size() <= number_of_sections_to_detect && node_no < nodes.size(); ++node_no) {
            if (nodes[node_no]->draw.shown) {
                sections.emplace_back(nodes[node_no]->seq_id, true); // true -> show line
            }
        }

          // remove sections having the same seq_id
        std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.name < b.name; });
        sections.erase(std::unique(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.name == b.name; }), sections.end());

          // detect first node for each section
        for (auto& section: sections)
            section.first = aTree.find_leaf_by_seqid(section.name);
          // sort sections by line_no of the beginning
        std::sort(sections.begin(), sections.end(), [](const auto& a, const auto& b) -> bool { return a.first->draw.line_no < b.first->draw.line_no; });
    }

} // HzSections::auto_detect

// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
