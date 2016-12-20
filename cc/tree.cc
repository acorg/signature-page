#include <iomanip>

#include "tree.hh"
#include "tree-iterate.hh"
#include "acmacs-base/float.hh"
#include "acmacs-base/virus-name.hh"
#include "locationdb/locdb.hh"

// ----------------------------------------------------------------------

void Tree::match_seqdb(const seqdb::Seqdb& seqdb)
{
    auto match = [&seqdb](Node& node) {
        node.data.assign(seqdb.find_by_seq_id(node.seq_id));
    };
    tree::iterate_leaf(*this, match);

} // Tree::match_seqdb

// ----------------------------------------------------------------------

void Tree::ladderize(Tree::LadderizeMethod aLadderizeMethod)
{
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
    auto set_number_strains = [](Node& aNode) {
        aNode.data.number_strains = 0;
        for (const auto& subnode: aNode.subtree) {
            aNode.data.number_strains += subnode.data.number_strains;
        }
    };
    tree::iterate_post(*this, set_number_strains);

} // Tree::set_number_strains

// ----------------------------------------------------------------------

void Tree::set_continents(const LocDb& locdb)
{
    auto set_continents = [&locdb](Node& aNode) {
        try {
            aNode.data.continent = locdb.continent(virus_name::location(name_decode(aNode.seq_id)), "UNKNOWN");
        }
        catch (virus_name::Unrecognized&) {
            aNode.data.continent = "UNKNOWN";
        }
    };
    tree::iterate_leaf(*this, set_continents);

} // Tree::set_continents

// ----------------------------------------------------------------------

size_t Tree::height() const
{
    size_t height = find_last_leaf(*this).draw.line_no;
    if (height == 0) {
        height = data.number_strains; // lines were not numbered, use number of leaves
    }
    return height;

} // Tree::height

// ----------------------------------------------------------------------

void Node::compute_cumulative_edge_length(double initial_edge_length, double& max_cumulative_edge_length)
{
    data.cumulative_edge_length = initial_edge_length + edge_length;
    if (!is_leaf()) {
        for (auto& node: subtree) {
            node.compute_cumulative_edge_length(data.cumulative_edge_length, max_cumulative_edge_length);
        }
    }
    else if (data.cumulative_edge_length > max_cumulative_edge_length) {
        max_cumulative_edge_length = data.cumulative_edge_length;
    }

} // Node::compute_cumulative_edge_length

// ----------------------------------------------------------------------

std::string Node::display_name() const
{
    return string::replace(name_decode(seq_id), "__", " ");

} // Node::display_name

// ----------------------------------------------------------------------

void Tree::report_cumulative_edge_length(std::ostream& out)
{
    compute_cumulative_edge_length();

    std::vector<std::pair<double, std::string>> cumulative_edge_length; // cumulative_edge_length and seq_id
    auto set_cumulative_edge_length = [&cumulative_edge_length](Node& aNode) {
        cumulative_edge_length.emplace_back(aNode.data.cumulative_edge_length, aNode.seq_id);
    };
    tree::iterate_leaf(*this, set_cumulative_edge_length);
    std::sort(cumulative_edge_length.begin(), cumulative_edge_length.end(), [](const auto& a, const auto& b) { return b.first < a.first; }); // descending
    for (const auto& e: cumulative_edge_length)
        out << std::fixed << std::setprecision(8) << std::setw(10) << e.first << ' ' << e.second << std::endl;

} // Tree::report_cumulative_edge_length

// ----------------------------------------------------------------------

double Tree::width(double ignore_if_cumulative_edge_length_bigger_than)
{
    compute_cumulative_edge_length();
    double width = 0;
    auto set_width = [&width,&ignore_if_cumulative_edge_length_bigger_than](Node& aNode) {
        if (aNode.data.cumulative_edge_length <= ignore_if_cumulative_edge_length_bigger_than && aNode.data.cumulative_edge_length > width)
            width = aNode.data.cumulative_edge_length;
    };
    tree::iterate_leaf(*this, set_width);
    return width;

} // Tree::width

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
