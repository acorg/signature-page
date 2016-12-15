#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>

#include "locationdb/locdb.hh"
#include "surface-cairo.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void read(Tree& tree, std::string aFilename);
void draw(Surface& aSurface, Tree& tree);

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int exit_code = 0;
    if (argc == 3) {
        try {
            Tree tree;
            read(tree, argv[1]);
            PdfCairo surface(argv[2], 500, 850);
            surface.background("white");
              // surface.rectangle({50, 50}, {50, 50}, "black", 5);
            const double offset = 50;
            const double scale = (surface.width() - offset * 2) / surface.width();
            std::cout << "Sub scale:" << scale << std::endl;
            std::unique_ptr<Surface> sub{surface.subsurface({offset, offset / surface.aspect()}, surface.size(), scale, false)};
            sub->border(0xA0FFA000, 10);
            draw(*sub, tree);
        }
        catch (std::exception& err) {
            std::cerr << err.what() << std::endl;
            exit_code = 1;
        }
    }
    else {
        std::cerr << "Usage: " << argv[0] << " <tree.json> <output.pdf>" << std::endl;
        exit_code = 2;
    }
    return exit_code;
}

// ----------------------------------------------------------------------

void read(Tree& tree, std::string aFilename)
{
    tree_import(aFilename, tree);
    LocDb locdb;
    locdb.importFrom(std::getenv("ACMACSD_ROOT") + std::string("/data/locationdb.json.xz"));
    tree.set_continents(locdb);

} // read

// ----------------------------------------------------------------------

void draw(Surface& aSurface, Tree& tree)
{
    aSurface.background("white");
    TreeDrawSettings settings{};
    TreeDraw tree_draw{aSurface, tree, settings};
    tree_draw.prepare();
    tree_draw.draw();
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
