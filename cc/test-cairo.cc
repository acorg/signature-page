#include <iostream>
#include <string>
#include <memory>

#include "surface-cairo.hh"

// ----------------------------------------------------------------------

void draw(Surface& aSurface);

// ----------------------------------------------------------------------

int main(int /*argc*/, const char */*argv*/[])
{
    int exit_code = 0;
    try {
        PdfCairo surface("/tmp/tc.pdf", 500, 850);
        draw(surface);
        std::unique_ptr<Surface> sub1{surface.subsurface({20, 500}, surface.size(), 0.3, true)};
        draw(*sub1);
        std::unique_ptr<Surface> sub2{surface.subsurface({270, 550}, surface.size(), 0.2, false)};
        draw(*sub2);
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------

void draw(Surface& aSurface)
{
    aSurface.background(0x40F0F0FF);
    aSurface.grid(200, "grey80", 5);
    aSurface.border("grey50", 20);
    aSurface.circle({500, 100}, 200, 1.0, 0.0, "black", 10);
    aSurface.circle({1000, 0}, 200, 1.0, 0.0, "black", 10);
    aSurface.circle({1000, 1700}, 200, 1.0, 0.0, "black", 10);
    aSurface.line({100, 100}, {300, 300}, "red", 10);
    aSurface.rectangle({100, 170}, {50, 70}, "orange", 5);
    aSurface.rectangle_filled({120, 200}, {250, 70}, "brown", 5, "#8080ffff");
    aSurface.circle({150, 170}, 100, 0.5, M_PI / 4.0, "violet", 5);
    aSurface.circle_filled({200, 220}, 100, 2, 0, 0x80FFA0FF, 5, 0x80A0FFA0);
    aSurface.square_filled({300, 420}, 100, 1.1, 0.1, 0x80A0A0FF, 5, 0x80C0FFA0);
    aSurface.triangle_filled({100, 420}, 100, 1.0, 0.0, 0x80FFA080, 5, 0x80FFFFA0);
    aSurface.double_arrow({100, 420}, {300, 550}, "blue", 2, 10);
    aSurface.double_arrow({300, 420}, {100, 550}, "blue4", 2, 10);

    aSurface.text({50, 600}, "Vozyaku", "green4", 100);
    auto text_size = aSurface.text_size("Vozyak", 100);
    aSurface.text({50 + text_size.width, 600}, "uVozyaku", "green3", 100);
    aSurface.text({500, 750}, "Vozyaku", "green4", 100, TextStyle("Arial", TextStyle::Slant::Italic, TextStyle::Weight::Bold), 0.5);
    aSurface.text({50, 800}, "Va", "red2", 100);
    aSurface.text({50, 800 - text_size.height}, "Va", "red2", 100);
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
