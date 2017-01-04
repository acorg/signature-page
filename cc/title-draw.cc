#include "title-draw.hh"
#include "tree.hh"
#include "chart.hh"

// ----------------------------------------------------------------------

void TitleDraw::init_settings()
{
    std::string virus_type = mTree.virus_type();
    if (!virus_type.empty())
        mSettings.title = virus_type;

    if (mChart) {
        if (!mSettings.title.empty())
            mSettings.title += " ";
        mSettings.title += mChart->chart_info().lab;
        mSettings.size = 10;
        mSettings.offset.set(5, 5 + mSettings.size);
    }
    else {
        mSettings.size = 20;
        mSettings.offset.set(10, 10 + mSettings.size);
    }

} // TitleDraw::init_settings

// ----------------------------------------------------------------------

void TitleDraw::prepare()
{

} // TitleDraw::prepare

// ----------------------------------------------------------------------

void TitleDraw::draw()
{
    mSurface.text(mSettings.offset, mSettings.title, mSettings.color, mSettings.size, mSettings.style);

} // TitleDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
