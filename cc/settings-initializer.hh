#pragma once

#include <memory>
#include <string>

// ----------------------------------------------------------------------

class SettingsInitializer
{
 public:
    virtual ~SettingsInitializer() = default;

    virtual std::vector<double> viewport_rel() const { return {0, 0, 0}; }
    virtual double time_series_width() const { return 100; }
};

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
