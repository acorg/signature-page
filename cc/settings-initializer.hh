#pragma once

#include <memory>
#include <string>
#include <optional>

// ----------------------------------------------------------------------

class SignaturePageDrawSettings;
class TitleDrawSettings;
class CladesDrawSettings;
class CladeData;
class TreeDraw;

// ----------------------------------------------------------------------

class SettingsInitializer
{
 public:
    virtual ~SettingsInitializer() = default;

    virtual bool show_aa_at_pos() const = 0;

    virtual void update(SignaturePageDrawSettings& settings) const = 0;
    virtual void update(TitleDrawSettings& settings) const = 0;
    virtual void update(CladesDrawSettings& settings) const = 0;
    virtual void update(CladesDrawSettings& settings, std::pair<const std::string, CladeData>& clade) const = 0;
    virtual void update(TreeDraw& tree_draw) const = 0;

    virtual std::vector<double> viewport_rel() const = 0;
    virtual double rotate_degrees() const = 0;
    virtual std::optional<std::string> flip() const = 0;
};

std::unique_ptr<SettingsInitializer> settings_initilizer_factory(std::string lab, std::string virus_type, std::string assay, bool show_aa_at_pos);

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
