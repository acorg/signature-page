#pragma once

#include <typeinfo>

#include "acmacs-base/rjson.hh"

#include "acmacs-base/color.hh"
#include "acmacs-base/size.hh"

// ----------------------------------------------------------------------

namespace rjson
{
    inline namespace v2
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
            virtual const value& get_or_empty_array(std::string aFieldName) const = 0;

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
            value& get_or_add(std::string aFieldName, value&& aDefault) override { value& val = mData.set(aFieldName); if (val.is_null()) val = std::move(aDefault); return val; }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { value& val = mData.set(aFieldName); if (val.is_null()) val = aDefault; return val; }
            const value& get_or_empty_array(std::string aFieldName) const override { const value& val = mData[aFieldName]; if (val.is_null()) return EmptyArray; else return val; }

            field_container_toplevel& set_field(std::string aFieldName, value&& aValue) override { mData[aFieldName] = std::move(aValue); return *this; }
            void remove_child(std::string aFieldName) override { mData.remove(aFieldName); }

            std::string to_json() const { return rjson::to_string(mData); }
            std::string to_json_pp(size_t aIndent, json_pp_emacs_indent emacs_indent = json_pp_emacs_indent::yes) const { return rjson::pretty(mData, aIndent, emacs_indent); }

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
            value& get_or_add(std::string aFieldName, value&& aDefault) override { return mParent.get_or_add(mFieldName, object{})[aFieldName] = std::move(aDefault); }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { return mParent.get_or_add(mFieldName, object{})[aFieldName] = aDefault; }
            const value& get_or_empty_array(std::string aFieldName) const override { const value& val = mParent[mFieldName][aFieldName]; if (val.is_null()) return EmptyArray; else return val; }

            void remove_child(std::string aFieldName) override
            {
                mParent[mFieldName].remove(aFieldName);
            }

            field_container_child& set_field(std::string aFieldName, value&& aValue) override
            {
                mParent.get_or_add(mFieldName, object{})[aFieldName] = std::move(aValue);
                return *this;
            }

            const value& data() const { return mParent[mFieldName]; }
            std::string to_json() const { return rjson::to_string(mParent[mFieldName]); }

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
            // class iterator
            // {
            //   public:
            //     using difference_type = array::iterator::difference_type;
            //     using value_type = Element;
            //     using pointer = Element*;
            //     using reference = Element&;
            //     using iterator_category = array::iterator::iterator_category;

            //     iterator& operator++()
            //     {
            //         ++iter;
            //         return *this;
            //     }
            //     iterator operator++(int)
            //     {
            //         iterator retval = *this;
            //         ++(*this);
            //         return retval;
            //     }
            //     bool operator==(iterator other) const { return iter == other.iter; }
            //     bool operator!=(iterator other) const { return !(*this == other); }
            //     Element operator*()
            //     { /* std::cerr << "DEBUG: " << *iter << DEBUG_LINE_FUNC << '\n'; */
            //         return Element{*iter};
            //     }
            //     difference_type operator-(iterator other) const { return iter - other.iter; }

            //   private:
            //     array::iterator iter;
            //     iterator(array::iterator aIter) : iter(aIter) {}

            //     friend class array_field_container_child<Element>;
            // };

            array_field_container_child(field_container_parent& aParent, std::string aFieldName) : mParent{aParent}, mFieldName{aFieldName} {}

            size_t size() const { return get_array().size(); }
            bool empty() const { return get_array().empty(); }
            // iterator begin() const { return get_array().begin(); }
            // iterator end() const { return get_array().end(); }
            // iterator begin() { return get_array().begin(); }
            // iterator end() { return get_array().end(); }
            Element operator[](size_t index) const { return mParent[mFieldName][index]; }
            Element emplace_back()
            {
                try {
                    auto& ar = mParent.get_or_add(mFieldName, array{});
                    ar.insert(object{});
                    return Element{ar[ar.size() - 1]};
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void push_back(size_t val)
            {
                try {
                    mParent.get_or_add(mFieldName, array{}).append(val);
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void push_back(std::string val)
            {
                try {
                    mParent.get_or_add(mFieldName, array{}).append(val);
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << mParent.get_or_empty_array(mFieldName).to_json() << '\n';
                    throw;
                }
            }

            void erase(size_t index) { get_array().remove(index); }
            void clear() { get_array().clear(); }

            std::string to_json() const { return rjson::to_string(get_array()); }

          private:
            field_container_parent& mParent;
            std::string mFieldName;

            value& get_array() { return mParent.get_or_add(mFieldName, array{}); }
            const value& get_array() const { return mParent.get_or_empty_array(mFieldName); }

        }; // class array_field_container_child<>

        class array_field_container_child_element : public field_container_parent
        {
          public:
            array_field_container_child_element(const value& aData) : mData{aData} {}

            const value& operator[](std::string aFieldName) const override { return mData[aFieldName]; }
            value& operator[](std::string aFieldName) override { return const_cast<value&>(mData)[aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override { value& val = const_cast<value&>(mData).set(aFieldName); if (val.is_null()) val = std::move(aDefault); return val; }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { value& val = const_cast<value&>(mData).set(aFieldName); if (val.is_null()) val = aDefault; return val; }
            const value& get_or_empty_object(std::string aFieldName) const { const value& val = mData[aFieldName]; if (val.is_null()) return EmptyObject; else return val; }
            const value& get_or_empty_array(std::string aFieldName) const override { const value& val = mData[aFieldName]; if (val.is_null()) return EmptyArray; else return val; }

            array_field_container_child_element& set_field(std::string aFieldName, value&& aValue) override { const_cast<value&>(mData)[aFieldName] = std::move(aValue); return *this; }
            void remove_child(std::string aFieldName) override { const_cast<value&>(mData).remove(aFieldName); }

            template <typename T> array_field_container_child_element& add(std::string aFieldName, T&& aValue) { return set_field(aFieldName, std::forward<T>(aValue)); }

            const value& data() const { return mData; }
            std::string to_json() const { return rjson::to_string(mData); }

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

            const value& get_value_ref() const { return mParent[mFieldName]; }

            field_get_set<FValue>& operator=(const FValue& aValue) { mParent.set_field(mFieldName, to_value(aValue)); return *this; }
            field_get_set<FValue>& operator=(const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }
            bool operator==(const FValue& aToCompare) const { return static_cast<FValue>(*this) == aToCompare; }
            bool operator!=(const FValue& aToCompare) const { return !operator==(aToCompare); }

            bool empty() const { return static_cast<FValue>(*this).empty(); }

            // to be specialized for complex types
            operator FValue() const { return get_value_ref(); }

          private:
            field_container_parent& mParent;
            std::string mFieldName;
            value mDefault;

            // const value& get() const;
            value& get_or_add() { return mParent.get_or_add(mFieldName, mDefault); }

        }; // class field_get_set<>

        // ----------------------------------------------------------------------
        // Color
        // ----------------------------------------------------------------------

        // template <> struct content_type<Color>
        // {
        //     using type = string;
        // };

        template <> inline field_get_set<Color>::operator Color() const
        {
            try {
                return Color(static_cast<std::string_view>(get_value_ref()));
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to Color: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const Color& aColor) { return aColor.to_string(); }
        template <> inline value to_value(Color&& aColor) { return aColor.to_string(); }

        template <> inline value& field_get_set<Color>::get_or_add() { return mParent.get_or_add(mFieldName, mDefault); }

        // ----------------------------------------------------------------------
        // Size
        // ----------------------------------------------------------------------

        // template <> struct content_type<Size>
        // {
        //     using type = array;
        // };

        template <> inline field_get_set<Size>::operator Size() const
        {
            try {
                const auto& ar = get_value_ref();
                return {static_cast<double>(ar[0]), static_cast<double>(ar[1])};
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to Size: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const Size& aSize) { return array{aSize.width, aSize.height}; }
        template <> inline value to_value(Size&& aSize) { return array{aSize.width, aSize.height}; }

        // ----------------------------------------------------------------------
        // acmacs::Offset
        // ----------------------------------------------------------------------

        // template <> struct content_type<acmacs::Offset>
        // {
        //     using type = array;
        // };

        template <> inline field_get_set<acmacs::Offset>::operator acmacs::Offset() const
        {
            try {
                const auto& ar = get_value_ref();
                return {static_cast<double>(ar[0]), static_cast<double>(ar[1])};
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to acmacs::Offset: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const acmacs::Offset& aOffset) { return array{aOffset.x(), aOffset.y()}; }
        template <> inline value to_value(acmacs::Offset&& aOffset) { return array{aOffset.x(), aOffset.y()}; }

        // ----------------------------------------------------------------------
        // TextStyle
        // ----------------------------------------------------------------------

        // template <> struct content_type<TextStyle>
        // {
        //     using type = object;
        // };

        template <> inline field_get_set<TextStyle>::operator TextStyle() const
        {
            try {
                const auto& obj = get_value_ref();
                TextStyle style;
                assign_if_not_null(obj["family"], style.font_family);
                assign_if_not_null(obj["slant"], style.slant);
                assign_if_not_null(obj["weight"], style.weight);
                return style;
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to TextStyle: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const TextStyle& aTextStyle)
        {
            return object{{"family", *aTextStyle.font_family}, {"slant", static_cast<std::string>(*aTextStyle.slant)}, {"weight", static_cast<std::string>(*aTextStyle.weight)}};
        }

        template <> inline value to_value(TextStyle&& aTextStyle) { return to_value(const_cast<const TextStyle&>(aTextStyle)); }

        // template <> inline value& field_get_set<TextStyle>::get_or_add()
        // {
        //     return mParent.get_or_add(mFieldName, mDefault);
        // }

        // ----------------------------------------------------------------------
        // std::vector<std::string>
        // ----------------------------------------------------------------------

        // template <> struct content_type<std::vector<std::string>>
        // {
        //     using type = array;
        // };

        template <> inline field_get_set<std::vector<std::string>>::operator std::vector<std::string>() const
        {
            try {
                return rjson::as_vector<std::string>(get_value_ref());
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to std::vector<std::string>: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const std::vector<std::string>& aData)
        {
            value ar{array{}};
            for (const auto& elt : aData)
                ar.append(elt);
            return ar;
        }

        template <> inline value to_value(std::vector<std::string>&& aData) { return to_value(const_cast<const std::vector<std::string>&>(aData)); }

        // ----------------------------------------------------------------------
        // std::map<std::string, std::string>
        // ----------------------------------------------------------------------

        // template <> struct content_type<std::map<std::string, std::string>>
        // {
        //     using type = object;
        // };

        template <> inline field_get_set<std::map<std::string, std::string>>::operator std::map<std::string, std::string>() const
        {
            try {
                std::map<std::string, std::string> result;
                rjson::for_each(get_value_ref(), [&result](const object::value_type& key_val) {
                    const auto& [key, val] = key_val;
                    result.emplace(key, static_cast<std::string>(val));
                });
                return result;
            }
            catch (std::exception&) {
                std::cerr << "ERROR: cannot convert json to std::vector<std::string>: " << rjson::to_string(get_value_ref()) << '\n';
                throw;
            }
        }

        template <> inline value to_value(const std::map<std::string, std::string>& aData)
        {
            object obj;
            for (const auto& [key, val] : aData)
                obj[key] = val;
            return obj;
        }

        template <> inline value to_value(std::map<std::string, std::string>&& aData)
        {
            return to_value(const_cast<const std::map<std::string, std::string>&>(aData));
        }

        // ----------------------------------------------------------------------
        //
        // ----------------------------------------------------------------------

        template <typename FValue>
        inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, FValue&& aDefault, initialize_field aInitialize)
            : mParent{aParent}, mFieldName{aFieldName}, mDefault{to_value(std::forward<FValue>(aDefault))}
              // mDefault{to_value(const_cast<const FValue&>(aDefault))} //
        {
            if (aInitialize == initialize_field::yes)
                get_or_add();
        }

        template <typename FValue>
        inline field_get_set<FValue>::field_get_set(field_container_parent& aParent, std::string aFieldName, const FValue& aDefault, initialize_field aInitialize)
            : mParent{aParent}, mFieldName{aFieldName}, mDefault{to_value(aDefault)}
        {
            if (aInitialize == initialize_field::yes)
                get_or_add();
        }

    } // namespace v2
} // namespace rjson

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
