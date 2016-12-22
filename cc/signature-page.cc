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
    const double offset = mSurface.size().width * mSettings->signature_page.border_space;
    Surface& tree_draw_surface = mSurface.subsurface({offset, offset}, mSurface.size() - Size(offset * 2, offset * 2), mSurface.size().width, false);
    mTreeDraw = std::unique_ptr<TreeDraw>{new TreeDraw{tree_draw_surface, *mTree, mSettings->tree_draw}};
      // mTreeDraw = std::unique_ptr<TreeDraw>{new TreeDraw{mSurface, *mTree, mSettings->tree_draw}};

    mTreeDraw->prepare();
      // mTimeSeriesDraw->prepare();

} // SignaturePageDraw::prepare

// ----------------------------------------------------------------------

void SignaturePageDraw::draw()
{
    mSurface.background("white");
    mSurface.border("green3", 10);
    mTreeDraw->draw();
      // mTimeSeriesDraw->draw();

} // SignaturePageDraw::draw

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
