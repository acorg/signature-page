#include <fstream>
#include <cstdlib>
#include <algorithm>

#include "surface-cairo.hh"

// ----------------------------------------------------------------------

void draw(Surface& aSurface, const std::vector<std::string>& fonts);
double show(Surface& aSurface, const Location& origin, double size, std::string font_name);
void get_fonts(std::vector<std::string>& fonts);

// ----------------------------------------------------------------------

int main(int /*argc*/, const char */*argv*/[])
{
    int exit_code = 0;
    try {
        std::vector<std::string> fonts;
        get_fonts(fonts);
          // std::cout << fonts.back() << std::endl;
          // fonts.erase(fonts.begin() + 418, fonts.end());
        PdfCairo surface("/tmp/cairo-fonts.pdf", 500, 850);
        draw(surface, fonts);
    }
    catch (std::exception& err) {
        std::cerr << err.what() << std::endl;
        exit_code = 1;
    }
    return exit_code;
}

// ----------------------------------------------------------------------

void draw(Surface& aSurface, const std::vector<std::string>& fonts)
{
    const size_t columns = 3;
    const size_t column_size = fonts.size() / columns + 1;
    const auto f_size = aSurface.text_size("W", 1);
    const double size = aSurface.size().height / (column_size * f_size.height * 1.15);
    std::cout << "fonts:" << fonts.size() << " size:" << size << std::endl;

    aSurface.background("white");
    Location loc;
    for (size_t column = 0, font_no = 0; column < columns; ++column) {
        for (; font_no < fonts.size() && font_no < (column + 1) * column_size; ++font_no) {
            loc.y += show(aSurface, loc, size, fonts[font_no]);
        }
        loc.y = 0;
        loc.x += aSurface.size().width / columns;
    }
}

// ----------------------------------------------------------------------

double show(Surface& aSurface, const Location& origin, double size, std::string font_name)
{
    // std::cout << font_name << std::endl;
    const double interline = 1.1;
    TextStyle style(font_name);
    auto text_size = aSurface.text_size("W", size, style);
    aSurface.text({origin.x, origin.y + text_size.height * interline}, font_name, "black", size, style);
    return text_size.height * interline;

} // show

// ----------------------------------------------------------------------

void get_fonts(std::vector<std::string>& fonts)
{
    std::vector<std::string> exclude = {"", "YuGothic Bold", "YuGothic Medium", "Zapfino"}; // fonts leading to cairo crash

    fonts.push_back("serif");
    fonts.push_back("sans-serif");
    fonts.push_back("monospace");

    if (std::system("fc-list :lang=en -f '%{?fullname{%{fullname[0]}}{%{family}}}\n' | sort -u >/tmp/cairo-fonts.txt"))
        throw std::runtime_error("Cannot obtain font list with fc-list command");
    std::ifstream input("/tmp/cairo-fonts.txt");
    while (input) {
        std::string line;
        getline(input, line);
        if (std::find(exclude.begin(), exclude.end(), line) == exclude.end()) {
            fonts.push_back(line);
        }
    }

} // get_fonts

// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
