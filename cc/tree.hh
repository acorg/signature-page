#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "acmacs-base/date.hh"
#include "seqdb/seqdb.hh"
#include "aa_transitions.hh"

// ----------------------------------------------------------------------

class LocDb;

// ----------------------------------------------------------------------

// SeqDb and HiDb access data
class NodeData
{
 public:
    inline NodeData()
        : number_strains(1), ladderize_max_edge_length(0), distance_from_previous(-1)
        {}

    inline std::string date() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.entry().date() : std::string{}; }
    inline std::string amino_acids() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.seq().amino_acids(true) : std::string{}; }
    inline const std::vector<std::string>* clades() const { return mSeqdbEntrySeq ? &mSeqdbEntrySeq.seq().clades() : nullptr; }
    inline const std::vector<std::string>* hi_names() const { return mSeqdbEntrySeq ? &mSeqdbEntrySeq.seq().hi_names() : nullptr; }

    inline void assign(seqdb::SeqdbEntrySeq&& entry_seq) { mSeqdbEntrySeq.assign(std::forward<seqdb::SeqdbEntrySeq>(entry_seq)); }
    void set_continent(const LocDb& locdb, std::string seq_id);

    size_t number_strains;
    double ladderize_max_edge_length;
    std::string ladderize_max_date;
    std::string ladderize_max_name_alphabetically;
    double cumulative_edge_length;
    double distance_from_previous; // for hz sections auto-detection
    std::string continent;

    std::string aa_at;          // see make_aa_at()
    AA_Transitions aa_transitions;

 private:
    seqdb::SeqdbEntrySeq mSeqdbEntrySeq;

};

// ----------------------------------------------------------------------

//  drawing related stuff
class NodeDrawData
{
 public:
    constexpr static const size_t HzSectionNoIndex = static_cast<size_t>(-1);

    inline NodeDrawData()
        : shown(true), line_no(0), hz_section_index(HzSectionNoIndex), vertical_pos(-1) {}

    bool shown;
    size_t line_no;
    size_t hz_section_index;
    double vertical_pos;
    std::string vaccine_label;
    size_t chart_antigen_index;
};

// ----------------------------------------------------------------------

class Node
{
 public:
    typedef std::vector<Node> Subtree;

    inline Node() : edge_length(0) {}
      // inline Node(std::string aSigId, double aEdgeLength, const Date& aDate = Date()) : edge_length(aEdgeLength), sig_id(aSigId) {}
    // inline Node(Node&&) = default;
    // inline Node(const Node&) = default;
    // inline Node& operator=(Node&&) = default; // needed for swap needed for sort

    double edge_length;              // indent of node or subtree
    std::string seq_id;                // node name or branch annotation
    Subtree subtree;

    NodeData data;
    mutable NodeDrawData draw;

    inline bool is_leaf() const { return subtree.empty() && !seq_id.empty(); }
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

// ----------------------------------------------------------------------

class Tree : public Node
{
 public:
    enum class LadderizeMethod { MaxEdgeLength, NumberOfLeaves };

    inline Tree() : Node(), mMaxCumulativeEdgeLength(-1) {}

    void match_seqdb(const seqdb::Seqdb& seqdb);
    void ladderize(LadderizeMethod aLadderizeMethod);

    void set_number_strains();
    void set_continents(const LocDb& locdb);
    void make_aa_transitions(); // for all positions
    void make_aa_transitions(const std::vector<size_t>& aPositions);
    void add_vaccine(std::string aSeqId, std::string aLabel);

    inline void compute_cumulative_edge_length()
        {
            if (mMaxCumulativeEdgeLength < 0)
                Node::compute_cumulative_edge_length(0, mMaxCumulativeEdgeLength);
        }
    void compute_distance_from_previous();

    size_t height() const; // number of lines in the tree
    inline double width() { compute_cumulative_edge_length(); return mMaxCumulativeEdgeLength; }
    double width(double ignore_if_cumulative_edge_length_bigger_than);

    void report_cumulative_edge_length(std::ostream& out);

    inline void leaf_nodes_sorted_by_cumulative_edge_length(std::vector<const Node*>& nodes)
        {
            compute_cumulative_edge_length();
            leaf_nodes_sorted_by(nodes, [](const Node* a, const Node* b) -> bool { return a->data.cumulative_edge_length > b->data.cumulative_edge_length; });
        }

    inline void leaf_nodes_sorted_by_date(std::vector<const Node*>& nodes) const
        {
            leaf_nodes_sorted_by(nodes, [](const Node* a, const Node* b) -> bool { return a->data.date() < b->data.date(); });
        }

    inline void leaf_nodes_sorted_by_distance_from_previous(std::vector<const Node*>& nodes)
        {
            compute_distance_from_previous();
            leaf_nodes_sorted_by(nodes, [](const Node* a, const Node* b) -> bool { return a->data.distance_from_previous > b->data.distance_from_previous; });
        }

    // inline leaf_nodes_sorted_by_edge_length_to_next(std::vector<const Node*>& nodes) const // longest first!
    //     {
    //         compute_cumulative_edge_length();
    //         leaf_nodes_sorted_by(nodes, [](const Node* a, const Node* b) -> bool { return b->data.edge_length_to_next < a->data.edge_length_to_next; });
    //     }

    // returns nullptr if not found
    Node* find_leaf_by_seqid(std::string aSeqId);

    void sequences_per_month(std::map<Date, size_t>& spm) const;

 private:
    double mMaxCumulativeEdgeLength;

    size_t longest_aa() const;
    void make_aa_at(const std::vector<size_t>& aPositions);
    void leaf_nodes_sorted_by(std::vector<const Node*>& nodes, const std::function<bool(const Node*,const Node*)>& cmp) const;

}; // class Tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
