#include "acmacs-base/string.hh"
#include "settings-initializer.hh"

// ----------------------------------------------------------------------

namespace
{
    class Default : public SettingsInitializer
    {
    };

    class AAAtPos : public SettingsInitializer
    {
     public:
        double time_series_width() const override { return 150; }
    };

      // ----------------------------------------------------------------------

    class WithMap : public Default
    {
     public:
        double time_series_width() const override { return 140; }
    };

    class TreeOnly : public Default
    {
     public:
        double time_series_width() const override { return 300; }
    };

      // ----------------------------------------------------------------------

    class H3 : public TreeOnly
    {
     public:
        double time_series_width() const override { return 250; }
    };

    class H3_HI_CDC : public WithMap
    {
     public:
        std::vector<double> viewport_rel() const override { return {4, 5, -7}; }
        double time_series_width() const override { return 100; }

    };

      // ----------------------------------------------------------------------

}

// ----------------------------------------------------------------------

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos)
{
    if (lab.empty() && show_aa_at_pos)
        return std::make_unique<AAAtPos>();
    else if (const auto tag = string::concat(lab, ' ', virus_type, ' ', assay); tag == "CDC A(H3N2) HI")
        return std::make_unique<H3_HI_CDC>();
    else if (tag == " A(H3N2) ")
        return std::make_unique<H3>();
    else
        return std::make_unique<Default>();

} // settings_initilizer_factory

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
