#include "title-draw.hh"
#include "tree.hh"
#include "chart-draw.hh"

// ----------------------------------------------------------------------

#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
#endif

static const std::map<std::string, const char*> lab_name_normalizer = {{"MELB", "VIDRL"}, {"NIMR", "Crick"}};

#pragma GCC diagnostic pop

void TitleDraw::init_settings()
{
    std::string title = mTree.virus_type();
    if (mChart) {
        std::string lab = mChart->lab();
        if (auto found = lab_name_normalizer.find(lab); found != lab_name_normalizer.end())
            lab = found->second;
        if (!lab.empty()) {
            if (!title.empty())
                title += " ";
            title += lab;
        }
        mSettings.offset = acmacs::Offset{10, 10};
    }
    else {
        // mSettings.offset.set(10, 10 + mSettings.size);
    }
    if (!title.empty())
        mSettings.title = title;

} // TitleDraw::init_settings

// ----------------------------------------------------------------------

void TitleDraw::prepare()
{

} // TitleDraw::prepare

// ----------------------------------------------------------------------

void TitleDraw::draw()
{
    mSurface.text(mSettings.offset, mSettings.title, mSettings.color, Pixels{mSettings.size}, mSettings.style);

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
