#pragma once

#include "acmacs-base/rjson.hh"

#include "acmacs-base/color.hh"
#include "acmacs-draw/size.hh"

// ----------------------------------------------------------------------

namespace rjson
{
    class field_container_parent
    {
     public:
        virtual inline ~field_container_parent() {}

        virtual value& get_ref(std::string aFieldName, value&& aDefault) = 0;
        virtual const value& get_ref(std::string aFieldName, value&& aDefault) const = 0;
        virtual object& get_ref_to_object(std::string aFieldName) = 0;
        virtual void set_field(std::string aFieldName, value&& aValue) = 0;

    }; // class field_container_parent

      // ----------------------------------------------------------------------

    class field_container_toplevel : public field_container_parent
    {
     public:
        inline void use_json(rjson::value&& aData) { mData = std::move(aData); }

        inline value& get_ref(std::string aFieldName, value&& aDefault) override { return mData.get_ref(aFieldName, std::forward<value>(aDefault)); }
        inline const value& get_ref(std::string aFieldName, value&& aDefault) const override { return mData.get_ref(aFieldName, std::forward<value>(aDefault)); }
        inline object& get_ref_to_object(std::string aFieldName) override { return mData.get_ref_to_object(aFieldName); }
        inline void set_field(std::string aFieldName, value&& aValue) override { mData.set_field(aFieldName, std::forward<value>(aValue)); }

        inline std::string to_json() const { return mData.to_json(); }

     private:
        mutable rjson::value mData = rjson::object{};

    }; // class field_container_toplevel

      // ----------------------------------------------------------------------

    class field_container_child : public field_container_parent
    {
     public:
        inline field_container_child(field_container_parent& aParent, std::string aFieldName)
            : mParent{aParent}, mFieldName{aFieldName} {}

        inline object& get_ref_to_object() { return mParent.get_ref_to_object(mFieldName); }
        inline const object& get_ref_to_object() const { return mParent.get_ref_to_object(mFieldName); }
        inline object& get_ref_to_object(std::string aFieldName) override { return get_ref_to_object().get_ref_to_object(aFieldName); }

        inline value& get_ref(std::string aFieldName, value&& aDefault) override { return get_ref_to_object().get_ref(aFieldName, std::forward<value>(aDefault)); }
        inline const value& get_ref(std::string aFieldName, value&& aDefault) const override { return get_ref_to_object().get_ref(aFieldName, std::forward<value>(aDefault)); }

        inline void set_field(std::string aFieldName, value&& aValue) override { get_ref_to_object().set_field(aFieldName, std::forward<value>(aValue)); }

        inline std::string to_json() const { return get_ref_to_object().to_json(); }

     private:
        field_container_parent& mParent;
        std::string mFieldName;

    }; // class field_container_child

      // ----------------------------------------------------------------------

    template <typename FValue> class field_get_set
    {
     public:
        inline field_get_set(field_container_parent& aParent, std::string aFieldName, FValue&& aDefault) : mParent{aParent}, mFieldName{aFieldName}, mDefault{std::move(aDefault)} {}
        inline operator FValue() const { return std::get<rjson_type<FValue>>(get_ref()); }
          // inline field_get_set<FValue>& operator = (FValue&& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
        inline field_get_set<FValue>& operator = (const FValue& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
        inline field_get_set<FValue>& operator = (const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }

     private:
        field_container_parent& mParent;
        std::string mFieldName;
        FValue mDefault;

        // inline value& get_ref() { return mParent.get_ref(mFieldName, to_value(mDefault)); }
        inline const value& get_ref() const { return mParent.get_ref(mFieldName, to_value(mDefault)); }
        // inline rjson_type<FValue>& get_value_ref() { return std::get<rjson_type<FValue>>(get_ref()); }
        inline const rjson_type<FValue>& get_value_ref() const { return std::get<rjson_type<FValue>>(get_ref()); }

    }; // class field_get_set<>

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
            const auto& ar = get_value_ref(); // std::get<array>(get_ref());
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
    }

      // ----------------------------------------------------------------------
      // TextStyle
      // ----------------------------------------------------------------------

    template <> struct content_type<TextStyle> { using type = object; };

    template <> inline field_get_set<TextStyle>::operator TextStyle() const
    {
        try {
            const auto& obj = get_value_ref(); // std::get<object>(get_ref());
            TextStyle style;
            style.font_family(obj.get_field("family", std::string{}));
            style.slant(obj.get_field("slant", std::string{"normal"}));
            style.weight(obj.get_field("weight", std::string{"normal"}));
            return style;
        }
        catch (std::exception& /*err*/) {
            std::cerr << "ERROR: cannot convert json to TextStyle: " << get_ref() << '\n';
            return {};
        }
    }

    template <> inline value to_value<TextStyle>(const TextStyle& aTextStyle)
    {
        return object{string{"family"}, string{aTextStyle.font_family()},
                      string{"slant"}, string{aTextStyle.slant_as_stirng()},
                      string{"weight"}, string{aTextStyle.weight_as_stirng()}};
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
