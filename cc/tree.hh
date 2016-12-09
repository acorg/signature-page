#pragma once

#include <string>
#include <vector>

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

      // leaf part
      // Date date;
    std::string aa;             // aligned AA sequence for coloring by subst
                                // for subtree: for each pos: space - children have different aa's at this pos (X not counted), letter - all children have the same aa at this pos (X not counted)

      // for coloring
    std::string continent;
    std::vector<std::string> clades;

      // subtree part
    Subtree subtree;

    inline bool is_leaf() const { return subtree.empty() && !seq_id.empty(); }
      // int months_from(const Date& aStart) const; // returns negative if date of the node is earlier than aStart

}; // class Node

// ----------------------------------------------------------------------

class Tree : public Node
{
 public:
    inline Tree() : Node() {}

}; // class Tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
