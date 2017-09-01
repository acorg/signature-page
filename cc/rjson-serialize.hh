#pragma once

#include "acmacs-base/rjson.hh"

#include "acmacs-base/color.hh"
#include "acmacs-draw/size.hh"

// ----------------------------------------------------------------------

namespace rjson
{
    class field_container
    {
     public:
        inline void use_json(value& aData) { mData = &aData; }
        inline operator value&() { assert(mData); return *mData; }
        inline operator const value&() const { assert(mData); return *mData; }
        inline std::string to_json() const { return static_cast<const value&>(*this).to_json(); }

     private:
        mutable value* mData = nullptr;
    };

      // ----------------------------------------------------------------------

    template <typename FValue> class field_get_set
    {
     public:
        inline field_get_set(field_container* aContainer, std::string aFieldName, FValue&& aDefault) : mContainer{*aContainer}, mFieldName{aFieldName}, mDefault{std::move(aDefault)} {}
        inline operator FValue() const { return static_cast<value&>(mContainer).get_field(mFieldName, mDefault); } // static_cast to non-const because we need to set to mDefault
        inline field_get_set<FValue>& operator = (FValue&& aValue) { static_cast<value&>(mContainer).set_field(mFieldName, std::forward<FValue>(aValue)); return *this; }
        inline field_get_set<FValue>& operator = (const FValue& aValue) { static_cast<value&>(mContainer).set_field(mFieldName, aValue); return *this; }
        inline field_get_set<FValue>& operator = (const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }

     protected:
        inline value& get_ref() { return static_cast<value&>(mContainer).get_ref(mFieldName, to_value(mDefault)); }
        inline const value& get_ref() const { return static_cast<value&>(mContainer).get_ref(mFieldName, to_value(mDefault)); }
        using rjson_type = typename content_type<FValue>::type;
        inline rjson_type& get_value_ref() { return std::get<rjson_type>(get_ref()); }
        inline const rjson_type& get_value_ref() const { return std::get<rjson_type>(get_ref()); }

     private:
        field_container& mContainer;
        std::string mFieldName;
        FValue mDefault;
    };

      // ----------------------------------------------------------------------
      // Color
      // ----------------------------------------------------------------------

    template <> struct content_type<Color> { using type = string; };

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

      // ----------------------------------------------------------------------
      // Size
      // ----------------------------------------------------------------------

    template <> struct content_type<Size> { using type = array; };

    template <> inline field_get_set<Size>::operator Size() const
    {
        try {
            const auto& ar = std::get<array>(get_ref());
            return {std::get<number>(ar[0]), std::get<number>(ar[1])};
        }
        catch (std::exception& /*err*/) {
            std::cerr << "ERROR: cannot convert json to Size: " << get_ref() << '\n';
            return {};
        }
    }

    template <> inline value to_value<Size>(const Size& aSize)
    {
        return array{number{aSize.width}, number{aSize.height}};

        // array result;
        // result.insert(number{aSize.width});
        // result.insert(number{aSize.height});
        // return result;
    }

} // namespace rjson

// ----------------------------------------------------------------------

template <typename FValue> inline std::ostream& operator<<(std::ostream& out, const rjson::field_get_set<FValue>& aValue)
{
    return out << static_cast<FValue>(aValue);
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
