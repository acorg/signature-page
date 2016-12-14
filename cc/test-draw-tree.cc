#include <iostream>
#include <string>
#include <memory>

#include "surface-cairo.hh"
#include "tree.hh"
#include "tree-export.hh"
#include "tree-draw.hh"

// ----------------------------------------------------------------------

void draw(Surface& aSurface, Tree& tree);

// ----------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int exit_code = 0;
    if (argc == 3) {
        try {
            Tree tree;
            tree_import(argv[1], tree);
            PdfCairo surface(argv[2], 500, 850);
            draw(surface, tree);
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
