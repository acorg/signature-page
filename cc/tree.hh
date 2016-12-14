#pragma once

#include <string>
#include <vector>

#include "seqdb/seqdb.hh"

// ----------------------------------------------------------------------

// SeqDb and HiDb access data, drawing related stuff
class NodeData
{
 public:
    inline NodeData()
        : number_strains(1), shown(true), ladderize_max_edge_length(0), line_no(0), vertical_gap_before(0), top(-1), bottom(-1)
        {}

    inline std::string date() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.entry().date() : std::string{}; }

    inline void assign(seqdb::SeqdbEntrySeq&& entry_seq) { mSeqdbEntrySeq.assign(std::forward<seqdb::SeqdbEntrySeq>(entry_seq)); }

    size_t number_strains;
    bool shown;
    double ladderize_max_edge_length;
    std::string ladderize_max_date;
    std::string ladderize_max_name_alphabetically;
    double cumulative_edge_length;
    size_t line_no;
    size_t vertical_gap_before;
    double top, bottom;         // subtree boundaries

 private:
    seqdb::SeqdbEntrySeq mSeqdbEntrySeq;

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

    inline bool is_leaf() const { return subtree.empty() && !seq_id.empty(); }
    inline double middle() const { return is_leaf() ? static_cast<double>(data.line_no) : ((data.top + data.bottom) / 2.0); }

    //   // leaf part
    //   // Date date;
    // std::string aa;             // aligned AA sequence for coloring by subst
    //                             // for subtree: for each pos: space - children have different aa's at this pos (X not counted), letter - all children have the same aa at this pos (X not counted)

    //   // for coloring
    // std::string continent;
    // std::vector<std::string> clades;


      // int months_from(const Date& aStart) const; // returns negative if date of the node is earlier than aStart

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

    inline void compute_cumulative_edge_length()
        {
            if (mMaxCumulativeEdgeLength < 0)
                Node::compute_cumulative_edge_length(0, mMaxCumulativeEdgeLength);
        }

    size_t height() const; // number of lines in the tree
    inline double width() { compute_cumulative_edge_length(); return mMaxCumulativeEdgeLength; }

 private:
    double mMaxCumulativeEdgeLength;

}; // class Tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
