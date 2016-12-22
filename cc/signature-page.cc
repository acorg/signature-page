#include <iostream>
#include <cstdlib>

#include "signature-page.hh"
#include "locationdb/locdb.hh"
#include "seqdb/seqdb.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"
#include "time-series-draw.hh"
#include "settings.hh"

// ----------------------------------------------------------------------

SignaturePageDraw::SignaturePageDraw(Surface& aSurface)
    : mSurface{aSurface}, mSettings{new Settings{}}, mLocdb{new LocDb{}}, mSeqdb{new seqdb::Seqdb{}},
      mTree{new Tree{}} //, mTreeDraw{new TreeDraw{}}, mTimeSeriesDraw{new TimeSeriesDraw{}}
{
    mLocdb->importFrom(std::getenv("ACMACSD_ROOT") + std::string("/data/locationdb.json.xz"));

} // SignaturePageDraw::SignaturePageDraw

// ----------------------------------------------------------------------

// must be declared here to allow proper destruction of unique_ptr
SignaturePageDraw::~SignaturePageDraw()
{
}

// ----------------------------------------------------------------------

void SignaturePageDraw::settings(std::string aFilename)
{
    if (!aFilename.empty())
        read_settings(*mSettings, aFilename);

} // SignaturePageDraw::settings

// ----------------------------------------------------------------------

void SignaturePageDraw::tree(std::string aTreeFilename, std::string aSeqdbFilename)
{
    tree_import(aTreeFilename, *mTree);
    mTree->set_continents(*mLocdb);
    if (!aSeqdbFilename.empty()) {
        mSeqdb->load(aSeqdbFilename);
        mTree->match_seqdb(*mSeqdb);
    }
      // tree.report_cumulative_edge_length(std::cout);
    mTree->ladderize(Tree::LadderizeMethod::NumberOfLeaves);           // must be before clade_setup
    mTree->make_aa_transitions();

    mTree->add_vaccine("A/SOUTH%20AUCKLAND/17/2016__MDCK%3F/SIAT1", "A/SOUTH AUCKLAND/17/2016 C");

} // SignaturePageDraw::tree

// ----------------------------------------------------------------------

void SignaturePageDraw::prepare()
{
    const Size page_size = mSurface.size();
    if (mSettings->signature_page.layout == SignaturePageDrawSettings::Layout::TreeTSClades) {
        const double section_height = page_size.height - (mSettings->signature_page.top + mSettings->signature_page.bottom);

        const double clades_width = mSettings->signature_page.clades_width;
        const double ts_width = mSettings->signature_page.time_series_width;
        const double tree_width = page_size.width - (mSettings->signature_page.left + mSettings->signature_page.tree_margin_right + ts_width + clades_width + mSettings->signature_page.right);

        const double ts_left = mSettings->signature_page.left + tree_width + mSettings->signature_page.tree_margin_right;
        const double clades_left = ts_left + ts_width;

        Surface& tree_draw_surface = mSurface.subsurface({mSettings->signature_page.left, mSettings->signature_page.top}, {tree_width, section_height}, page_size.width, false);
        mTreeDraw = std::unique_ptr<TreeDraw>{new TreeDraw{tree_draw_surface, *mTree, mSettings->tree_draw}};


        Surface& ts_surface = mSurface.subsurface({ts_left, mSettings->signature_page.top}, {ts_width, section_height}, page_size.width * ts_width / tree_width, false);
        mTimeSeriesDraw = std::make_unique<TimeSeriesDraw>(ts_surface, *mTree, *mTreeDraw, mSettings->time_series);

        Surface& clades_surface = mSurface.subsurface({clades_left, mSettings->signature_page.top}, {clades_width, section_height}, page_size.width * clades_width / tree_width, false);
        mCladesDraw = std::make_unique<CladesDraw>(clades_surface, *mTree, *mTreeDraw, mSettings->clades);
    }
    else {
        throw std::runtime_error("layout not implemented");
    }

    if (mTreeDraw)
        mTreeDraw->prepare();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->prepare();
    if (mCladesDraw)
        mCladesDraw->prepare();

} // SignaturePageDraw::prepare

// ----------------------------------------------------------------------

void SignaturePageDraw::draw()
{
    mSurface.background("white");

    if (mTreeDraw)
        mTreeDraw->draw();
    if (mTimeSeriesDraw)
        mTimeSeriesDraw->draw();
    if (mCladesDraw)
        mCladesDraw->draw();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
