#include "acmacs-base/string.hh"
#include "settings-initializer.hh"
#include "signature-page.hh"
#include "title-draw.hh"
#include "clades-draw.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

namespace
{
    class Default : public SettingsInitializer
    {
      public:
        Default(std::string lab, std::string virus_type, std::string assay) : lab_{lab}, virus_type_{virus_type}, assay_{assay} {}

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            settings.time_series_width = 100;
            settings.bottom = settings.top;
            if (!whocc_support)
                settings.clades_width = 0;
        }

        void update(TitleDrawSettings& settings) const override { settings.title = virus_type(); }
        void update(TreeDraw& /*tree_draw*/, bool /*whocc_support*/) const override {}
        void update(CladesDrawSettings& settings, bool whocc_support) const override
        {
            if (!whocc_support)
                settings.hide_default_clade();
        }

        void update(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, bool whocc_support) const override
        {
            if (whocc_support) {
                auto matcher = [&](const auto& c) { return static_cast<std::string>(c.name) == clade.first; };
                if (auto settings_clade = settings.clades.find_if(matcher); !settings_clade) {
                    auto new_clade = settings.clades.append();
                    new_clade->name = clade.first;
                    update_settings_clade(settings, clade, *new_clade);
                }
                else
                    update_settings_clade(settings, clade, **settings_clade);
            }
        }

        bool show_aa_at_pos() const override { return false; }
        std::vector<double> viewport_rel() const override { return {0, 0, 0}; }
        double rotate_degrees() const override { return 0; }
        std::optional<std::string> flip() const override { return {}; }

      protected:
        std::string lab() const { return lab_; }
        std::string virus_type() const { return virus_type_; }
        std::string assay() const { return assay_; }

        virtual void update_settings_clade(CladesDrawSettings& /*settings*/, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const
        {
            if (clade.first == "GLY" || clade.first == "NO-GLY" || clade.first == "159S" || clade.first == "159F" || clade.first == "159Y")
                settings_clade.show = false;
        }

      private:
        const std::string lab_, virus_type_, assay_;
    };

    // ----------------------------------------------------------------------

    class AAAtPos : public Default
    {
      public:
        using Default::Default;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            Default::update(settings, whocc_support);
            settings.time_series_width = 150;
            settings.top = 60;
            settings.bottom = 60;
            settings.left = 50;
            settings.right = 0;
            settings.clades_width = 50;
            settings.tree_margin_right = 10;
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            Default::update(tree_draw, whocc_support);
            tree_draw.settings().legend->width = 100;
        }

        bool show_aa_at_pos() const override { return true; }
    };

    // ----------------------------------------------------------------------

    class WithMap : public Default
    {
      public:
        using Default::Default;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            Default::update(settings, whocc_support);
            settings.top = 23;
            settings.bottom = 23;
            settings.left = 10;
            settings.clades_width = 20;
            settings.tree_margin_right = 10;
            settings.mapped_antigens_margin_right = 10;
            settings.time_series_width = 140;
        }

        void update(TitleDrawSettings& settings) const override
        {
            Default::update(settings);
            if (virus_type() == "A(H3N2)")
                settings.title = string::join(" ", {virus_type(), assay(), fixed_lab()});
            else
                settings.title = string::join(" ", {virus_type(), fixed_lab()});
            settings.offset = acmacs::Offset{10, 10};
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            Default::update(tree_draw, whocc_support);
            tree_draw.settings().legend->width = 100;
        }

        void update(CladesDrawSettings& settings, bool whocc_support) const override
        {
            Default::update(settings, whocc_support);
            settings.clades.for_each([](auto& clade, size_t /*no*/) { clade.label_offset = acmacs::Offset{1, 0}; });
        }

      protected:
        std::string fixed_lab() const
        {
            if (auto found = lab_name_normalizer_.find(lab()); found != lab_name_normalizer_.end())
                return found->second;
            else
                return lab();
        }

      private:
        const std::map<std::string, const char*> lab_name_normalizer_ = {{"MELB", "VIDRL"}, {"NIMR", "Crick"}};
    };

    class TreeOnly : public Default
    {
      public:
        using Default::Default;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            Default::update(settings, whocc_support);
            settings.top = 60;
            settings.bottom = 60;
            settings.left = 50;
            settings.right = 0;
            settings.tree_margin_right = 10;
            settings.time_series_width = 300;
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            Default::update(tree_draw, whocc_support);
            tree_draw.settings().legend->width = 180;
        }
    };

    // ----------------------------------------------------------------------

    class H1_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            TreeOnly::update(settings, whocc_support);
            if (whocc_support)
                settings.clades_width = 100;
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            TreeOnly::update(tree_draw, whocc_support);
            if (whocc_support) {
                // {
                //     auto mod = tree_draw.settings().mods.append();
                //     mod->mod = "hide-one";
                //     mod->s1 = "A%28H1N1%29/PERTH/9/2018__MDCK2";
                // }
                // {
                //     auto mod = tree_draw.settings().mods.append();
                //     mod->mod = "hide-one";
                //     mod->s1 = "A%28H1N1%29/PARANA/763/2018__OR";
                // }
                // {
                // auto mod = tree_draw.settings().mods.append();
                // mod->mod_help = "hide-if-cumulative-edge-length-bigger-than";
                // mod->d1 = 0.021;
                // }

                {
                    // most recent vaccine
                    auto mod2 = tree_draw.settings().mods.append();
                    mod2->mod = "mark-with-label";
                    mod2->name = "/BRISBANE/2/2018";
                    mod2->label = "A/Brisbane/2/2018";
                    mod2->label_color = "black";
                    mod2->label_size = 7.0;
                    mod2->label_style = "";
                    mod2->line_color = "grey";
                    mod2->line_width = 0.0;
                    mod2->collapse = 200;
                    mod2->label_absolute_x = 310.0;
                }

                // Code below does not work correctly, we should add
                // real hz sections with aa_transitions (current code
                // eventually adds replaced entries with node
                // references). In reality we need to figure out node
                // references upon ladderising.

                // const std::array transitions_for_sections{"T120A", "S183P", "N129D", "N451T"};
                // for (const auto& tr : transitions_for_sections)
                //     tree_draw.hz_sections().add(tr, true);
            }
        }

      protected:
        void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        {
            TreeOnly::update_settings_clade(settings, clade, settings_clade);
            if (clade.first == "6B") {
                clade.second.slot = settings_clade.slot = 4;
            }
        }
    };

    class H1_WithMap : public WithMap
    {
      public:
        using WithMap::WithMap;

        void update(TreeDraw& tree_draw, bool whocc_support) const override
            {
                WithMap::update(tree_draw, whocc_support);
                tree_draw.settings().legend->offset = acmacs::Offset{0, 950};
                tree_draw.settings().legend->width = 150;
            }

        // void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        // {
        //     WithMap::update(settings, whocc_support);
        //     // settings.time_series_width = 100;
        //     // settings.clades_width = 35;
        // }
    };

    class H1_HI_ALL : public H1_WithMap
    {
      public:
        using H1_WithMap::H1_WithMap;

        // std::vector<double> viewport_rel() const override { return {9, 9, -16}; }
    };

    class H1_HI_CDC : public H1_WithMap
    {
      public:
        using H1_WithMap::H1_WithMap;

        // std::vector<double> viewport_rel() const override { return {9, 9, -16}; }
    };

    class H1_HI_MELB : public H1_WithMap
    {
      public:
        using H1_WithMap::H1_WithMap;

        // std::vector<double> viewport_rel() const override { return {2, 2, -6}; }
    };

    class H1_HI_NIID : public H1_WithMap
    {
      public:
        using H1_WithMap::H1_WithMap;

        // std::vector<double> viewport_rel() const override { return {2, 3, -6}; }
    };

    class H1_HI_NIMR : public H1_WithMap
    {
      public:
        using H1_WithMap::H1_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -7}; }
    };

    // ----------------------------------------------------------------------

    class H3_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            TreeOnly::update(settings, whocc_support);
            if (whocc_support) {
                settings.time_series_width = 250;
                settings.clades_width = 160;
            }
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            TreeOnly::update(tree_draw, whocc_support);

            if (whocc_support) {
                auto mod1 = tree_draw.settings().mods.append();
                mod1->mod = "hide-if-cumulative-edge-length-bigger-than";
                mod1->d1 = 0.04;

                // most recent vaccine
                auto mod2 = tree_draw.settings().mods.append();
                mod2->mod = "mark-with-label";
                mod2->name = "/KANSAS/14/2017";
                mod2->label = "A/Kansas/14/2017";
                mod2->label_color = "black";
                mod2->label_size = 8.0;
                mod2->label_style = "";
                mod2->line_color = "grey";
                mod2->line_width = 0.0;
                mod2->collapse = 200;
                mod2->label_absolute_x = 310.0;

                tree_draw.hz_sections().sections.for_each([](auto& section, size_t /*no*/) {
                    if (section.triggering_clades.contains("first-leaf:first") || section.triggering_clades.contains("2A1:first") || section.triggering_clades.contains("2A1A:last"))
                        section.show_map = false;
                    else if (section.triggering_clades.contains("3A:first"))
                        section.label = "3a";
                    else if (section.triggering_clades.contains("2A4:first"))
                        section.label = "2a4";
                    else if (section.triggering_clades.contains("2A3:first"))
                        section.label = "2a3";
                    else if (section.triggering_clades.contains("2A1A:first"))
                        section.label = "2a1a";
                    else if (section.triggering_clades.contains("2A1B:first"))
                        section.label = "2a1b 135T";
                    else if (section.triggering_clades.contains("2A2:first"))
                        section.label = "2a2";
                });

                tree::iterate_pre(tree_draw.tree(), [&tree_draw](const Node& node) {
                    if (node.data.aa_transitions.size() == 1 && node.data.aa_transitions.contains("T135K") && node.data.number_strains > 200) {
                        auto section = tree_draw.hz_sections().add(find_first_leaf(node).seq_id, true, std::string{}, 0, true);
                        section->label = "2a1b 135K";
                        // std::cerr << "DEBUG: " << node.data.aa_transitions << ' ' << node.data.number_strains << DEBUG_LINE_FUNC << '\n';
                    }
                    else if (node.data.aa_transitions.contains("K135N") && node.data.number_strains > 100) {
                        auto section = tree_draw.hz_sections().add(find_first_leaf(node).seq_id, true, std::string{}, 0, true);
                        section->label = "2a1b 135N";
                    }
                });
            }
        }

      protected:
        void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        {
            TreeOnly::update_settings_clade(settings, clade, settings_clade);
            if (clade.first == "3C.3") {
                settings_clade.show = false;
            }
            else if (clade.first == "3A") {
                settings_clade.display_name = "3a";
                clade.second.slot = settings_clade.slot = 0;
            }
            else if (clade.first == "3B") {
                settings_clade.display_name = "3b";
                clade.second.slot = settings_clade.slot = 0;
            }
            else if (clade.first == "2A") {
                clade.second.slot = settings_clade.slot = 7;
                settings_clade.display_name = "2a";
            }
            else if (clade.first == "2A1") {
                clade.second.slot = settings_clade.slot = 4;
                settings_clade.display_name = "2a1";
            }
            else if (clade.first == "2A1A") {
                settings_clade.display_name = "2a1a";
                clade.second.slot = settings_clade.slot = 0;
            }
            else if (clade.first == "2A1B") {
                settings_clade.display_name = "2a1b";
                clade.second.slot = settings_clade.slot = 0;
            }
            else if (clade.first == "2A2") {
                settings_clade.display_name = "2a2";
                clade.second.slot = settings_clade.slot = 0;
            }
            else if (clade.first == "2A3") {
                settings_clade.display_name = "2a3";
            }
            else if (clade.first == "2A4") {
                settings_clade.display_name = "2a4";
            }
        }
    };

    class H3_WithMap : public WithMap
    {
      public:
        using WithMap::WithMap;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            WithMap::update(settings, whocc_support);
            if (whocc_support) {
                settings.time_series_width = 100;
                settings.clades_width = 35;
            }
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
            {
                WithMap::update(tree_draw, whocc_support);
                tree_draw.settings().legend->offset = acmacs::Offset{0, 950};
                tree_draw.settings().legend->width = 150;
            }

    };

    class H3_HI_CDC : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
    };

    class H3_HI_MELB : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        std::vector<double> viewport_rel() const override { return {4, 2, -5}; }
    };

    // class H3_HI_NIID : public H3_WithMap
    // {
    //   public:
    //     using H3_WithMap::H3_WithMap;

    //     // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    // };

    class H3_HI_NIMR : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        std::vector<double> viewport_rel() const override { return {2, 1, -4}; }
        double rotate_degrees() const override { return -70; }
    };

    class H3_NEUT_CDC : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
    };

    class H3_NEUT_MELB : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        std::vector<double> viewport_rel() const override { return {6, 4, -8}; }
    };

    class H3_NEUT_NIID : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    };

    class H3_NEUT_NIMR : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    };

    // ----------------------------------------------------------------------

    class BVic_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            TreeOnly::update(settings, whocc_support);
            if (whocc_support) {
                settings.clades_width = 70;
                settings.time_series_width = 350;
            }
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            constexpr double cumulative_threshold = 0.0191;
            TreeOnly::update(tree_draw, whocc_support);
            if (whocc_support) {
                {
                    auto mod = tree_draw.settings().mods.append();
                    mod->mod = "hide-if-cumulative-edge-length-bigger-than";
                    mod->d1 = cumulative_threshold;
                }
                {
                    auto mod = tree_draw.settings().mods.append();
                    mod->mod = "mark-clade-with-line";
                    mod->clade = "DEL2017";
                    mod->color = "#A0A0A0";
                    mod->line_width = 0.2;
                }
                {
                    auto mod = tree_draw.settings().mods.append();
                    mod->mod = "mark-clade-with-line";
                    mod->clade = "TRIPLEDEL2017";
                    mod->color = "#606060";
                    mod->line_width = 0.2;
                }
                {
                    auto mod = tree_draw.settings().mods.append();
                    mod->mod = "before2015-58P-or-146I-or-559I";
                    mod->help = "hides 1B";
                }

                {
                    // most recent vaccine
                    auto mod2 = tree_draw.settings().mods.append();
                    mod2->mod = "mark-with-label";
                    mod2->seq_id = "B/COLORADO/6/2017_MDCK1";
                    mod2->label = "A/Colorado/6/2017";
                    mod2->label_color = "black";
                    mod2->label_size = 7.0;
                    mod2->label_style = "";
                    mod2->line_color = "grey";
                    mod2->line_width = 0.0;
                    mod2->collapse = 200;
                    mod2->label_absolute_x = 310.0;
                }
            }

            bool del2017_shown = false;
            tree_draw.hz_sections().sections.for_each([&del2017_shown, &tree_draw](auto& section, size_t /*no*/) {
                if (section.triggering_clades.contains("DEL2017:first")) {
                    const auto* first_node = tree_draw.tree().find_leaf_by_seqid(section.name);
                    const bool branch_too_long = first_node && first_node->data.cumulative_edge_length > cumulative_threshold;
                    if (!del2017_shown && !branch_too_long) {
                        section.show_line = true;
                        section.label = "2-Del";
                        del2017_shown = true;
                    }
                    else {
                        section.comment_out();
                    }
                }
                else if (section.triggering_clades.contains("TRIPLEDEL2017:first")) {
                    section.show_line = true;
                    section.label = "3-Del";
                }
                else if (section.triggering_clades.contains("TRIPLEDEL2017:last"))
                    section.show_line = true;
                else if (section.triggering_clades.contains("DEL2017:last") || section.triggering_clades.contains("1:first") || section.triggering_clades.contains("1:last"))
                    section.comment_out();
            });
        }

      protected:
        void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        {
            TreeOnly::update_settings_clade(settings, clade, settings_clade);
            if (clade.first == "DEL2017" || clade.first == "TRIPLEDEL2017" || clade.first == "V1") {
                settings_clade.show = false;
            }
        }
    };

    class BVic_WithMap : public WithMap
    {
      public:
        using WithMap::WithMap;

        // void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        // {
        //     WithMap::update(settings, whocc_support);
        //     // settings.time_series_width = 100;
        //     // settings.clades_width = 35;
        // }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
            {
                WithMap::update(tree_draw, whocc_support);
                tree_draw.settings().legend->offset = acmacs::Offset{0, 950};
                tree_draw.settings().legend->width = 150;
            }

    };

    class BVic_HI_CDC : public BVic_WithMap
    {
      public:
        using BVic_WithMap::BVic_WithMap;

        // std::vector<double> viewport_rel() const override { return {2, 2, -4}; }
    };

    class BVic_HI_MELB : public BVic_WithMap
    {
      public:
        using BVic_WithMap::BVic_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 6.5, -7}; }
    };

    class BVic_HI_NIID : public BVic_WithMap
    {
      public:
        using BVic_WithMap::BVic_WithMap;

        // std::vector<double> viewport_rel() const override { return {3, 2, -3}; }
        // double rotate_degrees() const override { return -90; }
        // std::optional<std::string> flip() const override { return "ns"; }
    };

    class BVic_HI_NIMR : public BVic_WithMap
    {
      public:
        using BVic_WithMap::BVic_WithMap;

        // std::vector<double> viewport_rel() const override { return {5, 5, -7}; }
    };

    // ----------------------------------------------------------------------

    class BYam_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        {
            TreeOnly::update(settings, whocc_support);
            if (whocc_support) {
                settings.left = 70;
                settings.clades_width = 50;
            }
        }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
        {
            TreeOnly::update(tree_draw, whocc_support);
            if (whocc_support) {
                {
                    auto mod = tree_draw.settings().mods.append();
                    mod->mod = "hide-if-cumulative-edge-length-bigger-than";
                    mod->d1 = 0.043;
                }

                {
                    // most recent vaccine
                    auto mod2 = tree_draw.settings().mods.append();
                    mod2->mod = "mark-with-label";
                    mod2->name = "/PHUKET/3073/2013";
                    mod2->label = "A/Phuket/3073/2013";
                    mod2->label_color = "black";
                    mod2->label_size = 7.0;
                    mod2->label_style = "";
                    mod2->line_color = "grey";
                    mod2->line_width = 0.0;
                    mod2->collapse = 200;
                    mod2->label_absolute_x = 310.0;
                }
            }
        }

      protected:
        // void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        //     {
        //         TreeOnly::update_settings_clade(settings, clade, settings_clade);
        //     }
    };

    class BYam_WithMap : public WithMap
    {
      public:
        using WithMap::WithMap;

        // void update(SignaturePageDrawSettings& settings, bool whocc_support) const override
        // {
        //     WithMap::update(settings, whocc_support);
        //     // settings.time_series_width = 100;
        //     // settings.clades_width = 35;
        // }

        void update(TreeDraw& tree_draw, bool whocc_support) const override
            {
                WithMap::update(tree_draw, whocc_support);
                tree_draw.settings().legend->offset = acmacs::Offset{0, 950};
                tree_draw.settings().legend->width = 150;
            }

    };

    class BYam_HI_CDC : public BYam_WithMap
    {
      public:
        using BYam_WithMap::BYam_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
    };

    class BYam_HI_MELB : public BYam_WithMap
    {
      public:
        using BYam_WithMap::BYam_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    };

    class BYam_HI_NIID : public BYam_WithMap
    {
      public:
        using BYam_WithMap::BYam_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    };

    class BYam_HI_NIMR : public BYam_WithMap
    {
      public:
        using BYam_WithMap::BYam_WithMap;

        // std::vector<double> viewport_rel() const override { return {4, 4, -8}; }
    };

    // ----------------------------------------------------------------------

} // namespace

// ----------------------------------------------------------------------

struct settings_constructor_base
{
    virtual ~settings_constructor_base() = default;
    virtual std::unique_ptr<SettingsInitializer> make(std::string lab, std::string virus_type, std::string assay) const = 0;
};

template <typename T> struct settings_constructor : public settings_constructor_base
{
    std::unique_ptr<SettingsInitializer> make(std::string lab, std::string virus_type, std::string assay) const override { return std::make_unique<T>(lab, virus_type, assay); }
};

template <typename T> static inline std::unique_ptr<settings_constructor_base> maker() { return std::make_unique<settings_constructor<T>>(); }

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

static const std::array settings_constructors {
    std::pair{    " AH1N1 ",                 maker<H1_TreeOnly>()},
    std::pair{"CDC+MELB+NIID+NIMR AH1N1 HI", maker<H1_HI_ALL>()},
    std::pair{ "CDC AH1N1 HI",               maker<H1_HI_CDC>()},
    std::pair{"MELB AH1N1 HI",               maker<H1_HI_MELB>()},
    std::pair{"VIDRL AH1N1 HI",              maker<H1_HI_MELB>()},
    std::pair{"NIID AH1N1 HI",               maker<H1_HI_NIID>()},
    std::pair{"NIMR AH1N1 HI",               maker<H1_HI_NIMR>()},
    std::pair{"Crick AH1N1 HI",              maker<H1_HI_NIMR>()},

    std::pair{    " AH3N2 ",    maker<H3_TreeOnly>()},
    std::pair{ "CDC AH3N2 HI",  maker<H3_HI_CDC>()},
    std::pair{"MELB AH3N2 HI",  maker<H3_HI_MELB>()},
    std::pair{"VIDRL AH3N2 HI", maker<H3_HI_MELB>()},
      // std::pair{"NIID AH3N2 HI", maker<H3_HI_NIID>()},
    std::pair{"NIMR AH3N2 HI",  maker<H3_HI_NIMR>()},
    std::pair{"Crick AH3N2 HI", maker<H3_HI_NIMR>()},

    std::pair{ "CDC AH3N2 FOCUS REDUCTION",                  maker<H3_NEUT_CDC>()},
    std::pair{"MELB AH3N2 FOCUS REDUCTION",                  maker<H3_NEUT_MELB>()},
    std::pair{"VIDRL AH3N2 FOCUS REDUCTION",                 maker<H3_NEUT_MELB>()},
    std::pair{"NIID AH3N2 MN",                               maker<H3_NEUT_NIID>()},
    std::pair{"NIID AH3N2 FOCUS REDUCTION",                  maker<H3_NEUT_NIID>()},
    std::pair{"NIMR AH3N2 PLAQUE REDUCTION NEUTRALISATION",  maker<H3_NEUT_NIMR>()},
    std::pair{"Crick AH3N2 PLAQUE REDUCTION NEUTRALISATION", maker<H3_NEUT_NIMR>()},

    std::pair{    " B/Vic ",      maker<BVic_TreeOnly>()},
    std::pair{ "CDC B/Vic HI",    maker<BVic_HI_CDC>()},
    std::pair{"MELB B/Vic HI",    maker<BVic_HI_MELB>()},
    std::pair{"VIDRL B/Vic HI",   maker<BVic_HI_MELB>()},
    std::pair{"NIID B/Vic HI",    maker<BVic_HI_NIID>()},
    std::pair{"NIMR B/Vic HI",    maker<BVic_HI_NIMR>()},
    std::pair{"Crick B/Vic HI",   maker<BVic_HI_NIMR>()},

    std::pair{    " B/Yam ",      maker<BYam_TreeOnly>()},
    std::pair{ "CDC B/Yam HI",    maker<BYam_HI_CDC>()},
    std::pair{"MELB B/Yam HI",    maker<BYam_HI_MELB>()},
    std::pair{"VIDRL B/Yam HI",   maker<BYam_HI_MELB>()},
    std::pair{"NIID B/Yam HI",    maker<BYam_HI_NIID>()},
    std::pair{"NIMR B/Yam HI",    maker<BYam_HI_NIMR>()},
    std::pair{"Crick B/Yam HI",   maker<BYam_HI_NIMR>()},
};

#pragma GCC diagnostic pop

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos)
{
    if (lab.empty() && show_aa_at_pos)
        return std::make_unique<AAAtPos>(lab, virus_type, assay);
    const auto tag = string::concat(lab, ' ', virus_type, ' ', assay);
    std::cerr << ">>> settings initializer tag:\"" << tag << "\" lab:" << lab << " virus_type:" << virus_type << " assay:" << assay << "\n";
    if (auto found = std::find_if(settings_constructors.begin(), settings_constructors.end(), [&tag](const auto& entry) { return entry.first == tag; }); found != settings_constructors.end())
        return found->second->make(lab, virus_type, assay);
    std::cerr << "WARNING: No settings initializer for tag \"" << tag << "\"\n";
    return std::make_unique<Default>(lab, virus_type, assay);

} // settings_initilizer_factory

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
