#include "acmacs-base/string.hh"
#include "settings-initializer.hh"
#include "signature-page.hh"

// ----------------------------------------------------------------------

namespace
{
    class Default : public SettingsInitializer
    {
      public:
        void update(SignaturePageDrawSettings& settings) const override { settings.time_series_width = 100; }

        bool show_aa_at_pos() const override { return false; }
        std::vector<double> viewport_rel() const override { return {0, 0, 0}; }
    };

    // ----------------------------------------------------------------------

    class AAAtPos : public Default
    {
      public:
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

        bool show_aa_at_pos() const override { return false; }
    };

    // ----------------------------------------------------------------------

    class WithMap : public Default
    {
      public:
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
    };

    class TreeOnly : public Default
    {
      public:
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
    };

    // ----------------------------------------------------------------------

    class H3_TreeOnly : public TreeOnly
    {
      public:
        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.time_series_width = 250;
            settings.clades_width = 160;
        }
    };

    class H3_WithMap : public WithMap
    {
      public:
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
        void update(SignaturePageDrawSettings& settings) const override { H3_WithMap::update(settings); }

        std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
    };

    // ----------------------------------------------------------------------

    class H1_TreeOnly : public TreeOnly
    {
      public:
        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.clades_width = 100;
        }
    };

    // ----------------------------------------------------------------------

    class BVic_TreeOnly : public TreeOnly
    {
      public:
        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.clades_width = 50;
        }
    };

    // ----------------------------------------------------------------------

    class BYam_TreeOnly : public TreeOnly
    {
      public:
        void update(SignaturePageDrawSettings& settings) const override
        {
            TreeOnly::update(settings);
            settings.left = 70;
            settings.clades_width = 50;
        }
    };

    // ----------------------------------------------------------------------

} // namespace

// ----------------------------------------------------------------------

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos)
{
    if (lab.empty() && show_aa_at_pos)
        return std::make_unique<AAAtPos>();
    else if (const auto tag = string::concat(lab, ' ', virus_type, ' ', assay); tag == "CDC A(H3N2) HI")
        return std::make_unique<H3_HI_CDC>();
    else if (tag == " A(H3N2) ")
        return std::make_unique<H3_TreeOnly>();
    else if (tag == " A(H1N1) ")
        return std::make_unique<H1_TreeOnly>();
    else if (tag == " B/Vic ")
        return std::make_unique<BVic_TreeOnly>();
    else if (tag == " B/Yam ")
        return std::make_unique<BYam_TreeOnly>();
    else
        return std::make_unique<Default>();

} // settings_initilizer_factory

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
