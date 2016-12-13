#pragma once

#include <string>
#include <vector>

#include "seqdb/seqdb.hh"

// ----------------------------------------------------------------------

// SeqDb and HiDb access data, drawing related stuff
class NodeData
{
 public:
    inline NodeData() : number_strains(1), ladderize_max_edge_length(0) {}

    inline std::string date() const { return mSeqdbEntrySeq ? mSeqdbEntrySeq.entry().date() : std::string{}; }

    inline void assign(seqdb::SeqdbEntrySeq&& entry_seq) { mSeqdbEntrySeq.assign(std::forward<seqdb::SeqdbEntrySeq>(entry_seq)); }

    size_t number_strains;
    double ladderize_max_edge_length;
    std::string ladderize_max_date;
    std::string ladderize_max_name_alphabetically;

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

    //   // leaf part
    //   // Date date;
    // std::string aa;             // aligned AA sequence for coloring by subst
    //                             // for subtree: for each pos: space - children have different aa's at this pos (X not counted), letter - all children have the same aa at this pos (X not counted)

    //   // for coloring
    // std::string continent;
    // std::vector<std::string> clades;


      // int months_from(const Date& aStart) const; // returns negative if date of the node is earlier than aStart

}; // class Node

// ----------------------------------------------------------------------

class Tree : public Node
{
 public:
    enum class LadderizeMethod { MaxEdgeLength, NumberOfLeaves };

    inline Tree() : Node() {}

    void match_seqdb(const seqdb::Seqdb& seqdb);
    void ladderize(LadderizeMethod aLadderizeMethod);

    void set_number_strains();

}; // class Tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
