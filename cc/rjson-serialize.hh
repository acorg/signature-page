#pragma once

#include <typeinfo>

#include "acmacs-base/rjson.hh"

#include "acmacs-base/color.hh"
#include "acmacs-base/size.hh"

// ----------------------------------------------------------------------

namespace rjson
{
    namespace v1
    {
        using Size = acmacs::Size;
        using TextStyle = acmacs::TextStyle;

        class field_container_parent
        {
          public:
            virtual ~field_container_parent() = default;

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
            void use_json(value&& aData) { mData = std::move(aData); }
            void update(value&& aData) { mData.update(aData); }

            const value& operator[](std::string aFieldName) const override { return mData[aFieldName]; }
            value& operator[](std::string aFieldName) override { return mData[aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override { return mData.get_or_add(aFieldName, std::forward<value>(aDefault)); }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { return mData.get_or_add(aFieldName, aDefault); }
            const array& get_or_empty_array(std::string aFieldName) const override { return mData.get_or_empty_array(aFieldName); }

            field_container_toplevel& set_field(std::string aFieldName, value&& aValue) override
            {
                mData.set_field(aFieldName, std::forward<value>(aValue));
                return *this;
            }
            void remove_child(std::string aFieldName) override
            {
                try {
                    std::get<object>(mData).delete_field(aFieldName);
                }
                catch (field_not_found&) {
                }
            }

            std::string to_json() const { return mData.to_json(); }
            std::string to_json_pp(size_t aIndent, json_pp_emacs_indent emacs_indent = json_pp_emacs_indent::yes) const { return mData.to_json_pp(aIndent, emacs_indent); }

          private:
            mutable value mData = object{};

        }; // class field_container_toplevel

        // ----------------------------------------------------------------------

        class field_container_child : public field_container_parent
        {
          public:
            field_container_child(field_container_parent& aParent, std::string aFieldName) : mParent{aParent}, mFieldName{aFieldName} {}

            const value& operator[](std::string aFieldName) const override { return mParent[mFieldName][aFieldName]; }
            value& operator[](std::string aFieldName) override { return mParent[mFieldName][aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override { return mParent.get_or_add(mFieldName, object{}).get_or_add(aFieldName, std::forward<value>(aDefault)); }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { return mParent.get_or_add(mFieldName, object{}).get_or_add(aFieldName, aDefault); }
            const array& get_or_empty_array(std::string aFieldName) const override { return mParent[mFieldName].get_or_empty_array(aFieldName); }

            void remove_child(std::string aFieldName) override
            {
                try {
                    mParent[mFieldName].delete_field(aFieldName);
                }
                catch (field_not_found&) {
                }
            }

            field_container_child& set_field(std::string aFieldName, value&& aValue) override
            {
                const_cast<value&>(mParent[mFieldName]).set_field(aFieldName, std::forward<value>(aValue));
                return *this;
            }

            const object& data() const { return mParent[mFieldName]; }
            std::string to_json() const { return mParent[mFieldName].to_json(); }

            // remove this field from parent
            void remove() { mParent.remove_child(mFieldName); }

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

                iterator& operator++()
                {
                    ++iter;
                    return *this;
                }
                iterator operator++(int)
                {
                    iterator retval = *this;
                    ++(*this);
                    return retval;
                }
                bool operator==(iterator other) const { return iter == other.iter; }
                bool operator!=(iterator other) const { return !(*this == other); }
                Element operator*()
                { /* std::cerr << "DEBUG: " << *iter << DEBUG_LINE_FUNC << '\n'; */
                    return Element{*iter};
                }
                difference_type operator-(iterator other) const { return iter - other.iter; }

              private:
                array::iterator iter;
                iterator(array::iterator aIter) : iter(aIter) {}

                friend class array_field_container_child<Element>;
            };

            array_field_container_child(field_container_parent& aParent, std::string aFieldName) : mParent{aParent}, mFieldName{aFieldName} {}

            size_t size() const { return get_array().size(); }
            bool empty() const { return get_array().empty(); }
            iterator begin() const { return get_array().begin(); }
            iterator end() const { return get_array().end(); }
            iterator begin() { return get_array().begin(); }
            iterator end() { return get_array().end(); }
            Element operator[](size_t index) const { return mParent[mFieldName][index]; }
            Element emplace_back()
            {
                try {
                    array& ar = mParent.get_or_add(mFieldName, array{});
                    ar.insert(object{});
                    return Element{*ar.rbegin()};
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void push_back(size_t value)
            {
                try {
                    array& ar = mParent.get_or_add(mFieldName, array{});
                    ar.insert(integer{value});
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void push_back(std::string value)
            {
                try {
                    array& ar = mParent.get_or_add(mFieldName, array{});
                    ar.insert(string{value});
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void erase(size_t index) { get_array().erase(index); }
            void clear() { get_array().clear(); }

            std::string to_json() const { return get_array().to_json(); }

          private:
            field_container_parent& mParent;
            std::string mFieldName;

            array& get_array() { return mParent.get_or_add(mFieldName, array{}); }
            const array& get_array() const { return mParent.get_or_empty_array(mFieldName); }

        }; // class array_field_container_child<>

        class array_field_container_child_element : public field_container_parent
        {
          public:
            array_field_container_child_element(const value& aData) : mData{aData} {}

            const value& operator[](std::string aFieldName) const override { return mData[aFieldName]; }
            value& operator[](std::string aFieldName) override { return const_cast<value&>(mData)[aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override { return const_cast<value&>(mData).get_or_add(aFieldName, std::forward<value>(aDefault)); }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { return const_cast<value&>(mData).get_or_add(aFieldName, aDefault); }
            const object& get_or_empty_object(std::string aFieldName) const { return mData.get_or_empty_object(aFieldName); }
            const array& get_or_empty_array(std::string aFieldName) const override { return mData.get_or_empty_array(aFieldName); }

            array_field_container_child_element& set_field(std::string aFieldName, value&& aValue) override
            {
                const_cast<value&>(mData).set_field(aFieldName, std::forward<value>(aValue));
                return *this;
            }
            void remove_child(std::string aFieldName) override
            {
                try {
                    std::get<object>(const_cast<value&>(mData)).delete_field(aFieldName);
                }
                catch (std::exception&) {
                }
            }

            template <typename T> array_field_container_child_element& add(std::string aFieldName, T&& aValue) { return set_field(aFieldName, to_value(std::forward<T>(aValue))); }

            const value& data() const { return mData; }
            std::string to_json() const { return mData.to_json(); }

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

            const rjson_type<FValue>& get_value_ref() const
            {
                try {
                    const auto& val = mParent[mFieldName];
                    try {
                        return val;
                    }
                    catch (std::bad_variant_access&) {
                        std::cerr << "ERROR: cannot convert rjson::value to " << typeid(rjson_type<FValue>).name() << " (" << typeid(FValue).name() << "): " << val.to_json() << '\n';
                        throw;
                    }
                }
                catch (field_not_found&) {
                    return mDefault;
                }
            }

            // field_get_set<FValue>& operator = (FValue&& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
            field_get_set<FValue>& operator=(const FValue& aValue)
            {
                mParent.set_field(mFieldName, to_value(aValue));
                return *this;
            }
            field_get_set<FValue>& operator=(const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }
            bool operator==(const FValue& aToCompare) const { return static_cast<FValue>(*this) == aToCompare; }
            bool operator!=(const FValue& aToCompare) const { return !operator==(aToCompare); }

            bool empty() const { return static_cast<FValue>(*this).empty(); }

            // to be specialized for complex types
            operator FValue() const
            {
                if constexpr (std::is_same_v<FValue, std::string>)
                    return get_value_ref().str();
                else
                    return get_value_ref();
            }

          private:
            field_container_parent& mParent;
            std::string mFieldName;
            rjson_type<FValue> mDefault;

            // const value& get() const;
            value& get_or_add() { return mParent.get_or_add(mFieldName, mDefault); }

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

        template <> struct content_type<Color>
        {
            using type = string;
        };

        template <> inline field_get_set<Color>::operator Color() const
        {
            try {
                return Color(get_value_ref());
            }
            catch (std::exception&) {
                const auto& val_ref = get_value_ref();
                std::cerr << "ERROR: cannot convert json to Color: " << val_ref.to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<Color>(const Color& aColor) { return string{aColor.to_string()}; }

        template <> inline value to_value<Color>(Color&& aColor) { return string{aColor.to_string()}; }

        template <> inline value& field_get_set<Color>::get_or_add() { return mParent.get_or_add(mFieldName, mDefault); }

        // ----------------------------------------------------------------------
        // Size
        // ----------------------------------------------------------------------

        template <> struct content_type<Size>
        {
            using type = array;
        };

        template <> inline field_get_set<Size>::operator Size() const
        {
            try {
                const auto& ar = get_value_ref();
                return {static_cast<double>(ar[0]), static_cast<double>(ar[1])};
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to Size: " << get_value_ref().to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<Size>(const Size& aSize) { return array{aSize.width, aSize.height}; }

        template <> inline value to_value<Size>(Size&& aSize) { return array{aSize.width, aSize.height}; }

        // ----------------------------------------------------------------------
        // acmacs::Offset
        // ----------------------------------------------------------------------

        template <> struct content_type<acmacs::Offset>
        {
            using type = array;
        };

        template <> inline field_get_set<acmacs::Offset>::operator acmacs::Offset() const
        {
            try {
                const auto& ar = get_value_ref();
                return {static_cast<double>(ar[0]), static_cast<double>(ar[1])};
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to acmacs::Offset: " << get_value_ref().to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<acmacs::Offset>(const acmacs::Offset& aOffset) { return array{aOffset.x(), aOffset.y()}; }

        template <> inline value to_value<acmacs::Offset>(acmacs::Offset&& aOffset) { return array{aOffset.x(), aOffset.y()}; }

        // ----------------------------------------------------------------------
        // TextStyle
        // ----------------------------------------------------------------------

        template <> struct content_type<TextStyle>
        {
            using type = object;
        };

        template <> inline field_get_set<TextStyle>::operator TextStyle() const
        {
            try {
                const auto& obj = get_value_ref();
                TextStyle style;
                try {
                    style.font_family = obj["family"].str();
                }
                catch (field_not_found&) {
                }
                try {
                    style.slant = obj["slant"].str();
                }
                catch (field_not_found&) {
                }
                try {
                    style.weight = obj["weight"].str();
                }
                catch (field_not_found&) {
                }
                return style;
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to TextStyle: " << get_value_ref().to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<TextStyle>(const TextStyle& aTextStyle)
        {
            return object{{"family", string{aTextStyle.font_family}}, {"slant", string{*aTextStyle.slant}}, {"weight", string{*aTextStyle.weight}}};
        }

        template <> inline value to_value<TextStyle>(TextStyle&& aTextStyle) { return to_value(const_cast<const TextStyle&>(aTextStyle)); }

        // template <> inline value& field_get_set<TextStyle>::get_or_add()
        // {
        //     return mParent.get_or_add(mFieldName, mDefault);
        // }

        // ----------------------------------------------------------------------
        // std::vector<std::string>
        // ----------------------------------------------------------------------

        template <> struct content_type<std::vector<std::string>>
        {
            using type = array;
        };

        template <> inline field_get_set<std::vector<std::string>>::operator std::vector<std::string>() const
        {
            try {
                const auto& ar = get_value_ref();
                std::vector<std::string> result{ar.size()};
                std::transform(ar.begin(), ar.end(), result.begin(), [](const value& elt) -> std::string { return std::get<string>(elt).str(); });
                return result;
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to std::vector<std::string>: " << get_value_ref().to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<std::vector<std::string>>(const std::vector<std::string>& aData)
        {
            array ar;
            for (const auto& elt : aData)
                ar.insert(string{elt});
            return ar;
        }

        template <> inline value to_value<std::vector<std::string>>(std::vector<std::string>&& aData) { return to_value(const_cast<const std::vector<std::string>&>(aData)); }

        // ----------------------------------------------------------------------
        // std::map<std::string, std::string>
        // ----------------------------------------------------------------------

        template <> struct content_type<std::map<std::string, std::string>>
        {
            using type = object;
        };

        template <> inline field_get_set<std::map<std::string, std::string>>::operator std::map<std::string, std::string>() const
        {
            try {
                const object& obj = get_value_ref();
                std::map<std::string, std::string> result;
                for (const auto& [key, val] : obj)
                    result.emplace(key, val.str());
                return result;
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to std::vector<std::string>: " << get_value_ref().to_json() << '\n';
                throw;
            }
        }

        template <> inline value to_value<std::map<std::string, std::string>>(const std::map<std::string, std::string>& aData)
        {
            object obj;
            for (const auto& [key, val] : aData)
                obj[key] = string(val);
            return obj;
        }

        template <> inline value to_value<std::map<std::string, std::string>>(std::map<std::string, std::string>&& aData)
        {
            return to_value(const_cast<const std::map<std::string, std::string>&>(aData));
        }

        // ----------------------------------------------------------------------
        //
        // ----------------------------------------------------------------------

        template <typename FValue>
        inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, FValue&& aDefault, initialize_field aInitialize)
            : mParent{aParent}, mFieldName{aFieldName}, mDefault{static_cast<const rjson_type<FValue>&>(to_value(std::forward<FValue>(aDefault)))}
              // mDefault{to_value(const_cast<const FValue&>(aDefault))} //
        {
            if (aInitialize == initialize_field::yes)
                get_or_add();
        }

        template <typename FValue>
        inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, const FValue& aDefault, initialize_field aInitialize)
            : mParent{aParent}, mFieldName{aFieldName}, mDefault{static_cast<const rjson_type<FValue>&>(to_value(aDefault))}
        {
            if (aInitialize == initialize_field::yes)
                get_or_add();
        }

    } // namespace v1
} // namespace rjson

// ----------------------------------------------------------------------

template <typename FValue> inline std::ostream& operator<<(std::ostream& out, const rjson::v1::field_get_set<FValue>& aValue)
{
    return out << static_cast<FValue>(aValue);
}

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
