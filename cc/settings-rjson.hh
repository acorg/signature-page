#pragma once

#include "acmacs-base/rjson.hh"
#include "acmacs-base/color.hh"
#include "acmacs-draw/size.hh"

// ----------------------------------------------------------------------

namespace rjson
{
    namespace implementation
    {
        template <> struct content_type<Size> { using type = rjson::array; };
        template <> struct content_type<Color> { using type = rjson::string; };

        inline Size to_size(const value& aValue)
        {
            // try {
                const auto& ar = std::get<array>(aValue);
                return {std::get<number>(ar[0]), std::get<number>(ar[1])};
            // }
            // catch (std::exception& /*err*/) {
            //     std::cerr << "ERROR: cannot convert json to Size: " << aValue << '\n';
            //     return {};
            // }
        }

        template <> inline value to_value<Size>(const Size& aSize)
        {
            array result;
            result.insert(number{aSize.width});
            result.insert(number{aSize.height});
            return result;
        }

        // template <> inline value to_value<Color>(const Color& aColor)
        // {
        //     return string{aColor};
        // }

    } // namespace implementation

      // ----------------------------------------------------------------------

    template <> inline field_get_set<Size>::operator Size() const
    {
        try {
            return implementation::to_size(get_ref());
        }
        catch (std::exception& /*err*/) {
            std::cerr << "ERROR: cannot convert json to Size: " << get_ref() << '\n';
            return {};
        }
    }

    // template <> inline void value::set_field(std::string aFieldName, Size&& aSize)
    // {
    //     std::get<object>(*this).set_field(aFieldName, implementation::to_value(std::forward<Size>(aSize)));
    // }

      // ----------------------------------------------------------------------

    template <> inline field_get_set<Color>::operator Color() const
    {
        try {
            return static_cast<std::string>(get_value_ref());
        }
        catch (std::exception& /*err*/) {
            std::cerr << "ERROR: cannot convert json to Color: " << get_ref() << '\n';
            return {};
        }
    }

} // namespace rjson

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
