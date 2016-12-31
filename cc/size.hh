#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "acmacs-base/float.hh"

// ----------------------------------------------------------------------

class Size;

class Location
{
 public:
    double x, y;

    inline Location() : x(0), y(0) {}
    inline Location(double aX, double aY) : x(aX), y(aY) {}
    Location(const Size& s);

    inline Location& operator -= (const Location& a) { x -= a.x; y -= a.y; return *this; }
    inline Location& operator += (const Location& a) { x += a.x; y += a.y; return *this; }
    Location& operator += (const Size& a);
    inline std::string to_string() const { return "Location(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

    inline void min(const Location& a) { x = std::min(x, a.x); y = std::min(y, a.y); }
    inline void max(const Location& a) { x = std::max(x, a.x); y = std::max(y, a.y); }
    static inline Location center_of(const Location& a, const Location& b) { return {(a.x + b.x) / 2.0, (a.y + b.y) / 2.0}; }

    inline bool isnan() const { return std::isnan(x) || std::isnan(y); }
    inline std::vector<double> to_vector() const { return {x, y}; }
    inline void from_vector(const std::vector<double>& source) { x = source[0]; y = source[1]; }

}; // class Location

inline std::ostream& operator<<(std::ostream& out, const Location& loc)
{
    return out << '{' << loc.x << ", " << loc.y << '}';
}

// ----------------------------------------------------------------------

class Size
{
 public:
    double width, height;

    inline Size() : width(0), height(0) {}
    inline Size(double aWidth, double aHeight) : width(aWidth), height(aHeight) {}
    inline Size(const Location& a, const Location& b) : width(std::abs(a.x - b.x)), height(std::abs(a.y - b.y)) {}
    inline void set(double aWidth, double aHeight) { width = aWidth; height = aHeight; }
    inline double aspect() const { return width / height; }
    inline bool empty() const { return float_zero(width) && float_zero(height); }

    inline std::string to_string() const { return "Size(" + std::to_string(width) + ", " + std::to_string(height) + ")"; }

    Size& operator += (const Size& sz) { width += sz.width; height += sz.height; return *this; }

}; // class Size

inline std::ostream& operator<<(std::ostream& out, const Size& size)
{
    return out << '{' << size.width << ", " << size.height << '}';
}

// ----------------------------------------------------------------------

inline Location::Location(const Size& s)
    : x(s.width), y(s.height)
{
}

inline Location& Location::operator += (const Size& a)
{
    x += a.width;
    y += a.height;
    return *this;
}

inline Location operator + (const Location& a, const Size& s)
{
    return {a.x + s.width, a.y + s.height};
}

inline Location operator + (const Location& a, const Location& b)
{
    return {a.x + b.x, a.y + b.y};
}

inline Location operator - (const Location& a, const Size& s)
{
    return {a.x - s.width, a.y - s.height};
}

double inline distance(const Location& a, const Location& b)
{
    const double dx = a.x - b.x, dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

inline Size operator - (const Location& a, const Location& b)
{
    return {a.x - b.x, a.y - b.y};
}

inline Size operator - (const Size& a, const Location& b)
{
    return {a.width - b.x, a.height - b.y};
}

inline Size operator - (const Size& a, const Size& b)
{
    return {a.width - b.width, a.height - b.height};
}

inline Size operator + (const Size& a, const Size& b)
{
    return {a.width + b.width, a.height + b.height};
}

inline Size operator * (const Size& a, double v)
{
    return {a.width * v, a.height * v};
}

inline Size operator / (const Size& a, double v)
{
    return {a.width / v, a.height / v};
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
