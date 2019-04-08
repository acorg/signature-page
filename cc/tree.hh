#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "acmacs-base/date.hh"
#include "acmacs-base/color.hh"
#include "acmacs-base/size-scale.hh"
#include "seqdb/seqdb.hh"
#include "aa_transitions.hh"
#include "tree-iterate.hh"

// ----------------------------------------------------------------------

namespace acmacs::chart { class Chart; }

// ----------------------------------------------------------------------

// SeqDb and HiDb access data
class NodeData
{
 public:
    NodeData() = default;

    std::string date() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.entry().date() : std::string{}; }
    std::string amino_acids() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.seq().amino_acids(true) : std::string{}; }
    const std::vector<std::string>* clades() const { return mSeqdbEntrySeq ? &mSeqdbEntrySeq.seq().clades() : nullptr; }
    bool has_clade(std::string aClade) const { const auto* cld = clades(); return cld ? std::find(cld->begin(), cld->end(), aClade) != cld->end() : false; }
    const std::vector<std::string>* hi_names() const { return mSeqdbEntrySeq ? &mSeqdbEntrySeq.seq().hi_names() : nullptr; }

    void assign(seqdb::SeqdbEntrySeq&& entry_seq) { mSeqdbEntrySeq.assign(std::forward<seqdb::SeqdbEntrySeq>(entry_seq)); }
    void set_continent(std::string seq_id);

    size_t number_strains = 1;
    double ladderize_max_edge_length = 0;
    std::string ladderize_max_date;
    std::string ladderize_max_name_alphabetically;
    double cumulative_edge_length = -1;
    double distance_from_previous = -1; // for hz sections auto-detection
    std::string continent;

    std::string aa_at;          // see make_aa_at()
    AA_Transitions aa_transitions;

 private:
    seqdb::SeqdbEntrySeq mSeqdbEntrySeq;

}; // class NodeData

// ----------------------------------------------------------------------

//  drawing related stuff
class NodeDrawData
{
 public:
    constexpr static const size_t HzSectionNoIndex = static_cast<size_t>(-1);

    NodeDrawData() = default;

    bool shown = true;
    size_t line_no = 0;
    size_t hz_section_index = HzSectionNoIndex;
    double vertical_pos = -1;
    Color mark_with_line = ColorNoChange;
    Pixels mark_with_line_width{0};
    std::optional<size_t> chart_antigen_index;
    size_t matched_antigens = 0; // for parent nodes only
    bool mark_with_label = false;

}; // class NodeDrawData

// ----------------------------------------------------------------------

class Node
{
 public:
    using Subtree = std::vector<Node>;

    Node() = default;
      // Node(std::string aSigId, double aEdgeLength, const Date& aDate = Date()) : edge_length(aEdgeLength), sig_id(aSigId) {}
    // Node(Node&&) = default;
    // Node(const Node&) = default;
    // Node& operator=(Node&&) = default; // needed for swap needed for sort

    double edge_length = 0;              // indent of node or subtree
    std::string seq_id;                // node name or branch annotation
    Subtree subtree;

    NodeData data;
    mutable NodeDrawData draw;

    bool is_leaf() const { return subtree.empty() && !seq_id.empty(); }
    std::string display_name() const;

    //   // leaf part
    //   // Date date;
    // std::string aa;             // aligned AA sequence for coloring by subst
    //                             // for subtree: for each pos: space - children have different aa's at this pos (X not counted), letter - all children have the same aa at this pos (X not counted)

    //   // for coloring
    // std::string continent;
    // std::vector<std::string> clades;


      // int months_from(const Date& aStart) const; // returns negative if date of the node is earlier than aStart

    void remove_aa_transition(size_t aPos, char aRight, bool aDescentUponRemoval); // recursively

 protected:
    void compute_cumulative_edge_length(double initial_edge_length, double& max_cumulative_edge_length);
    bool find_name_r(std::string aName, std::vector<const Node*>& aPath) const;

}; // class Node

// ----------------------------------------------------------------------

inline const Node& find_first_leaf(const Node& aNode)
{
    return aNode.is_leaf() ? aNode : find_first_leaf(aNode.subtree.front());
}

inline Node& find_first_leaf(Node& aNode)
{
    return aNode.is_leaf() ? aNode : find_first_leaf(aNode.subtree.front());
}

inline const Node& find_last_leaf(const Node& aNode)
{
    return aNode.is_leaf() ? aNode : find_last_leaf(aNode.subtree.back());
}

inline Node& find_last_leaf(Node& aNode)
{
    return aNode.is_leaf() ? aNode : find_last_leaf(aNode.subtree.back());
}

// ----------------------------------------------------------------------

class Tree : public Node
{
 public:
    enum class LadderizeMethod { MaxEdgeLength, NumberOfLeaves };

    Tree() = default;

    void match_seqdb(const seqdb::Seqdb& seqdb, seqdb::Seqdb::ignore_not_found ignore = seqdb::Seqdb::ignore_not_found::no);
    void ladderize(LadderizeMethod aLadderizeMethod);

    void set_number_strains();
    void set_continents();
    void make_aa_transitions(); // for all positions
    void make_aa_transitions(const std::vector<size_t>& aPositions);

    void compute_cumulative_edge_length()
    {
        // const double was = mMaxCumulativeEdgeLength;
        mMaxCumulativeEdgeLength = -1;
        Node::compute_cumulative_edge_length(0, mMaxCumulativeEdgeLength);
        // std::cout << "TREE: compute_cumulative_edge_length " << std::setprecision(10) << was << " --> " << mMaxCumulativeEdgeLength << '\n';
    }

    void compute_distance_from_previous();

    size_t height() const; // number of lines in the tree
    double width() { return mMaxCumulativeEdgeLength; }
    // double width(double ignore_if_cumulative_edge_length_bigger_than);

    void report_cumulative_edge_length(std::ostream& out);
    void list_strains(std::ostream& out);
    void report_first_node_of_subtree(std::ostream& out, size_t leaf_nodes_threshold);

    std::pair<double, double> cumulative_edge_minmax() const
        {
            auto nodes = leaf_nodes();
            const auto [min_node, max_node] = std::minmax_element(nodes.begin(), nodes.end(), [](const Node* a, const Node* b) -> bool { return a->data.cumulative_edge_length < b->data.cumulative_edge_length; });
            return {(*min_node)->data.cumulative_edge_length, (*max_node)->data.cumulative_edge_length};
        }

    std::vector<const Node*> leaf_nodes() const
    {
        std::vector<const Node*> result;
        tree::iterate_leaf(*this, [&result](const Node& aNode) -> void { result.push_back(&aNode); });
        return result;
    }

    std::vector<const Node*> leaf_nodes_sorted_by_cumulative_edge_length() const
    {
        // compute_cumulative_edge_length();
        return leaf_nodes_sorted_by([](const Node* a, const Node* b) -> bool { return a->data.cumulative_edge_length > b->data.cumulative_edge_length; });
    }

    std::vector<const Node*> leaf_nodes_sorted_by_date() const
    {
        return leaf_nodes_sorted_by([](const Node* a, const Node* b) -> bool { return a->data.date() < b->data.date(); });
    }

    std::vector<const Node*> leaf_nodes_sorted_by_distance_from_previous()
    {
        compute_distance_from_previous();
        return leaf_nodes_sorted_by([](const Node* a, const Node* b) -> bool { return a->data.distance_from_previous > b->data.distance_from_previous; });
    }

    // std::vector<const Node*> leaf_nodes_sorted_by_edge_length_to_next() const // longest first!
    //     {
    //         compute_cumulative_edge_length();
    //         return leaf_nodes_sorted_by([](const Node* a, const Node* b) -> bool { return b->data.edge_length_to_next < a->data.edge_length_to_next; });
    //     }

    // returns nullptr if not found
    Node* find_leaf_by_seqid(std::string aSeqId);
    const Node* find_leaf_by_line_no(size_t line_no) const;
    const Node* find_previous_leaf(const Node& aNode, bool shown_only) const;
    const Node* find_next_leaf(const Node& aNode, bool shown_only) const;

    void sequences_per_month(std::map<Date, size_t>& spm) const;
    std::string virus_type() const;
    std::pair<std::string, std::string> virus_type_lineage() const;

    std::vector<const Node*> find_name(std::string aName) const;
    std::vector<const Node*> find_nodes_matching(std::string aName) const; // returns list of nodes or empty list if nothing found
    void re_root(const std::vector<const Node*>& aNewRoot);
    // re-roots tree making the parent of the leaf node with the passed name root
    void re_root(std::string aName);

      // returns number of matched antigen names
    size_t match(const acmacs::chart::Chart& chart);

  private:
    double mMaxCumulativeEdgeLength = -1;

    size_t longest_aa() const;
    void make_aa_at(const std::vector<size_t>& aPositions);

    template <typename Cmp> std::vector<const Node*> leaf_nodes_sorted_by(Cmp&& cmp) const // Cmp: [](const Node*, const Node*) -> bool
    {
        auto result = leaf_nodes();
        std::sort(result.begin(), result.end(), cmp);
        return result;
    }

}; // class Tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
