#include "acmacs-base/stream.hh"
#include "acmacs-base/enumerate.hh"
#include "aa-at-pos-draw.hh"
#include "tree.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void AAAtPosDraw::prepare()
{
    if (mSettings.width > 0.0) {
        if (!mSettings.positions.empty()) {
            positions_.resize(mSettings.positions.size());
            auto pos_iter = positions_.begin();
            mSettings.positions.for_each([&pos_iter](size_t pos_based_1) { *pos_iter = pos_based_1 - 1; ++pos_iter; });
            collect_aa_per_pos();
        }
        else if (mSettings.diverse_index_threshold > 0UL) {
            find_most_diverse_positions();
        }
        set_colors();
    }

} // AAAtPosDraw::prepare

// ----------------------------------------------------------------------

void AAAtPosDraw::collect_aa_per_pos()
{
    auto update = [this](size_t pos, char aa) {
        if (aa != 'X')
            ++this->aa_per_pos_[pos][aa];
    };
    auto collect = [&, this](const Node& node) {
        const auto sequence = node.data.amino_acids();
        if (this->positions_.empty()) {
            for (auto [pos, aa] : acmacs::enumerate(sequence))
                update(pos, aa);
        }
        else {
            for (auto pos : this->positions_) {
                if (pos < sequence.size())
                    update(pos, sequence[pos]);
            }
        }
    };
    tree::iterate_leaf(mTree, collect);

} // AAAtPosDraw::collect_aa_per_pos

// ----------------------------------------------------------------------

void AAAtPosDraw::find_most_diverse_positions()
{
    collect_aa_per_pos();

    // https://en.wikipedia.org/wiki/Diversity_index
    using all_pos_t = std::pair<size_t, size_t>; // position, shannon_index
    std::vector<all_pos_t> all_pos(aa_per_pos_.size());
    std::transform(aa_per_pos_.begin(), aa_per_pos_.end(), all_pos.begin(), [](const auto& src) -> all_pos_t {
        const auto sum = std::accumulate(src.second.begin(), src.second.end(), 0UL, [](auto accum, const auto& entry) { return accum + entry.second; });
        const auto shannon_index = -std::accumulate(src.second.begin(), src.second.end(), 0.0, [sum = double(sum)](auto accum, const auto& entry) {
            const double p = entry.second / sum;
            return accum + p * std::log(p);
        });
        return {src.first, static_cast<size_t>(std::lround(shannon_index * 100))};
    });
    // sort all_pos by shannon_index, more diverse first
    std::sort(all_pos.begin(), all_pos.end(), [](const auto& p1, const auto& p2) { return p1.second > p2.second; });
    const auto last = std::find_if(all_pos.begin(), all_pos.end(), [this](const auto& entry) { return entry.second < this->mSettings.diverse_index_threshold; });
    positions_.resize(static_cast<size_t>(last - all_pos.begin()));
    std::transform(all_pos.begin(), last, positions_.begin(), [](const all_pos_t& entry) { return entry.first; });

    if (mSettings.report_most_diverse_positions) {
        std::cout << "\nINFO: most diverse positions" << '\n';
        for (const auto& pos_index : all_pos) {
            if (pos_index.second > 0)
                std::cout << std::setw(3) << std::right << (pos_index.first + 1) << ' ' << std::setw(4) << std::right << pos_index.second << ' ' << aa_per_pos_[pos_index.first] << '\n';
        }
        std::cout << '\n';
    }

} // AAAtPosDraw::find_most_diverse_positions

// ----------------------------------------------------------------------

void AAAtPosDraw::set_colors()
{
    if (!positions_.empty()) {
        for (auto pos : positions_) {
            const auto& aa_freq = aa_per_pos_[pos];
            std::vector<char> aas(aa_freq.size());
            std::transform(aa_freq.begin(), aa_freq.end(), aas.begin(), [](const auto& entry) { return entry.first; });
            std::sort(aas.begin(), aas.end(), [&](char aa1, char aa2) { return aa_freq.find(aa1)->second > aa_freq.find(aa2)->second; }); // most frequent aa first
            // std::cout << pos << ' ' << aas << ' ' << aa_freq << '\n';
            for (size_t no = 1; no < aas.size(); ++no) // no color for the most frequent aa
                colors_[pos].emplace(aas[no], Color::distinct(no));
        }
    }

} // AAAtPosDraw::set_colors

// ----------------------------------------------------------------------

struct AAPosSection
{
    const Node* first;
    const Node* last;
    char aa;
    size_t num_nodes;
};

void AAAtPosDraw::make_aa_pos_sections(bool init_settings, size_t hz_section_threshold)
{
    if (!positions_.empty()) {
        std::cout << "\nINFO: sections for positions (small sections eliminated [threshold: " << mSettings.small_section_threshold << "], adjacent sections merged, most frequent AA sections removed)\n";
        for (auto pos : positions_) {
            std::vector<AAPosSection> sections;
            tree::iterate_leaf(mTree, [&](const Node& node) {
                if (const auto sequence = node.data.amino_acids(); pos < sequence.size()) {
                    const auto aa = sequence[pos];
                    if (sections.empty() || sections.back().aa != aa) {
                        sections.emplace_back(AAPosSection{&node, &node, aa, 1});
                    }
                    else {
                        ++sections.back().num_nodes;
                        sections.back().last = &node;
                    }
                }
                else {
                    if (pos < 500)
                        std::cerr << "WARNING: AAAtPosDraw::make_aa_pos_sections: sequence " << node.seq_id << " is too short: " << sequence.size() << " pos: " << pos << '\n';
                }
            });

            // std::cout << ' ' << std::setw(3) << std::right << (pos + 1) << '\n';
            // for (const auto& section : sections)
            //     std::cout << "   " << section.aa << ' ' << std::setw(4) << std::right << section.num_nodes << ' ' << section.first->seq_id << " -- " << section.last->seq_id << '\n';

            // remove small sections
            sections.erase(
                std::remove_if(sections.begin(), sections.end(), [threshold = static_cast<size_t>(mSettings.small_section_threshold)](const auto& section) { return section.num_nodes <= threshold; }),
                sections.end());

            // merge adjacent sections
            for (auto section_it = sections.begin(); section_it != sections.end(); /* no increment! */) {
                if (section_it != sections.begin() && (section_it - 1)->aa == section_it->aa) {
                    (section_it - 1)->last = section_it->last;
                    (section_it - 1)->num_nodes += section_it->num_nodes;
                    section_it = sections.erase(section_it);
                }
                else
                    ++section_it;
            }

            // remove sections for the most frequent AA
            std::map<char, size_t> aa_freq;
            for (const auto& section : sections)
                aa_freq[section.aa] += section.num_nodes;
            const auto most_freq_aa = std::max_element(aa_freq.begin(), aa_freq.end(), [](const auto& e1, const auto& e2) { return e1.second < e2.second; })->first;
            sections.erase(std::remove_if(sections.begin(), sections.end(), [most_freq_aa](const auto& section) { return section.aa == most_freq_aa; }), sections.end());

            if (init_settings) {
                for (const auto& section : sections) {
                    auto sect = mSettings.sections.append();
                    sect->pos = pos + 1;
                    sect->aa = std::string(1, section.aa);
                    sect->num = section.num_nodes;
                    if (section.first)
                        sect->first = section.first->seq_id;
                    if (section.last)
                        sect->last = section.last->seq_id;
                }

                for (const auto& section : sections) {
                    if (section.num_nodes >= hz_section_threshold) {
                          // std::cout << "INFO: aa-at-pos - add hz section: " << std::setw(4) << std::right << (pos + 1) << ' ' << std::setw(4) << std::right << section.num_nodes << ' ' << section.first->seq_id << ':' << section.first->draw.line_no << ' ' << section.last->seq_id << '\n';
                        mHzSections.add(mTree, *section.first, *section.last, true, std::string{}, pos + 1);
                    }
                }
            }

            std::cout << ' ' << std::setw(3) << std::right << (pos + 1) << '\n';
            for (const auto& section : sections)
                std::cout << "   " << section.aa << ' ' << std::setw(4) << std::right << section.num_nodes << ' ' << section.first->seq_id << " -- " << section.last->seq_id << '\n';
        }
        std::cout << '\n';
    }

    mHzSections.sort(mTree);
      // mHzSections.report(std::cout);

} // AAAtPosDraw::make_aa_pos_sections

// ----------------------------------------------------------------------

void AAAtPosDraw::draw(bool init_settings, size_t hz_section_threshold)
{
    if (!positions_.empty()) {
        make_aa_pos_sections(init_settings, hz_section_threshold); // must be here, after ladderrizing
        const auto surface_width = mSurface.viewport().size.width;
        const auto section_width = surface_width / positions_.size();
        const auto line_length = section_width * mSettings.line_length;

        auto draw_dash = [&, this](const Node& aNode) {
            const auto sequence = aNode.data.amino_acids();
            for (auto [section_no, pos] : acmacs::enumerate(this->positions_)) {
                if (pos < sequence.size()) {
                    const auto aa = sequence[pos];
                    const auto base_x = section_width * section_no + (section_width - line_length) / 2;
                    const std::string aa_s(1, aa);
                    mSurface.text({base_x, aNode.draw.vertical_pos + this->mSettings.line_width / 2}, aa_s, BLACK /* found->second */, Pixels{this->mSettings.line_width});
                    if (const auto color_p = this->colors_[pos].find(aa); color_p != colors_[pos].end()) {
                        const auto aa_width = mSurface.text_size(aa_s, Pixels{this->mSettings.line_width}).width * 2;
                        mSurface.line({base_x + aa_width, aNode.draw.vertical_pos}, {base_x + line_length - aa_width, aNode.draw.vertical_pos}, color_p->second, Pixels{this->mSettings.line_width},
                                      acmacs::surface::LineCap::Round);
                    }
                }
            }
        };
        tree::iterate_leaf(mTree, draw_dash);

        // const auto pos_text_height = mSurface.text_size("8", Pixels{}).height;
        for (size_t section_no = 0; section_no < positions_.size(); ++section_no) {
            const auto pos = positions_[section_no];
            mSurface.text({section_width * section_no + section_width / 4, mSurface.viewport().size.height + 10}, std::to_string(pos + 1), BLACK, Pixels{line_length}, acmacs::TextStyle{},
                          Rotation{M_PI_2});
        }

        draw_hz_section_lines();
    }

} // AAAtPosDraw::draw

// ----------------------------------------------------------------------

void AAAtPosDraw::draw_hz_section_lines() const
{
    const auto surface_width = mSurface.viewport().size.width;
    const auto section_width = surface_width / positions_.size();
    double previous_vertical_pos = -1e-8;
    auto draw = [&](const Node& node) {
        if (node.draw.shown) {
            if (node.draw.hz_section_index != NodeDrawData::HzSectionNoIndex) {
                if (const auto& section = mHzSections.sections[node.draw.hz_section_index]; section->show_line) {
                    const auto y = (previous_vertical_pos + node.draw.vertical_pos) / 2;
                    mSurface.line({0, y}, {mSurface.viewport().size.width, y}, mHzSections.line_color, Pixels{mHzSections.line_width}, acmacs::surface::Dash::Dash3);
                    mSurface.text({-20, y}, std::to_string(node.draw.line_no), BLACK, Pixels{6});
                    section->triggering_aa_pos.for_each([this,section_width,y](size_t aa_pos) {
                        const auto section_no = std::find(positions_.begin(), positions_.end(), (aa_pos - 1)) - positions_.begin();
                        mSurface.line({section_width * section_no + section_width * 0.25, y}, {section_width * (section_no + 1) - section_width * 0.25, y}, BLACK, Pixels{mHzSections.line_width * 2});
                        // std::cerr << "DEBUG: " << node.draw.line_no << " triggering_aa_pos: " << aa_pos << ' ' << section_no << '\n';
                    });
                }
            }
            previous_vertical_pos = node.draw.vertical_pos;
        }
    };
    tree::iterate_leaf(mTree, draw);

} // AAAtPosDraw::draw_hz_section_lines

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
