#pragma once

// #include <string>
// #include <limits>
// #include <utility>
#include <memory>

#include "surface.hh"

// ----------------------------------------------------------------------

class Settings;
class Tree;
namespace seqdb { class Seqdb; }
class LocDb;
class TreeDraw;
class TimeSeriesDraw;

// ----------------------------------------------------------------------

class SignaturePageDrawSettings
{
 public:
    inline SignaturePageDrawSettings()
        {}

}; // class SignaturePageDrawSettings

// ----------------------------------------------------------------------

class SignaturePageDraw
{
 public:
    SignaturePageDraw(Surface& aSurface);
    ~SignaturePageDraw();

    void settings(std::string aFilename);
    void tree(std::string aTreeFilename, std::string aSeqdbFilename);

    void prepare();
    void draw();

 private:
    Surface& mSurface;
    std::unique_ptr<Settings> mSettings;
    std::unique_ptr<LocDb> mLocdb;
    std::unique_ptr<seqdb::Seqdb> mSeqdb;
    std::unique_ptr<Tree> mTree;
    std::unique_ptr<TreeDraw> mTreeDraw;
    std::unique_ptr<TimeSeriesDraw> mTimeSeriesDraw;

}; // class SignaturePageDraw

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
