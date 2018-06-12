#include "title-draw.hh"
#include "tree.hh"
#include "chart-draw.hh"

// ----------------------------------------------------------------------

void TitleDraw::init_settings()
{
    std::string title = mTree.virus_type();
    if (mChart) {
        if (!title.empty())
            title += " ";
        title += mChart->lab();
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
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
