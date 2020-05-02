#include "title-draw.hh"
#include "tree.hh"
#include "chart-draw.hh"
#include "settings-initializer.hh"

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

static const std::map<std::string, const char*> lab_name_normalizer = {{"MELB", "VIDRL"}, {"NIMR", "Crick"}};

#pragma GCC diagnostic pop

void TitleDraw::init_settings(const SettingsInitializer& settings_initilizer)
{
    settings_initilizer.update(mSettings);

} // TitleDraw::init_settings

// ----------------------------------------------------------------------

void TitleDraw::prepare()
{

} // TitleDraw::prepare

// ----------------------------------------------------------------------

void TitleDraw::draw()
{
    mSurface.text(mSettings.offset, *mSettings.title, mSettings.color, Pixels{*mSettings.size}, mSettings.style);

} // TitleDraw::draw

// ----------------------------------------------------------------------

void TitleDrawSettings::remove_for_tree_settings()
{
    style.remove();

} // TitleDrawSettings::remove_for_tree_settings

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
