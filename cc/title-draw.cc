#include "title-draw.hh"

// ----------------------------------------------------------------------

void TitleDraw::init_settings()
{
      // infer virus type from mTree and lab name from mChart

} // TitleDraw::init_settings

// ----------------------------------------------------------------------

void TitleDraw::prepare()
{

} // TitleDraw::prepare

// ----------------------------------------------------------------------

void TitleDraw::draw()
{
    mSurface.text({0, 30}, "TITLE", "brown", 30);

} // TitleDraw::draw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
