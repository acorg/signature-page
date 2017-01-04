#include "title-draw.hh"
#include "tree.hh"

// ----------------------------------------------------------------------

void TitleDraw::init_settings()
{
      // infer virus type from mTree and lab name from mChart
    std::string virus_type = mTree.virus_type();
    if (!virus_type.empty())
        mSettings.title = virus_type;

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
