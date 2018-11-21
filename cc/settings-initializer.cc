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

        void update(SignaturePageDrawSettings& settings) const override
        {
            settings.time_series_width = 100;
            settings.bottom = settings.top;
        }

        void update(TitleDrawSettings& settings) const override { settings.title = virus_type(); }
        void update(CladesDrawSettings& /*settings*/) const override {}
        void update(TreeDrawSettings& /*settings*/, HzSections& /*hz_sections*/) const override {}

        void update(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade) const override
        {
            auto matcher = [&](const auto& c) { return static_cast<std::string>(c.name) == clade.first; };
            if (auto settings_clade = settings.clades.find_if(matcher); !settings_clade) {
                auto new_clade = settings.clades.append();
                new_clade->name = clade.first;
                update_settings_clade(settings, clade, *new_clade);
            }
            else
                update_settings_clade(settings, clade, **settings_clade);
        }

        bool show_aa_at_pos() const override { return false; }
        std::vector<double> viewport_rel() const override { return {0, 0, 0}; }

      protected:
        std::string lab() const { return lab_; }
        std::string virus_type() const { return virus_type_; }
        std::string assay() const { return assay_; }

        virtual void update_settings_clade(CladesDrawSettings& /*settings*/, std::pair<const std::string, CladeData>& /*clade*/, CladeDrawSettings& /*settings_clade*/) const {}

      private:
        const std::string lab_, virus_type_, assay_;
    };

    // ----------------------------------------------------------------------

    class AAAtPos : public Default
    {
      public:
        using Default::Default;

        void update(SignaturePageDrawSettings& settings) const override
        {
            Default::update(settings);
            settings.time_series_width = 150;
            settings.top = 60;
            settings.bottom = 60;
            settings.left = 50;
            settings.right = 0;
            settings.clades_width = 50;
            settings.tree_margin_right = 10;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override
        {
            Default::update(settings, hz_sections);
            settings.legend->width = 100;
        }

        bool show_aa_at_pos() const override { return false; }
    };

    // ----------------------------------------------------------------------

    class WithMap : public Default
    {
      public:
        using Default::Default;

        void update(SignaturePageDrawSettings& settings) const override
        {
            Default::update(settings);
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
            settings.title = string::join(" ", {virus_type(), fixed_lab()});
            settings.offset = acmacs::Offset{10, 10};
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override { Default::update(settings, hz_sections); settings.legend->width = 100; }

        void update(CladesDrawSettings& settings) const override
        {
            Default::update(settings);
            settings.clades.for_each([](auto& clade) { clade.label_offset = acmacs::Offset{1, 0}; });
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

        void update(SignaturePageDrawSettings& settings) const override
        {
            Default::update(settings);
            settings.top = 60;
            settings.bottom = 60;
            settings.left = 50;
            settings.right = 0;
            settings.tree_margin_right = 10;
            settings.time_series_width = 300;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override { Default::update(settings, hz_sections); settings.legend->width = 180; }
    };

    // ----------------------------------------------------------------------

    class H1_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.clades_width = 100;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override
        {
            TreeOnly::update(settings, hz_sections);
            auto mod = settings.mods.append();
            mod->mod = "hide-if-cumulative-edge-length-bigger-than";
            mod->d1 = 0.021;
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

    // ----------------------------------------------------------------------

    class H3_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.time_series_width = 250;
            settings.clades_width = 160;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override
        {
            TreeOnly::update(settings, hz_sections);

            auto mod = settings.mods.append();
            mod->mod = "hide-if-cumulative-edge-length-bigger-than";
            mod->d1 = 0.04;

            // if (auto found = hz_sections.sections.find_if([](const auto& section) {section.triggering_aa_pos}); found) {
            // }
        }

      protected:
        void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        {
            TreeOnly::update_settings_clade(settings, clade, settings_clade);
            if (clade.first == "GLY" || clade.first == "NO-GLY") {
                settings_clade.show = false;
            }
            else if (clade.first == "3C.3") {
                settings_clade.show = false;
            }
            else if (clade.first == "3C.3A") {
                settings_clade.display_name = "3C.3a";
            }
            else if (clade.first == "3C.3B") {
                settings_clade.display_name = "3C.3b";
            }
            else if (clade.first == "3C.2A") {
                clade.second.slot = settings_clade.slot = 7;
                settings_clade.display_name = "3C.2a";
            }
            else if (clade.first == "2A1") {
                clade.second.slot = settings_clade.slot = 4;
                settings_clade.display_name = "2a1";
            }
            else if (clade.first == "2A1A") {
                settings_clade.display_name = "2a1a";
            }
            else if (clade.first == "2A1B") {
                settings_clade.display_name = "2a1b";
            }
            else if (clade.first == "2A2") {
                settings_clade.display_name = "2a2";
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

        void update(SignaturePageDrawSettings& settings) const override
        {
            WithMap::update(settings);
            settings.time_series_width = 100;
            settings.clades_width = 35;
        }
    };

    class H3_HI_CDC : public H3_WithMap
    {
      public:
        using H3_WithMap::H3_WithMap;

        void update(SignaturePageDrawSettings& settings) const override { H3_WithMap::update(settings); }

        std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
    };

    // ----------------------------------------------------------------------

    class BVic_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.clades_width = 50;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override
        {
            TreeOnly::update(settings, hz_sections);
            {
                auto mod = settings.mods.append();
                mod->mod = "hide-if-cumulative-edge-length-bigger-than";
                mod->d1 = 0.0191;
            }
            {
                auto mod = settings.mods.append();
                mod->mod = "mark-clade-with-line";
                mod->clade = "DEL2017";
                mod->color = "#A0A0A0";
                mod->line_width = 0.2;
            }
            {
                auto mod = settings.mods.append();
                mod->mod = "mark-clade-with-line";
                mod->clade = "TRIPLEDEL2017";
                mod->color = "#606060";
                mod->line_width = 0.2;
            }
            {
                auto mod = settings.mods.append();
                mod->mod = "before2015-58P-or-146I-or-559I";
                mod->help = "hides 1B";
            }
        }

      protected:
        void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        {
            TreeOnly::update_settings_clade(settings, clade, settings_clade);
            if (clade.first == "DEL2017" || clade.first == "TRIPLEDEL2017") {
                settings_clade.show = false;
            }
        }
    };

    // ----------------------------------------------------------------------

    class BYam_TreeOnly : public TreeOnly
    {
      public:
        using TreeOnly::TreeOnly;

        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.left = 70;
            settings.clades_width = 50;
        }

        void update(TreeDrawSettings& settings, HzSections& hz_sections) const override
        {
            TreeOnly::update(settings, hz_sections);
            auto mod = settings.mods.append();
            mod->mod = "hide-if-cumulative-edge-length-bigger-than";
            mod->d1 = 0.043;
        }

      protected:
        // void update_settings_clade(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade, CladeDrawSettings& settings_clade) const override
        //     {
        //         TreeOnly::update_settings_clade(settings, clade, settings_clade);
        //     }
    };

    // ----------------------------------------------------------------------

} // namespace

// ----------------------------------------------------------------------

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos)
{
    if (lab.empty() && show_aa_at_pos)
        return std::make_unique<AAAtPos>(lab, virus_type, assay);
    else if (const auto tag = string::concat(lab, ' ', virus_type, ' ', assay); tag == "CDC A(H3N2) HI")
        return std::make_unique<H3_HI_CDC>(lab, virus_type, assay);
    else if (tag == " A(H3N2) ")
        return std::make_unique<H3_TreeOnly>(lab, virus_type, assay);
    else if (tag == " A(H1N1) ")
        return std::make_unique<H1_TreeOnly>(lab, virus_type, assay);
    else if (tag == " B/Vic ")
        return std::make_unique<BVic_TreeOnly>(lab, virus_type, assay);
    else if (tag == " B/Yam ")
        return std::make_unique<BYam_TreeOnly>(lab, virus_type, assay);
    else
        return std::make_unique<Default>(lab, virus_type, assay);

} // settings_initilizer_factory

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
