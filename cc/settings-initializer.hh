#pragma once

#include <memory>
#include <string>

// ----------------------------------------------------------------------

class SignaturePageDrawSettings;

class SettingsInitializer
{
 public:
    virtual ~SettingsInitializer() = default;

    virtual void update(SignaturePageDrawSettings& settings) const = 0;
    virtual bool show_aa_at_pos() const = 0;
    virtual std::vector<double> viewport_rel() const = 0;
};

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
