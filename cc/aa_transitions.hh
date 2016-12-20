#pragma once

#include <iostream>
#include <string>
#include <vector>

// ----------------------------------------------------------------------

class Node;

// ----------------------------------------------------------------------

class AA_Transition
{
 public:
    static constexpr const std::string::value_type Empty = ' ';
    inline AA_Transition() : left(Empty), right(Empty), pos(9999), for_left(nullptr) {}
    inline AA_Transition(size_t aPos, char aRight) : left(Empty), right(aRight), pos(aPos), for_left(nullptr) {}
    inline std::string display_name() const { return std::string(1, left) + std::to_string(pos + 1) + std::string(1, right); }
    inline bool empty_left() const { return left == Empty; }
    inline bool left_right_same() const { return left == right; }
    inline operator bool() const { return !empty_left() && !left_right_same(); } // if transition is good for display
    friend inline std::ostream& operator<<(std::ostream& out, const AA_Transition& a) { return out << a.display_name(); }

    char left;
    char right;
    size_t pos;
    const Node* for_left;       // node used to set left part, for debugging transition labels

}; // class AA_Transition

// ----------------------------------------------------------------------

class AA_Transitions : public std::vector<AA_Transition>
{
 public:
    inline void add(size_t aPos, char aRight) { emplace_back(aPos, aRight); }

      // returns if anything was removed
    inline bool remove(size_t aPos)
        {
            auto start = std::remove_if(begin(), end(), [=](const auto& e) { return e.pos == aPos; });
            bool anything_to_remove = start != end();
            if (anything_to_remove)
                erase(start, end());
            return anything_to_remove;
        }

      // returns if anything was removed
    inline bool remove(size_t aPos, char aRight)
        {
            auto start = std::remove_if(begin(), end(), [=](const auto& e) { return e.pos == aPos && e.right == aRight; });
            bool anything_to_remove = start != end();
            if (anything_to_remove) {
                erase(start, end());
            }
            return anything_to_remove;
        }

    inline const AA_Transition* find(size_t aPos) const
        {
            const auto found = std::find_if(begin(), end(), [=](const auto& e) { return e.pos == aPos; });
            return found == end() ? nullptr : &*found;
        }

    inline operator bool() const
        {
              // return std::any_of(begin(), end(), std::mem_fn(&AA_Transition::operator bool));
            return std::any_of(begin(), end(), [](const auto& a) -> bool { return a; });
        }

    inline void make_labels(std::vector<std::pair<std::string, const Node*>>& labels, bool show_empty_left = false) const
        {
            for (const auto& aa_transition: *this) {
                if (show_empty_left || !aa_transition.empty_left()) {
                    labels.emplace_back(aa_transition.display_name(), aa_transition.for_left);
                }
            }
        }

    inline void make_labels(std::vector<std::string>& labels, bool show_empty_left = false) const
        {
            for (const auto& aa_transition: *this) {
                if (show_empty_left || !aa_transition.empty_left()) {
                    labels.push_back(aa_transition.display_name());
                }
            }
        }

}; // class AA_Transitions

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
