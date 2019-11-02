#include <iomanip>

#include "acmacs-base/float.hh"
#include "acmacs-base/fmt.hh"
#include "acmacs-virus/virus-name.hh"
#include "acmacs-chart-2/chart.hh"
#include "locationdb/locdb.hh"
#include "signature-page/tree.hh"

// ----------------------------------------------------------------------

void Tree::match_seqdb()
{
    if (const auto& seqdb = acmacs::seqdb::get(); !seqdb.empty()) {
        const auto& seq_id_index = seqdb.seq_id_index();
        tree::iterate_leaf(*this, [&seq_id_index](Node& node) {
            if (const auto found = seq_id_index.find(acmacs::seqdb::seq_id_t{node.seq_id}); found != seq_id_index.end())
                node.data.assign(found->second);
            else
                fmt::print(stderr, "WARNING: {} not found in seqdb\n", node.seq_id);
        });
    }

} // Tree::match_seqdb

// ----------------------------------------------------------------------

void Tree::ladderize(Tree::LadderizeMethod aLadderizeMethod)
{
      // std::cerr << "DEBUG: Tree: ladderizing" << '\n';

    auto set_max_edge = [](Node& aNode) {
        aNode.data.ladderize_max_edge_length = aNode.edge_length;
        aNode.data.ladderize_max_date = aNode.data.date();
        aNode.data.ladderize_max_name_alphabetically = aNode.seq_id;
    };

    auto compute_max_edge = [](Node& aNode) {
        auto const max_subtree_edge_node = std::max_element(aNode.subtree.begin(), aNode.subtree.end(), [](auto const& a, auto const& b) { return a.data.ladderize_max_edge_length < b.data.ladderize_max_edge_length; });
        aNode.data.ladderize_max_edge_length = aNode.edge_length + max_subtree_edge_node->data.ladderize_max_edge_length;
        aNode.data.ladderize_max_date = std::max_element(aNode.subtree.begin(), aNode.subtree.end(), [](auto const& a, auto const& b) { return a.data.ladderize_max_date < b.data.ladderize_max_date; })->data.ladderize_max_date;
        aNode.data.ladderize_max_name_alphabetically = std::max_element(aNode.subtree.begin(), aNode.subtree.end(), [](auto const& a, auto const& b) { return a.data.ladderize_max_name_alphabetically < b.data.ladderize_max_name_alphabetically; })->data.ladderize_max_name_alphabetically;
    };

      // set max_edge_length field for every node
    tree::iterate_leaf_post(*this, set_max_edge, compute_max_edge);

    auto reorder_by_max_edge_length = [](const Node& a, const Node& b) -> bool {
        bool r = false;
        if (float_equal(a.data.ladderize_max_edge_length, b.data.ladderize_max_edge_length)) {
            if (a.data.ladderize_max_date == b.data.ladderize_max_date) {
                r = a.data.ladderize_max_name_alphabetically < b.data.ladderize_max_name_alphabetically;
            }
            else {
                r = a.data.ladderize_max_date < b.data.ladderize_max_date;
            }
        }
        else {
            r = a.data.ladderize_max_edge_length < b.data.ladderize_max_edge_length;
        }
        return r;
    };

    auto reorder_by_number_of_leaves = [&reorder_by_max_edge_length](const Node& a, const Node& b) -> bool {
        bool r = false;
        if (a.data.number_strains == b.data.number_strains) {
            r = reorder_by_max_edge_length(a, b);
        }
        else {
            r = a.data.number_strains < b.data.number_strains;
        }
        return r;
    };

    switch (aLadderizeMethod) {
      case LadderizeMethod::None:
          break;
      case LadderizeMethod::MaxEdgeLength:
          tree::iterate_post(*this, [&reorder_by_max_edge_length](Node& aNode) { std::sort(aNode.subtree.begin(), aNode.subtree.end(), reorder_by_max_edge_length); });
          break;
      case LadderizeMethod::NumberOfLeaves:
          tree::iterate_post(*this, [&reorder_by_number_of_leaves](Node& aNode) { std::sort(aNode.subtree.begin(), aNode.subtree.end(), reorder_by_number_of_leaves); });
          break;
    }

} // Tree::ladderize

// ----------------------------------------------------------------------

void Tree::set_number_strains()
{
    // std::cerr << "DEBUG: Tree: set number strains" << '\n';

    auto set_number_strains = [](Node& aNode) {
        aNode.data.number_strains = 0;
        for (const auto& subnode: aNode.subtree) {
            if (aNode.draw.shown)
                aNode.data.number_strains += subnode.data.number_strains;
        }
    };
    tree::iterate_post(*this, set_number_strains);

} // Tree::set_number_strains

// ----------------------------------------------------------------------

void Tree::set_continents()
{
    // std::cerr << "DEBUG: Tree: set continents" << '\n';

    tree::iterate_leaf(*this, [](Node& aNode) { aNode.data.set_continent(aNode.seq_id); });

} // Tree::set_continents

// ----------------------------------------------------------------------

size_t Tree::height() const
{
    size_t height = find_last_leaf(*this).draw.line_no;
    if (height == 0) {
        fmt::print(stderr, "WARNING: (Tree::height) cannot find last leaf line_no\n");
        height = data.number_strains; // lines were not numbered, use number of leaves
    }
    return height;

} // Tree::height

// ----------------------------------------------------------------------

void NodeData::set_continent(std::string seq_id)
{
    if (mSeqdbRef)
        continent = mSeqdbRef.entry->continent;
    if (continent.empty()) {
        try {
            continent = get_locdb().continent(virus_name::location(seq_id), "UNKNOWN");
        }
        catch (virus_name::Unrecognized&) {
            continent = "UNKNOWN";
        }
    }

} // NodeData::set_continent

// ----------------------------------------------------------------------

void Node::compute_cumulative_edge_length(double initial_edge_length, double& max_cumulative_edge_length)
{
    if (draw.shown) {
        data.cumulative_edge_length = initial_edge_length + edge_length;
        if (!is_leaf()) {
            for (auto& node: subtree) {
                node.compute_cumulative_edge_length(data.cumulative_edge_length, max_cumulative_edge_length);
            }
        }
        else if (data.cumulative_edge_length > max_cumulative_edge_length) {
            max_cumulative_edge_length = data.cumulative_edge_length;
        }
    }
    else {
        data.cumulative_edge_length = -1;
    }

} // Node::compute_cumulative_edge_length

// ----------------------------------------------------------------------

std::string Node::display_name() const
{
    return fmt::format("{} {}", seq_id, data.date());

} // Node::display_name

// ----------------------------------------------------------------------

void Tree::report_cumulative_edge_length(std::ostream& out)
{
    const auto nodes = leaf_nodes_sorted_by_cumulative_edge_length();
    const auto min_edge_length = nodes.back()->data.cumulative_edge_length;
    for (const auto& node: nodes) {
        out << std::fixed << std::setprecision(8) << std::setw(10) << node->data.cumulative_edge_length
            << " (" << std::fixed << std::setprecision(2) << std::setw(5) << (node->data.cumulative_edge_length / min_edge_length) << ") "
            << node->seq_id << '\n';
    }

} // Tree::report_cumulative_edge_length

// ----------------------------------------------------------------------

void Tree::list_strains(std::ostream& out)
{
    out << "Strains in order in the tree, distance to previous\n";
    compute_distance_from_previous();
    auto report = [&out](const Node& aNode) { out << aNode.seq_id << ' ' << aNode.data.distance_from_previous << '\n'; };
    tree::iterate_leaf(*this, report);

} // Tree::list_strains

// ----------------------------------------------------------------------

void Tree::report_first_node_of_subtree(std::ostream& out, size_t leaf_nodes_threshold)
{
    out << "first nodes of subtrees having more than " << (leaf_nodes_threshold > 0 ? leaf_nodes_threshold - 1 : leaf_nodes_threshold) << " leaves\n";
    bool report_leaf = false;
    auto pre_report = [&out, leaf_nodes_threshold, &report_leaf](const Node& aNode) {
        if (aNode.data.number_strains >= leaf_nodes_threshold) {
            out << "leaves: " << aNode.data.number_strains << '\n';
            if (aNode.data.aa_transitions) {
                out << "transitions: " << aNode.data.aa_transitions << '\n';
            }
            report_leaf = true;
        }
    };
    auto leaf_report = [&out, &report_leaf](const Node& aNode) {
        if (report_leaf) {
            out << aNode.seq_id << "\n\n";
            report_leaf = false;
        }
    };
    tree::iterate_leaf_pre(*this, leaf_report, pre_report);

} // Tree::report_first_node_of_subtree

// ----------------------------------------------------------------------

// double Tree::width(double ignore_if_cumulative_edge_length_bigger_than)
// {
//     compute_cumulative_edge_length();
//     double width = 0;
//     auto set_width = [&width,&ignore_if_cumulative_edge_length_bigger_than](Node& aNode) {
//         if (aNode.data.cumulative_edge_length <= ignore_if_cumulative_edge_length_bigger_than && aNode.data.cumulative_edge_length > width)
//             width = aNode.data.cumulative_edge_length;
//     };
//     tree::iterate_leaf(*this, set_width);
//     return width;

// } // Tree::width

// ----------------------------------------------------------------------

  // for all positions
void Tree::make_aa_transitions()
{
    const auto num_positions = longest_aa();
    if (num_positions) {
        std::vector<size_t> all_positions(num_positions);
        std::iota(all_positions.begin(), all_positions.end(), 0);
        make_aa_transitions(all_positions);
    }
    else {
        std::cerr << "WARNING: Tree:0: cannot make AA transition labels: no AA sequences present (match with seqdb?)" << std::endl;
    }

} // Tree::make_aa_transitions

// ----------------------------------------------------------------------

void Tree::make_aa_at(const std::vector<size_t>& aPositions)
{
    auto reset_aa_at = [&aPositions](Node& aNode) {
        aNode.data.aa_at = aNode.data.amino_acids();
        aNode.data.aa_at.resize(aPositions.back() + 1, AA_Transition::Empty); // actual max length of aa in child leaf nodes may be less than aPositions.back()
    };
    tree::iterate_leaf(*this, reset_aa_at);

    auto make_aa_at = [&aPositions](Node& aNode) {
        aNode.data.aa_at.assign(aPositions.back() + 1, 'X');
        for (size_t pos: aPositions) {
            for (const auto& child: aNode.subtree) {
                if (child.data.aa_at[pos] != 'X') {
                    if (aNode.data.aa_at[pos] == 'X')
                        aNode.data.aa_at[pos] = child.data.aa_at[pos];
                    else if (aNode.data.aa_at[pos] != child.data.aa_at[pos])
                        aNode.data.aa_at[pos] = AA_Transition::Empty;
                    if (aNode.data.aa_at[pos] == AA_Transition::Empty)
                        break;
                }
            }
              // If this node has AA_Transition::Empty and a child node has letter, then set aa_transition for the child (unless child is a leaf)
            if (aNode.data.aa_at[pos] == AA_Transition::Empty) {
                std::map<char, size_t> aa_count;
                for (auto& child: aNode.subtree) {
                    if (child.data.aa_at[pos] != AA_Transition::Empty && child.data.aa_at[pos] != 'X') {
                        child.data.aa_transitions.add(pos, child.data.aa_at[pos]);
                        ++aa_count[child.data.aa_at[pos]];
                          // std::cout << "aa_transition " << child.aa_transitions << " " << child.name << std::endl;
                    }
                    else {
                        const auto found = child.data.aa_transitions.find(pos);
                        if (found)
                            ++aa_count[found->right];
                    }
                }
                if (!aa_count.empty()) {
                    const auto max_aa_count = std::max_element(aa_count.begin(), aa_count.end(), [](const auto& e1, const auto& e2) { return e1.second < e2.second; });
                      // std::cout << "aa_count " << aa_count << "   max: " << max_aa_count->first << ':' << max_aa_count->second << std::endl;
                    if (max_aa_count->second > 1) {
                        aNode.remove_aa_transition(pos, max_aa_count->first, false);
                        aNode.data.aa_transitions.add(pos, max_aa_count->first);
                    }
                }
            }
        }
          // std::cout << "aa  " << aNode.aa << std::endl;
    };
    tree::iterate_post(*this, make_aa_at);

} // Tree::make_aa_at

// ----------------------------------------------------------------------

void Node::remove_aa_transition(size_t aPos, char aRight, bool aDescentUponRemoval)
{
    auto remove_aa_transition = [=](Node& aNode) -> bool {
        const bool present_any = aNode.data.aa_transitions.find(aPos);
        aNode.data.aa_transitions.remove(aPos, aRight);
        return !present_any;
    };
    if (aDescentUponRemoval)
        tree::iterate_leaf_pre(*this, remove_aa_transition, remove_aa_transition);
    else
        tree::iterate_leaf_pre_stop(*this, remove_aa_transition, remove_aa_transition);

} // Node::remove_aa_transition

// ----------------------------------------------------------------------

void Tree::make_aa_transitions(const std::vector<size_t>& aPositions)
{
    make_aa_at(aPositions);

    const auto leaf_nodes = leaf_nodes_sorted_by_cumulative_edge_length();

      // add left part to aa transitions (Derek's algorithm)
    auto add_left_part = [&](Node& aNode) {
        if (!aNode.data.aa_transitions.empty()) {
            const auto node_left_edge = aNode.data.cumulative_edge_length - aNode.edge_length;

            auto lb = leaf_nodes.begin();
            for (auto ln = leaf_nodes.begin() + 1; ln != leaf_nodes.end(); ++ln) {
                if ((*ln)->data.cumulative_edge_length < node_left_edge) {
                    lb = ln;
                    break;
                }
            }

            const Node* node_for_left = lb == leaf_nodes.begin() ? nullptr : *(lb - 1);
            for (auto& transition: aNode.data.aa_transitions) {
                if (node_for_left && node_for_left->data.amino_acids().size() > transition.pos) { // node_for_left can have shorter aa
                    transition.left = node_for_left->data.amino_acids()[transition.pos];
                    transition.for_left = node_for_left;
                }
            }
        }

          // remove transitions having left and right parts the same
        aNode.data.aa_transitions.erase(std::remove_if(aNode.data.aa_transitions.begin(), aNode.data.aa_transitions.end(), [](auto& e) { return e.left_right_same(); }), aNode.data.aa_transitions.end());

          // add transition labels information to settings
        if (aNode.data.aa_transitions) {
            std::vector<std::string> labels;
            aNode.data.aa_transitions.make_labels(labels);
              // std::cerr << labels << std::endl;

              // std::vector<std::pair<std::string, const Node*>> labels;
              // aNode.data.aa_transitions.make_labels(labels);
              // settings().draw_tree.aa_transition.add(aNode.branch_id, labels);
        }
    };
    tree::iterate_leaf_pre(*this, add_left_part, add_left_part);

} // Tree::make_aa_transitions

// ----------------------------------------------------------------------

size_t Tree::longest_aa() const
{
    size_t longest_aa = 0;
    auto find_longest_aa = [&longest_aa](const Node& aNode) {
        longest_aa = std::max(longest_aa, aNode.data.amino_acids().size());
    };
    tree::iterate_leaf(*this, find_longest_aa);
    return longest_aa;

} // Tree::longest_aa

// ----------------------------------------------------------------------

Node* Tree::find_leaf_by_seqid(std::string aSeqId)
{
    Node* result = nullptr;
    auto find_node = [&aSeqId,&result](Node& aNode) -> bool {
        if (aNode.seq_id == aSeqId)
            result = &aNode;
        return result != nullptr;
    };
    tree::iterate_leaf_stop(*this, find_node);
    return result;

} // Tree::find_leaf_by_seqid

// ----------------------------------------------------------------------

const Node* Tree::find_leaf_by_line_no(size_t line_no) const
{
    const Node* result = nullptr;
    const auto find_node = [&line_no,&result](const Node& aNode) -> bool {
        if (aNode.draw.shown && aNode.draw.line_no == line_no)
            result = &aNode;
        return result != nullptr;
    };
    tree::iterate_leaf_stop(*this, find_node);
    return result;

} // Tree::find_leaf_by_line_no

// ----------------------------------------------------------------------

const Node* Tree::find_previous_leaf(const Node& current_leaf, bool shown_only) const
{
    const Node* result = nullptr;
    const auto find_node = [&current_leaf,&result,shown_only](const Node& aNode) -> bool {
        if (&aNode == &current_leaf)
            return true;
        if (!shown_only || aNode.draw.shown)
            result = &aNode;
        return false;
    };
    tree::iterate_leaf_stop(*this, find_node);
    return result;

} // Tree::find_previous_leaf

// ----------------------------------------------------------------------

const Node* Tree::find_next_leaf(const Node& previous_leaf, bool shown_only) const
{
    const Node* result = nullptr;
    auto find_node = [&previous_leaf,&result,shown_only,previous_found=false](const Node& aNode) mutable -> bool {
        if (previous_found && (!shown_only || aNode.draw.shown))
            result = &aNode;
        else if (&aNode == &previous_leaf)
            previous_found = true;
        return result != nullptr;
    };
    tree::iterate_leaf_stop(*this, find_node);
    return result;

} // Tree::find_next_leaf

// ----------------------------------------------------------------------

void Tree::sequences_per_month(std::map<date::year_month_day, size_t>& spm) const
{
    auto worker = [&spm](const Node& aNode) -> void {
        const auto d = aNode.data.date();
        if (!d.empty() && aNode.draw.shown) {
            ++spm[date::beginning_of_month(date::from_string(d, date::allow_incomplete::yes))];
        }
    };
    tree::iterate_leaf(*this, worker);

} // Tree::sequences_per_month

// ----------------------------------------------------------------------

void Tree::compute_distance_from_previous()
{
    double distance = -1;

    auto pre_post = [&distance](const Node& aNode) -> void {
        if (distance >= 0)
            distance += aNode.edge_length;
    };

    auto leaf = [&distance](Node& aNode) -> void {
        aNode.data.distance_from_previous = distance >= 0 ? distance + aNode.edge_length : distance;
        distance = aNode.edge_length;
    };

    tree::iterate_leaf_pre_post(*this, leaf, pre_post, pre_post);

} // Tree::compute_distance_from_previous

// ----------------------------------------------------------------------

std::string Tree::virus_type() const
{
    auto [virus_type, lineage] = virus_type_lineage();
    if (!lineage.empty()) {
        virus_type.append(1, '/');
        virus_type.append(string::capitalize(lineage.substr(0, 3)));
    }
    return virus_type;

} // Tree::virus_type

// ----------------------------------------------------------------------

std::pair<std::string, std::string> Tree::virus_type_lineage() const
{
    std::string virus_type;
    auto find_virus_type = [&virus_type](const Node& aNode) -> bool {
        bool r = false;
        // fmt::print(stderr, "DEBUG: seq_id: [{}]\n", aNode.seq_id);
        const auto pos = aNode.seq_id.find('/');
        if ((pos == 1 && aNode.seq_id[0] == 'B') || (pos == 5 && aNode.seq_id[0] == 'A' && aNode.seq_id[1] == 'H')) {
            virus_type.assign(aNode.seq_id, 0, pos);
            r = true;
        }
        return r;
    };
    tree::iterate_leaf_stop(*this, find_virus_type);

    std::string lineage;
    if (virus_type == "B") {    // infer lineage
        auto find_lineage = [&lineage](const Node& aNode) -> bool {
            if (std::string_view(aNode.seq_id.data(), 27) == "B/SOUTH_AUSTRALIA/81/2012" || std::string_view(aNode.seq_id.data(), 19) == "B/IRELAND/3154/2016" || std::string_view(aNode.seq_id.data(), 19) == "B/VICTORIA/830/2013") {
                lineage = "VICTORIA";
                return true;
            }
            else if (std::string_view(aNode.seq_id.data(), 18) == "B/PHUKET/3073/2013" || std::string_view(aNode.seq_id.data(), 23) == "B/CHRISTCHURCH/503/2013") {
                lineage = "YAMAGATA";
                return true;
            }
            return false;
        };
        tree::iterate_leaf_stop(*this, find_lineage);
    }

    return {virus_type, lineage};

} // Tree::virus_type_lineage

// ----------------------------------------------------------------------


std::vector<const Node*> Tree::find_name(std::string aName) const
{
    std::vector<const Node*> path;
    bool found = find_name_r(aName, path);
    if (!found)
        throw std::runtime_error(aName + " not found in the tree");
    return path;

} // Tree::find_name

// ----------------------------------------------------------------------

std::vector<const Node*> Tree::find_nodes_matching(std::string name) const
{
    std::vector<const Node*> result;
    const auto find_matching = [&result,&name](const Node& aNode) -> void {
        if (aNode.seq_id.find(name) != std::string::npos)
            result.push_back(&aNode);
    };
    tree::iterate_leaf(*this, find_matching);
    return result;

} // Tree::find_name_substring

std::vector<Node*> Tree::find_nodes_matching(std::string name)
{
    std::vector<Node*> result;
    const auto find_matching = [&result,&name](Node& aNode) -> void {
        if (aNode.seq_id.find(name) != std::string::npos)
            result.push_back(&aNode);
    };
    tree::iterate_leaf(*this, find_matching);
    return result;

} // Tree::find_name_substring

// ----------------------------------------------------------------------

bool Node::find_name_r(std::string aName, std::vector<const Node*>& aPath) const
{
    bool found = false;
    if (is_leaf()) {
        if (seq_id == aName) {
            aPath.push_back(this);
            found = true;
        }
    }
    else {
        aPath.push_back(this);
        for (const Node& child: subtree) {
            found = child.find_name_r(aName, aPath);
            if (found)
                break;
        }
        if (!found)
            aPath.pop_back();
    }
    return found;

} // Node::find_name_r

// ----------------------------------------------------------------------

void Tree::re_root(const std::vector<const Node*>& aNewRoot)
{
      // std::cout << "TREE: re-rooting" << std::endl;

    if (aNewRoot.front() != this)
        throw std::invalid_argument("Invalid path passed to Tree::re_root");

    std::vector<Node> nodes;
    for (size_t item_no = 0; item_no < (aNewRoot.size() - 1); ++item_no) {
        const Node& source = *aNewRoot[item_no];
        nodes.push_back(Node());
        std::copy_if(source.subtree.begin(), source.subtree.end(), std::back_inserter(nodes.back().subtree), [&](const Node& to_copy) -> bool { return &to_copy != aNewRoot[item_no + 1]; });
        nodes.back().edge_length = aNewRoot[item_no + 1]->edge_length;
    }

    std::vector<Node> new_subtree(aNewRoot.back()->subtree.begin(), aNewRoot.back()->subtree.end());
    Subtree* append_to = &new_subtree;
    for (auto child = nodes.rbegin(); child != nodes.rend(); ++child) {
        append_to->push_back(*child);
        append_to = &append_to->back().subtree;
    }
    subtree = new_subtree;
    edge_length = 0;
    mMaxCumulativeEdgeLength = -1;
      // set_number_strains();

} // Tree::re_root

// ----------------------------------------------------------------------

void Tree::re_root(std::string aName)
{
    std::vector<const Node*> path = find_name(aName);
    path.pop_back();
    re_root(path);

} // Tree::re_root

// ----------------------------------------------------------------------

size_t Tree::match(const acmacs::chart::Chart& chart)
{
    auto antigens = chart.antigens();

    auto match_chart_antigens = [&](Node& node) {
        node.draw.chart_antigen_index.reset();
        if (const auto* hi_names = node.data.hi_names(); hi_names) {
            for (const auto& name : *hi_names) {
                if (const auto antigen_index = antigens->find_by_full_name(name); antigen_index) {
                    node.draw.chart_antigen_index = antigen_index;
                    break;
                }
            }
        }
    };

    auto sum_matched_antigens = [](Node& node) {
        node.draw.matched_antigens = std::accumulate(node.subtree.begin(), node.subtree.end(), 0UL, [](size_t accum, const Node& subnode) {
            if (subnode.draw.chart_antigen_index)
                return accum + 1;
            else
                return accum + subnode.draw.matched_antigens;
        });
    };

    tree::iterate_leaf_post(*this, match_chart_antigens, sum_matched_antigens);
    if (draw.matched_antigens == 0)
        std::cerr << "WARNING: No tree sequences found in the chart" << DEBUG_LINE_FUNC << '\n';

    return draw.matched_antigens;

} // Tree::match

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
