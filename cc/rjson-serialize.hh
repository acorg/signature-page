#pragma once

#include <typeinfo>

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

        virtual const value& operator[](std::string aFieldName) const = 0;
        virtual value& operator[](std::string aFieldName) = 0;
        virtual value& get_or_add(std::string aFieldName, value&& aDefault) = 0;
        virtual value& get_or_add(std::string aFieldName, const value& aDefault) = 0;
        virtual const array& get_or_empty_array(std::string aFieldName) const = 0;

        virtual field_container_parent& set_field(std::string aFieldName, value&& aValue) = 0;
        virtual void remove_child(std::string aFieldName) = 0;

    }; // class field_container_parent

      // ----------------------------------------------------------------------

    class field_container_toplevel : public field_container_parent
    {
     public:
        inline void use_json(rjson::value&& aData) { mData = std::move(aData); }
        inline void update(rjson::value&& aData) { mData.update(aData); }

        inline const value& operator[](std::string aFieldName) const override { return mData[aFieldName]; }
        inline value& operator[](std::string aFieldName) override { return mData[aFieldName]; }
        inline value& get_or_add(std::string aFieldName, value&& aDefault) override { return mData.get_or_add(aFieldName, std::forward<value>(aDefault)); }
        inline value& get_or_add(std::string aFieldName, const value& aDefault) override { return mData.get_or_add(aFieldName, aDefault); }
        inline const array& get_or_empty_array(std::string aFieldName) const override { return mData.get_or_empty_array(aFieldName); }

        inline field_container_toplevel& set_field(std::string aFieldName, value&& aValue) override { mData.set_field(aFieldName, std::forward<value>(aValue)); return *this; }
        inline void remove_child(std::string aFieldName) override { try { std::get<object>(mData).delete_field(aFieldName); } catch (field_not_found&) {} }

        inline std::string to_json() const { return mData.to_json(); }
        inline std::string to_json_pp(size_t aIndent, json_pp_emacs_indent emacs_indent = json_pp_emacs_indent::yes) const { return mData.to_json_pp(aIndent, emacs_indent); }

     private:
        mutable rjson::value mData = rjson::object{};

    }; // class field_container_toplevel

      // ----------------------------------------------------------------------

    class field_container_child : public field_container_parent
    {
     public:
        inline field_container_child(field_container_parent& aParent, std::string aFieldName)
            : mParent{aParent}, mFieldName{aFieldName} {}

        inline const value& operator[](std::string aFieldName) const override { return mParent[mFieldName][aFieldName]; }
        inline value& operator[](std::string aFieldName) override { return mParent[mFieldName][aFieldName]; }
        inline value& get_or_add(std::string aFieldName, value&& aDefault) override { return mParent.get_or_add(mFieldName, rjson::object{}).get_or_add(aFieldName, std::forward<value>(aDefault)); }
        inline value& get_or_add(std::string aFieldName, const value& aDefault) override { return mParent.get_or_add(mFieldName, rjson::object{}).get_or_add(aFieldName, aDefault); }
        inline const array& get_or_empty_array(std::string aFieldName) const override { return mParent[mFieldName].get_or_empty_array(aFieldName); }

        inline void remove_child(std::string aFieldName) override { try { mParent[mFieldName].delete_field(aFieldName); } catch (field_not_found&) {} }

        inline field_container_child& set_field(std::string aFieldName, value&& aValue) override { const_cast<value&>(mParent[mFieldName]).set_field(aFieldName, std::forward<value>(aValue)); return *this; }

        inline const object& data() const { return mParent[mFieldName]; }
        inline std::string to_json() const { return mParent[mFieldName].to_json(); }

          // remove this field from parent
        inline void remove() { mParent.remove_child(mFieldName); }

     private:
        field_container_parent& mParent;
        std::string mFieldName;

    }; // class field_container_child

      // ----------------------------------------------------------------------

    template <typename Element> class array_field_container_child
    {
     public:
        class iterator
        {
         public:
            using difference_type = array::iterator::difference_type;
            using value_type = Element;
            using pointer = Element*;
            using reference = Element&;
            using iterator_category = array::iterator::iterator_category;

            inline iterator& operator++() { ++iter; return *this;}
            inline iterator operator++(int) { iterator retval = *this; ++(*this); return retval;}
            inline bool operator==(iterator other) const { return iter == other.iter; }
            inline bool operator!=(iterator other) const { return !(*this == other); }
            inline Element operator*() { /* std::cerr << "DEBUG: " << *iter << DEBUG_LINE_FUNC << '\n'; */ return Element{*iter}; }
            inline difference_type operator-(iterator other) const { return iter - other.iter; }

         private:
            array::iterator iter;
            inline iterator(array::iterator aIter) : iter(aIter) {}

            friend class array_field_container_child<Element>;
        };

        inline array_field_container_child(field_container_parent& aParent, std::string aFieldName)
            : mParent{aParent}, mFieldName{aFieldName} {}

        inline size_t size() const { return get_array().size(); }
        inline bool empty() const { return get_array().empty(); }
        inline iterator begin() const { return get_array().begin(); }
        inline iterator end() const { return get_array().end(); }
        inline iterator begin() { return get_array().begin(); }
        inline iterator end() { return get_array().end(); }
        inline Element operator[](size_t index) const { return mParent[mFieldName][index]; }
        inline Element emplace_back()
            {
                try {
                    rjson::array& ar = mParent.get_or_add(mFieldName, array{});
                    ar.insert(object{});
                    return Element{*ar.rbegin()};
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName) << '\n';
                    throw;
                }
            }

        inline void erase(size_t index) { get_array().erase(index); }
        inline void clear() { get_array().clear(); }

        inline std::string to_json() const { return get_array().to_json(); }

     private:
        field_container_parent& mParent;
        std::string mFieldName;

        inline array& get_array() { return mParent.get_or_add(mFieldName, array{}); }
        inline const array& get_array() const { return mParent.get_or_empty_array(mFieldName); }

    }; // class array_field_container_child<>

    class array_field_container_child_element : public field_container_parent
    {
     public:
        inline array_field_container_child_element(const value& aData) : mData{aData} {}

        inline const value& operator[](std::string aFieldName) const override { return mData[aFieldName]; }
        inline value& operator[](std::string aFieldName) override { return const_cast<value&>(mData)[aFieldName]; }
        inline value& get_or_add(std::string aFieldName, value&& aDefault) override { return const_cast<value&>(mData).get_or_add(aFieldName, std::forward<value>(aDefault)); }
        inline value& get_or_add(std::string aFieldName, const value& aDefault) override { return const_cast<value&>(mData).get_or_add(aFieldName, aDefault); }
        inline const object& get_or_empty_object(std::string aFieldName) const { return mData.get_or_empty_object(aFieldName); }
        inline const array& get_or_empty_array(std::string aFieldName) const override { return mData.get_or_empty_array(aFieldName); }

        inline array_field_container_child_element& set_field(std::string aFieldName, value&& aValue) override { const_cast<value&>(mData).set_field(aFieldName, std::forward<value>(aValue)); return *this; }
        inline void remove_child(std::string aFieldName) override { try { std::get<object>(const_cast<value&>(mData)).delete_field(aFieldName); } catch (std::exception&) {} }

        template <typename T> inline array_field_container_child_element& add(std::string aFieldName, T&& aValue) { return set_field(aFieldName, to_value(std::forward<T>(aValue))); }

        inline const value& data() const { return mData; }
        inline std::string to_json() const { return mData.to_json(); }

     private:
        const value& mData;
    };

      // ----------------------------------------------------------------------

    enum class initialize_field { no, yes };

    template <typename FValue> class field_get_set
    {
     public:
        field_get_set(field_container_parent& aParent, std::string aFieldName, FValue&& aDefault, initialize_field aInitialize = initialize_field::no);
        field_get_set(field_container_parent& aParent, std::string aFieldName, const FValue& aDefault, initialize_field aInitialize = initialize_field::no);

        inline const rjson_type<FValue>& get_value_ref() const
            {
                try {
                    const auto& val = mParent[mFieldName];
                    try {
                        return val;
                    }
                    catch (std::bad_variant_access&) {
                        std::cerr << "ERROR: cannot convert rjson::value to " <<  typeid(rjson_type<FValue>).name() << " (" << typeid(FValue).name() << "): " << val << '\n';
                        throw;
                    }
                }
                catch (field_not_found&) {
                    return mDefault;
                }
            }

          // inline field_get_set<FValue>& operator = (FValue&& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
        inline field_get_set<FValue>& operator = (const FValue& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
        inline field_get_set<FValue>& operator = (const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }
        inline bool operator == (const FValue& aToCompare) const { return static_cast<FValue>(*this) == aToCompare; }
        inline bool operator != (const FValue& aToCompare) const { return !operator==(aToCompare); }

        inline bool empty() const { return static_cast<FValue>(*this).empty(); }

          // to be specialized for complex types
        inline operator FValue() const { return get_value_ref(); }

     private:
        field_container_parent& mParent;
        std::string mFieldName;
        rjson_type<FValue> mDefault;

        // const value& get() const;
        inline value& get_or_add()
            {
                return mParent.get_or_add(mFieldName, mDefault);
            }

    }; // class field_get_set<>

      // ----------------------------------------------------------------------
      // double: can be extracted from rjson::number and rjson::integer
      // ----------------------------------------------------------------------

    template <> inline field_get_set<double>::operator double() const
    {
        try {
            return mParent[mFieldName];
        }
        catch (field_not_found&) {
            return mDefault;
        }
    }

      // ----------------------------------------------------------------------
      // Color
      // ----------------------------------------------------------------------

    template <> struct content_type<Color> { using type = string; };

    template <> inline field_get_set<Color>::operator Color() const
    {
        try {
            return static_cast<std::string>(get_value_ref());
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to Color: " << get_value_ref() << '\n';
            throw;
        }
    }

    template <> inline value to_value<Color>(const Color& aColor)
    {
        return rjson::string{aColor.to_string()};
    }

    template <> inline value to_value<Color>(Color&& aColor)
    {
        return rjson::string{aColor.to_string()};
    }

    template <> inline value& field_get_set<Color>::get_or_add()
    {
        return mParent.get_or_add(mFieldName, mDefault);
    }

      // ----------------------------------------------------------------------
      // Size
      // ----------------------------------------------------------------------

    template <> struct content_type<Size> { using type = array; };

    template <> inline field_get_set<Size>::operator Size() const
    {
        try {
            const auto& ar = get_value_ref();
            return {static_cast<double>(ar[0]), static_cast<double>(ar[1])};
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to Size: " << get_value_ref() << '\n';
            throw;
        }
    }

    template <> inline value to_value<Size>(const Size& aSize)
    {
        return array{aSize.width, aSize.height};
    }

    template <> inline value to_value<Size>(Size&& aSize)
    {
        return array{aSize.width, aSize.height};
    }

      // ----------------------------------------------------------------------
      // TextStyle
      // ----------------------------------------------------------------------

    template <> struct content_type<TextStyle> { using type = object; };

    template <> inline field_get_set<TextStyle>::operator TextStyle() const
    {
        try {
            const auto& obj = get_value_ref();
            TextStyle style;
            try { style.font_family(obj["family"]); } catch (field_not_found&) {}
            try { style.slant(obj["slant"]); } catch (field_not_found&) {}
            try { style.weight(obj["weight"]); } catch (field_not_found&) {}
            return style;
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to TextStyle: " << get_value_ref() << '\n';
            throw;
        }
    }

    template <> inline value to_value<TextStyle>(const TextStyle& aTextStyle)
    {
        return object{
            {"family", string{aTextStyle.font_family()}},
            {"slant", string{aTextStyle.slant_as_stirng()}},
            {"weight", string{aTextStyle.weight_as_stirng()}}
        };
    }

    template <> inline value to_value<TextStyle>(TextStyle&& aTextStyle)
    {
        return to_value(const_cast<const TextStyle&>(aTextStyle));
    }

    // template <> inline value& field_get_set<TextStyle>::get_or_add()
    // {
    //     return mParent.get_or_add(mFieldName, mDefault);
    // }

      // ----------------------------------------------------------------------
      // std::vector<std::string>
      // ----------------------------------------------------------------------

    template <> struct content_type<std::vector<std::string>> { using type = array; };

    template <> inline field_get_set<std::vector<std::string>>::operator std::vector<std::string>() const
    {
        try {
            const auto& ar = get_value_ref();
            std::vector<std::string> result{ar.size()};
            std::transform(ar.begin(), ar.end(), result.begin(), [](const rjson::value& elt) -> std::string { return std::get<rjson::string>(elt); });
            return result;
        }
        catch (std::exception&) {
            std::cerr << "ERROR: cannot convert json to std::vector<std::string>: " << get_value_ref() << '\n';
            throw;
        }
    }

    template <> inline value to_value<std::vector<std::string>>(const std::vector<std::string>& aData)
    {
        array ar;
        for (const auto& elt: aData)
            ar.insert(string{elt});
        return ar;
    }

    template <> inline value to_value<std::vector<std::string>>(std::vector<std::string>&& aData)
    {
        return to_value(const_cast<const std::vector<std::string>&>(aData));
    }

      // ----------------------------------------------------------------------
      // std::vector<std::string>
      // ----------------------------------------------------------------------

    template <typename FValue> inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, FValue&& aDefault, initialize_field aInitialize)
        : mParent{aParent}, mFieldName{aFieldName}, mDefault{static_cast<const rjson_type<FValue>&>(to_value(std::forward<FValue>(aDefault)))} // mDefault{to_value(const_cast<const FValue&>(aDefault))} //
    {
        if (aInitialize == initialize_field::yes)
            get_or_add();
    }

    template <typename FValue> inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, const FValue& aDefault, initialize_field aInitialize)
        : mParent{aParent}, mFieldName{aFieldName}, mDefault{static_cast<const rjson_type<FValue>&>(to_value(aDefault))}
    {
        if (aInitialize == initialize_field::yes)
            get_or_add();
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
