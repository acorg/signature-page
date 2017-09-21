#pragma once

// ----------------------------------------------------------------------

namespace tree
{

// ----------------------------------------------------------------------

    template <typename N, typename F1> inline void iterate_leaf(N& aNode, F1 f_name)
    {
        if (aNode.is_leaf()) {
            f_name(aNode);
        }
        else {
            for (auto& node: aNode.subtree) {
                iterate_leaf(node, f_name);
            }
        }
    }

// ----------------------------------------------------------------------

      // stops iterating if f_name returns true
    template <typename N, typename F1> inline bool iterate_leaf_stop(N& aNode, F1 f_name)
    {
        bool stop = false;
        if (aNode.is_leaf()) {
            stop = f_name(aNode);
        }
        else {
            for (auto& node: aNode.subtree) {
                if ((stop = iterate_leaf_stop(node, f_name)))
                    break;
            }
        }
        return stop;
    }

// ----------------------------------------------------------------------

    template <typename N, typename F1, typename F3> inline void iterate_leaf_post(N& aNode, F1 f_name, F3 f_subtree_post)
    {
        if (aNode.is_leaf()) {
            f_name(aNode);
        }
        else {
            for (auto& node: aNode.subtree) {
                iterate_leaf_post(node, f_name, f_subtree_post);
            }
            f_subtree_post(aNode);
        }
    }

// ----------------------------------------------------------------------

    template <typename N, typename F1, typename F2> inline void iterate_leaf_pre(N& aNode, F1 f_name, F2 f_subtree_pre)
    {
        if (aNode.is_leaf()) {
            f_name(aNode);
        }
        else {
            f_subtree_pre(aNode);
            for (auto& node: aNode.subtree) {
                iterate_leaf_pre(node, f_name, f_subtree_pre);
            }
        }
    }

// ----------------------------------------------------------------------

      // Stop descending the tree if f_subtree_pre returned false
    template <typename N, typename F1, typename F2> inline void iterate_leaf_pre_stop(N& aNode, F1 f_name, F2 f_subtree_pre)
    {
        if (aNode.is_leaf()) {
            f_name(aNode);
        }
        else {
            if (f_subtree_pre(aNode)) {
                for (auto& node: aNode.subtree) {
                    iterate_leaf_pre_stop(node, f_name, f_subtree_pre);
                }
            }
        }
    }

// ----------------------------------------------------------------------

    template <typename N, typename F3> inline void iterate_pre(N& aNode, F3 f_subtree_pre)
    {
        if (!aNode.is_leaf()) {
            f_subtree_pre(aNode);
            for (auto& node: aNode.subtree) {
                iterate_pre(node, f_subtree_pre);
            }
        }
    }

// ----------------------------------------------------------------------

    template <typename N, typename F3> inline void iterate_post(N& aNode, F3 f_subtree_post)
    {
        if (!aNode.is_leaf()) {
            for (auto& node: aNode.subtree) {
                iterate_post(node, f_subtree_post);
            }
            f_subtree_post(aNode);
        }
    }

// ----------------------------------------------------------------------

    template <typename N, typename F1, typename F2, typename F3> inline void iterate_leaf_pre_post(N& aNode, F1 f_name, F2 f_subtree_pre, F3 f_subtree_post)
    {
        if (aNode.is_leaf()) {
            f_name(aNode);
        }
        else {
            f_subtree_pre(aNode);
            for (auto node = aNode.subtree.begin(); node != aNode.subtree.end(); ++node) {
                iterate_leaf_pre_post(*node, f_name, f_subtree_pre, f_subtree_post);
            }
            f_subtree_post(aNode);
        }
    }

// ----------------------------------------------------------------------

} // namespace tree

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
