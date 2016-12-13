#include <iostream>
#include <string>

#include "surface-cairo.hh"

// ----------------------------------------------------------------------

int main(int /*argc*/, const char */*argv*/[])
{
    int exit_code = 0;
    try {
        PdfCairo surface("/tmp/tc.pdf", 500, 850);

        surface.line({100, 100}, {300, 300}, "red", 10);
        surface.rectangle({100, 170}, {50, 70}, "orange", 5);
        surface.rectangle_filled({120, 200}, {250, 70}, "brown", 5, "#8080ffff");
        surface.circle({150, 170}, 100, 0.5, M_PI / 4.0, "violet", 5);
        surface.circle_filled({200, 220}, 100, 2, 0, 0x80FFA0FF, 5, 0x80A0FFA0);

          //   //surface.line(Location(100, 100), Location(200, 100), 0xFFA500, 2);
          // TextStyle style;
          // double tsize = 10;
          // Size sz = surface.text_size("Jopa", tsize, style);
          // surface.text(Location(200, 100 + sz.height / 2), "Jopa", 0x00FF00, tsize, style);

          // cairo_set_source_rgb(mContext, 0, 1, 0);
          // // cairo_move_to(mContext, 0, 0);
          // // cairo_line_to(mContext, 1, 1);
          // // cairo_stroke(mContext);
          // cairo_move_to(mContext, 100, 200);
          // cairo_set_font_size(mContext, 30);
          // cairo_show_text(mContext, "JOPA");

          // cairo_line_to(mContext, 200, 300);
          // cairo_stroke(mContext);
          // cairo_move_to(mContext, 200, 300);
          // cairo_show_text(mContext, "APOJ");

          // cairo_destroy(mContext);

    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------
