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
    AA_Transition() : left(Empty), right(Empty), pos(9999), for_left(nullptr) {}
    AA_Transition(size_t aPos, char aRight) : left(Empty), right(aRight), pos(aPos), for_left(nullptr) {}
    std::string display_name() const { return std::string(1, left) + std::to_string(pos + 1) + std::string(1, right); }
    bool empty_left() const { return left == Empty; }
    bool left_right_same() const { return left == right; }
    operator bool() const { return !empty_left() && !left_right_same(); } // if transition is good for display
    friend inline std::ostream& operator<<(std::ostream& out, const AA_Transition& a) { return out << a.display_name(); }

    char left;
    char right;
    size_t pos;
    const Node* for_left;       // node used to set left part, for debugging transition labels

}; // class AA_Transition

// ----------------------------------------------------------------------

class AA_TransitionLabels : public std::vector<std::pair<std::string, const Node*>>
{
  public:
    AA_TransitionLabels() = default;

    std::string label() const
    {
        if (mLabel.empty()) {
            auto summer = [](const std::string& accum, const auto& source) -> std::string {
                if (accum.empty())
                    return source.first;
                else
                    return accum + " " + source.first;
            };
            mLabel = std::accumulate(begin(), end(), std::string{}, summer);
        }
        return mLabel;
    }

    void add(std::string label, const Node* for_left)
        {
            if (std::find_if(begin(), end(), [&label](const auto& entry) { return entry.first == label; }) == end())
                emplace_back(label, for_left);
        }

  private:
    mutable std::string mLabel;

}; // class AA_TransitionLabels

// ----------------------------------------------------------------------

class AA_Transitions : public std::vector<AA_Transition>
{
  public:
    void add(size_t aPos, char aRight) { emplace_back(aPos, aRight); }

    // returns if anything was removed
    bool remove(size_t aPos)
    {
        auto start = std::remove_if(begin(), end(), [=](const auto& e) { return e.pos == aPos; });
        bool anything_to_remove = start != end();
        if (anything_to_remove)
            erase(start, end());
        return anything_to_remove;
    }

    // returns if anything was removed
    bool remove(size_t aPos, char aRight)
    {
        auto start = std::remove_if(begin(), end(), [=](const auto& e) { return e.pos == aPos && e.right == aRight; });
        bool anything_to_remove = start != end();
        if (anything_to_remove) {
            erase(start, end());
        }
        return anything_to_remove;
    }

    const AA_Transition* find(size_t aPos) const
    {
        const auto found = std::find_if(begin(), end(), [=](const auto& e) { return e.pos == aPos; });
        return found == end() ? nullptr : &*found;
    }

    operator bool() const
    {
        // return std::any_of(begin(), end(), std::mem_fn(&AA_Transition::operator bool));
        return std::any_of(begin(), end(), [](const auto& a) -> bool { return a; });
    }

    using label_node_t = std::vector<std::pair<std::string, const Node*>>;

    AA_TransitionLabels make_labels(bool show_empty_left = false) const
    {
        AA_TransitionLabels labels;
        for (const auto& aa_transition : *this) {
            if (show_empty_left || !aa_transition.empty_left())
                labels.add(aa_transition.display_name(), aa_transition.for_left);
        }
        return labels;
    }

    void make_labels(std::vector<std::string>& labels, bool show_empty_left = false) const
    {
        for (const auto& aa_transition : *this) {
            if (show_empty_left || !aa_transition.empty_left()) {
                labels.push_back(aa_transition.display_name());
            }
        }
    }

    bool contains(std::string label) const
    {
        return std::find_if(begin(), end(), [&label](const auto& aa_transition) { return aa_transition.display_name() == label; }) != end();
    }

}; // class AA_Transitions

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
