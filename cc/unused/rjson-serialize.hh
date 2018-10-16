#pragma once
#error Obsolete 2018-10-08

#include <typeinfo>
#include <optional>

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

        class field_container_parent;

        // ----------------------------------------------------------------------
        // ----------------------------------------------------------------------

        enum class initialize_field { no, yes };

        class field_get_set_base
        {
         public:
            bool empty() const { return get_value_ref().empty(); }

         protected:
            field_get_set_base(field_container_parent& parent, std::string field_name, const value& a_default);
            field_get_set_base(const field_get_set_base&) = delete; // cannot copy parent_ (source parent_ may be destroyed before this object)
            virtual ~field_get_set_base() = default;
            field_get_set_base& operator=(const value& val);

            const value& get_value_ref() const;

            void copy_from(const field_get_set_base& source)
                {
                      // parent ref must not be copied
                    default_ = source.default_;
                }

            field_container_parent& parent_;
            const std::string field_name_;
            value default_;

            friend class field_container_parent;

        }; // class field_get_set_base

          // ----------------------------------------------------------------------

        template <typename FValue> class field_get_set : public field_get_set_base
        {
          public:
            field_get_set(const field_get_set&) = delete; // field_get_set_base cannot be copy constructed
            field_get_set(field_container_parent& parent, std::string field_name, const FValue& a_default, initialize_field aInitialize = initialize_field::no);
            field_get_set<FValue>& operator=(const FValue& aValue);
            field_get_set<FValue>& operator=(const field_get_set<FValue>& aSource) { return operator=(static_cast<FValue>(aSource)); }
            bool operator==(const FValue& aToCompare) const { return static_cast<FValue>(*this) == aToCompare; }
            bool operator!=(const FValue& aToCompare) const { return !operator==(aToCompare); }

            // to be specialized for complex types
            operator FValue() const
            {
                const value& val = get_value_ref();
                if constexpr (std::is_same_v<FValue, bool>)
                    return static_cast<bool>(val);
                else
                    return static_cast<FValue>(val);
            }

          private:
            value& get_or_add(); // to be specialized for complex types

        }; // class field_get_set<>

        // ----------------------------------------------------------------------
        // ----------------------------------------------------------------------

        class field_container_parent
        {
          public:
            field_container_parent() = default;
            field_container_parent(const field_container_parent&) { /* do nothing, copy_fields() must be called by children */ }
            virtual ~field_container_parent() = default;

            virtual const value& operator[](std::string aFieldName) const = 0;
            virtual const value& get(std::string n1, std::string n2) const = 0;
            virtual value& operator[](std::string aFieldName) = 0;
            virtual value& get_or_add(std::string aFieldName, value&& aDefault) = 0;
            virtual value& get_or_add(std::string aFieldName, const value& aDefault) = 0;
            virtual const value& get_or_empty_array(std::string aFieldName) const = 0;

            virtual field_container_parent& set_field(std::string aFieldName, const value& aValue) = 0;
            virtual void remove_child(std::string aFieldName) = 0;

         protected:
            void copy_fields(const field_container_parent& source)
                {
                    assert(fields_.size() == source.fields_.size());
                    for (size_t f_no = 0; f_no < fields_.size(); ++f_no)
                        fields_[f_no]->copy_from(*source.fields_[f_no]);
                }

         private:
            std::vector<field_get_set_base*> fields_;

            void register_field(field_get_set_base* field) { fields.push_back(field); }

            friend class field_get_set_base;

        }; // class field_container_parent

        // ----------------------------------------------------------------------

        class field_container_toplevel : public field_container_parent
        {
         public:
            field_container_toplevel(const field_container_toplevel&) = default;
            void use_json(value&& aData) { mData = std::move(aData); }
            void update(value&& aData) { mData.update(aData); }

            const value& operator[](std::string aFieldName) const override { return mData.get(aFieldName); }
            const value& get(std::string n1, std::string n2) const override { return mData.get(n1, n2); }
            value& operator[](std::string aFieldName) override { return mData[aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override
            {
                value& val = mData.set(aFieldName);
                if (val.is_null())
                    val = std::move(aDefault);
                return val;
            }
            value& get_or_add(std::string aFieldName, const value& aDefault) override
            {
                value& val = mData.set(aFieldName);
                if (val.is_null())
                    val = aDefault;
                return val;
            }
            const value& get_or_empty_array(std::string aFieldName) const override
            {
                const value& val = mData[aFieldName];
                if (val.is_null())
                    return EmptyArray;
                else
                    return val;
            }

            field_container_toplevel& set_field(std::string aFieldName, const value& aValue) override
            {
                mData[aFieldName] = aValue;
                return *this;
            }
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

            const value& operator[](std::string aFieldName) const override { return mParent.get(mFieldName, aFieldName); }
            const value& get(std::string /*n1*/, std::string /*n2*/) const override { throw std::runtime_error("field_container_child::get(n1, n2)"); }
            value& operator[](std::string aFieldName) override { return mParent[mFieldName][aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override { return mParent.get_or_add(mFieldName, object{})[aFieldName] = std::move(aDefault); }
            value& get_or_add(std::string aFieldName, const value& aDefault) override { return mParent.get_or_add(mFieldName, object{})[aFieldName] = aDefault; }
            const value& get_or_empty_array(std::string aFieldName) const override
            {
                const value& val = mParent[mFieldName][aFieldName];
                if (val.is_null())
                    return EmptyArray;
                else
                    return val;
            }

            void remove_child(std::string aFieldName) override { mParent[mFieldName].remove(aFieldName); }

            field_container_child& set_field(std::string aFieldName, const value& aValue) override
            {
                mParent.get_or_add(mFieldName, object{})[aFieldName] = aValue;
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
            array_field_container_child(field_container_parent& aParent, std::string aFieldName) : mParent{aParent}, mFieldName{aFieldName} {}

            size_t size() const { return get_array().size(); }
            bool empty() const { return get_array().empty(); }
            Element operator[](size_t index) const { return mParent[mFieldName][index]; }
            Element emplace_back()
            {
                try {
                    auto& ar = mParent.get_or_add(mFieldName, array{});
                    ar.append(object{});
                    return Element{ar[ar.size() - 1]};
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << rjson::to_string(mParent.get_or_empty_array(mFieldName)) << '\n';
                    throw;
                }
            }

            void push_back(size_t val)
            {
                try {
                    mParent.get_or_add(mFieldName, array{}).append(val);
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << rjson::to_string(mParent.get_or_empty_array(mFieldName)) << '\n';
                    throw;
                }
            }

            void push_back(std::string val)
            {
                try {
                    mParent.get_or_add(mFieldName, array{}).append(val);
                }
                catch (std::exception&) {
                    std::cerr << "Not array? " << mFieldName << ": " << rjson::to_string(mParent.get_or_empty_array(mFieldName)) << '\n';
                    throw;
                }
            }

            void erase(size_t index) { get_array().remove(index); }
            void clear() { get_array().clear(); }

            std::string to_json() const { return rjson::to_string(get_array()); }

            template <typename Func> std::optional<Element> find_if(Func&& func) const
            {
                if (const value& found = rjson::find_if(get_array(), [&func](const rjson::value& val) -> bool { return func(Element(val)); }); !found.is_null())
                    return Element{found};
                else
                    return {};
            }

            template <typename Func> std::optional<size_t> find_index_if(Func&& func) const { return rjson::find_index_if(get_array(), std::forward<Func>(func)); }

            template <typename Func> void for_each(Func&& func) const
            {
                rjson::for_each(get_array(), [&func](const rjson::value& val) { func(Element(val)); });
            }

            template <typename Func> void for_each(Func&& func)
            {
                rjson::for_each(get_array(), [&func](rjson::value& val) {
                    Element elt(val);
                    func(elt);
                });
            }

            const value& get_array() const { return mParent.get_or_empty_array(mFieldName); }

          private:
            field_container_parent& mParent;
            std::string mFieldName;

            value& get_array() { return mParent.get_or_add(mFieldName, array{}); }

        }; // class array_field_container_child<>

        class array_field_container_child_element : public field_container_parent
        {
          public:
            array_field_container_child_element(const value& aData) : mData{aData} {}
            array_field_container_child_element(const array_field_container_child_element&) = default;
            array_field_container_child_element& operator=(const array_field_container_child_element&) = delete; // cannot re-assign const value& mData
            array_field_container_child_element& operator=(array_field_container_child_element&&) = delete;      // cannot re-assign const value& mData

            const value& operator[](std::string aFieldName) const override { return mData.get(aFieldName); }
            const value& get(std::string n1, std::string n2) const override { return mData.get(n1, n2); }
            value& operator[](std::string aFieldName) override { return const_cast<value&>(mData)[aFieldName]; }
            value& get_or_add(std::string aFieldName, value&& aDefault) override
            {
                value& val = const_cast<value&>(mData).set(aFieldName);
                if (val.is_null())
                    val = std::move(aDefault);
                return val;
            }
            value& get_or_add(std::string aFieldName, const value& aDefault) override
            {
                value& val = const_cast<value&>(mData).set(aFieldName);
                if (val.is_null())
                    val = aDefault;
                return val;
            }
            const value& get_or_empty_object(std::string aFieldName) const
            {
                const value& val = mData[aFieldName];
                if (val.is_null())
                    return EmptyObject;
                else
                    return val;
            }
            const value& get_or_empty_array(std::string aFieldName) const override
            {
                const value& val = mData[aFieldName];
                if (val.is_null())
                    return EmptyArray;
                else
                    return val;
            }

            array_field_container_child_element& set_field(std::string aFieldName, const value& aValue) override
            {
                const_cast<value&>(mData)[aFieldName] = aValue;
                return *this;
            }
            void remove_child(std::string aFieldName) override { const_cast<value&>(mData).remove(aFieldName); }

            template <typename T> array_field_container_child_element& add(std::string aFieldName, T&& aValue) { return set_field(aFieldName, aValue); }

            const value& data() const { return mData; }
            std::string to_json() const { return rjson::to_string(mData); }

          private:
            const value& mData;
        };

        // **********************************************************************
        // specialisations for field_get_set<>
        // **********************************************************************

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

        template <> inline value& field_get_set<Color>::get_or_add() { return parent_.get_or_add(field_name_, default_); }

        // ----------------------------------------------------------------------
        // Size
        // ----------------------------------------------------------------------

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

        template <> inline field_get_set<TextStyle>::operator TextStyle() const
        {
            try {
                const auto& obj = get_value_ref();
                TextStyle style;
                assign_string_if_not_null(obj["family"], style.font_family);
                assign_string_if_not_null(obj["slant"], style.slant);   // assign_if_not_null(obj["slant"], style.slant, [](const value& val) { return acmacs::FontSlant{val}; });
                assign_string_if_not_null(obj["weight"], style.weight); // assign_if_not_null(obj["weight"], style.weight, [](const value& val) { return acmacs::FontWeight{val}; });
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

        // ----------------------------------------------------------------------
        // std::vector<std::string>
        // ----------------------------------------------------------------------

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

        template <> inline field_get_set<std::map<std::string, std::string>>::operator std::map<std::string, std::string>() const
        {
            try {
                std::map<std::string, std::string> result;
                rjson::for_each(get_value_ref(), [&result](const std::string& field_name, const rjson::value& item_value) { result.emplace(field_name, static_cast<std::string>(item_value)); });
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

        template <> inline value to_value(std::map<std::string, std::string>&& aData) { return to_value(const_cast<const std::map<std::string, std::string>&>(aData)); }

        // **********************************************************************
        // implementation
        // **********************************************************************

        inline field_get_set_base::field_get_set_base(field_container_parent& parent, std::string field_name, const value& a_default)
            : parent_(parent), field_name_(field_name), default_(a_default)
        {
            parent_.register_field(this);
        }

        inline field_get_set_base& field_get_set_base::operator=(const value& val)
        {
            parent_.set_field(field_name_, val);
            return *this;
        }

        inline const value& field_get_set_base::get_value_ref() const
        {
            if (const auto& val = parent_[field_name_]; !val.is_null())
                return val;
            else
                return default_;
        }

        // ----------------------------------------------------------------------

        template <typename FValue>
            inline field_get_set<FValue>::field_get_set(field_container_parent& parent, std::string field_name, const FValue& a_default, initialize_field initialize)
                : field_get_set_base(parent, field_name, to_value(a_default))
                {
                    if (initialize == initialize_field::yes)
                        get_or_add();
                }

        template <typename FValue> inline field_get_set<FValue>& field_get_set<FValue>::operator=(const FValue& aValue)
        {
            field_get_set_base::operator=(to_value(aValue));
            return *this;
        }

        template <typename FValue> inline value& field_get_set<FValue>::get_or_add()
        {
            return parent_.get_or_add(field_name_, default_);
        }

        template <typename FValue> inline std::ostream& operator<<(std::ostream& out, const field_get_set<FValue>& field) { return out << static_cast<FValue>(field); }


    } // namespace v2
} // namespace rjson

// ----------------------------------------------------------------------
/// Local Variables:
/// eval: (if (fboundp 'eu-rename-buffer) (eu-rename-buffer))
/// End:
