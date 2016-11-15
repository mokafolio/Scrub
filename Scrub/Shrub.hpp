#ifndef SCRUB_SHRUB_HPP
#define SCRUB_SHRUB_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/UniquePtr.hpp>
#include <Stick/String.hpp>
#include <Stick/Maybe.hpp>
#include <Stick/Map.hpp>
#include <Stick/Path.hpp>
#include <Stick/StringConversion.hpp>
#include <Stick/Utility.hpp>
#include <Stick/URI.hpp>
#include <Stick/Result.hpp>

#include <type_traits>

namespace scrub
{
    //hints for serializing
    STICK_API_ENUM_CLASS(ValueHint)
    {
        None,
        JSONInt,
        JSONBool,
        JSONString,
        JSONDouble,
        JSONObject,
        JSONArray,
        XMLAttribute
    };

    namespace detail
    {
        template<class T>
        inline T convert(const stick::String & _str);

        template<>
        inline const stick::String & convert<const stick::String &>(const stick::String & _str)
        {
            return _str;
        }

        template<>
        inline bool convert<bool>(const stick::String & _str)
        {
            return _str == "true";
        }

        template<>
        inline stick::Float32 convert<stick::Float32>(const stick::String & _str)
        {
            return stick::toFloat32(_str);
        }

        template<>
        inline stick::Float64 convert<stick::Float64>(const stick::String & _str)
        {
            return stick::toFloat64(_str);
        }

        template<>
        inline stick::Int32 convert<stick::Int32>(const stick::String & _str)
        {
            return stick::toInt32(_str);
        }

        template<>
        inline stick::UInt32 convert<stick::UInt32>(const stick::String & _str)
        {
            return stick::toUInt32(_str);
        }

        template<>
        inline stick::Int64 convert<stick::Int64>(const stick::String & _str)
        {
            return stick::toInt64(_str);
        }

        template<>
        inline stick::UInt64 convert<stick::UInt64>(const stick::String & _str)
        {
            return stick::toUInt64(_str);
        }

        template<class T>
        inline stick::String toString(T _val, stick::Allocator & _alloc)
        {
            return stick::toString(_val, _alloc);
        }

        inline stick::String toString(const stick::String & _str, stick::Allocator & _alloc)
        {
            return _str;
        }

        inline stick::String toString(const char * _str, stick::Allocator & _alloc)
        {
            return stick::String(_str, _alloc);
        }

        template<class T>
        inline ValueHint deduceHint()
        {
            return ValueHint::None;
        }

        template<>
        inline ValueHint deduceHint<stick::UInt8>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::Int8>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::UInt16>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::Int16>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::UInt32>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::Int32>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::UInt64>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::Int64>()
        {
            return ValueHint::JSONInt;
        }

        template<>
        inline ValueHint deduceHint<stick::Float32>()
        {
            return ValueHint::JSONDouble;
        }

        template<>
        inline ValueHint deduceHint<stick::Float64>()
        {
            return ValueHint::JSONDouble;
        }

        template<>
        inline ValueHint deduceHint<stick::String>()
        {
            return ValueHint::JSONString;
        }
    }

    class STICK_API Shrub
    {
    public:

        typedef stick::DynamicArray<Shrub> ChildArray;
        typedef ChildArray::Iter ChildIter;
        typedef ChildArray::ConstIter ChildConstIter;
        typedef ChildArray::ReverseIter ReverseChildIter;
        typedef ChildArray::ReverseConstIter ReverseChildConstIter;


        Shrub(stick::Allocator & _allocator = stick::defaultAllocator());

        Shrub(const stick::String & _name, ValueHint _hint = ValueHint::None, stick::Allocator & _allocator = stick::defaultAllocator());

        Shrub(const stick::String & _name, const stick::String & _value, ValueHint _hint = ValueHint::None, stick::Allocator & _allocator = stick::defaultAllocator());

        /*Shrub(const Shrub & _other);

        Shrub(Shrub && _other);*/

        stick::Maybe<Shrub &> child(const stick::String & _path, char _separator = '.');

        stick::Maybe<const Shrub &> child(const stick::String & _path, char _separator = '.') const;

        template<class C>
        stick::Maybe<Shrub &> find(C _condition)
        {
            if(_condition(*this))
                return *this;

            for(auto & child : m_children)
            {
                auto maybe = child.find(_condition);
                if(maybe) return maybe;
            }

            return stick::Maybe<Shrub &>();
        }

        template<class C>
        stick::Maybe<const Shrub &> find(C _condition) const
        {
            if(_condition(*this))
                return *this;

            for(auto & child : m_children)
            {
                auto maybe = child.find(_condition);
                if(maybe) return maybe;
            }

            return stick::Maybe<const Shrub &>();
        }

        template<class T>
        stick::Maybe<T> maybe(const stick::String & _path, char _separator = '.') const
        {
            const Shrub * desc = resolvePath(_path, _separator);
            if (desc)
                return detail::convert<T>(desc->m_value);
            return stick::Maybe<T>();
        }

        template<class T>
        T maybe(const stick::String & _path, T _orValue) const
        {
            auto m = maybe<T>(_path);
            if (m)
                return *m;
            return _orValue;
        }

        template<class T>
        T get(const stick::String & _path, char _separator = '.') const
        {
            return maybe<T>(_path, _separator).value();
        }

        Shrub & setName(const stick::String & _name);

        Shrub & setValue(const stick::String & _value);

        Shrub & setValueHint(ValueHint _hint);

        template<class T>
        Shrub & set(const stick::String & _path, T _val, char _separator = '.')
        {
            return set(_path, _val, detail::deduceHint<typename std::remove_cv<T>::type>(), _separator);
        }

        template<class T>
        Shrub & set(const stick::String & _path, T _val, ValueHint _hint, char _separator = '.')
        {
            auto it = ensureTree(_path, _separator);
            it->m_value = detail::toString(_val, m_children.allocator());
            it->m_valueHint = _hint;
            return *it;
        }

        Shrub & append(const stick::String & _path, char _separator = '.');

        template<class T>
        Shrub & append(const stick::String & _path, T _val, char _separator = '.')
        {
            return append(_path, _val, detail::deduceHint<std::remove_cv<T>::type>(), _separator);
        }

        template<class T>
        Shrub & append(const stick::String & _path, T _val, ValueHint _hint, char _separator = '.')
        {
            auto it = ensureTree(_path, _separator);
            it->m_children.append(Shrub(stick::String("", m_children.allocator()), detail::toString(_val, m_children.allocator()), _hint, m_children.allocator()));
            return it->m_children.last();
        }

        Shrub & append(const stick::String & _path, const Shrub & _node, char _separator = '.');

        Shrub & append(const Shrub & _child);

        Shrub & append(Shrub && _child);


        template<class T>
        T value() const
        {
            return detail::convert<T>(m_value);
        }

        const stick::String & valueString() const;

        const stick::String & name() const;

        ValueHint valueHint() const;

        Shrub & sort();

        ChildIter begin();

        ChildConstIter begin() const;

        ChildIter end();

        ChildConstIter end() const;

        ReverseChildIter rbegin();

        ReverseChildConstIter rbegin() const;

        ReverseChildIter rend();

        ReverseChildConstIter rend() const;

        stick::Size count() const;

        stick::Allocator & allocator();

        const stick::Allocator & allocator() const;


    private:

        const Shrub * resolvePath(const stick::String & _path, char _separator) const;

        ChildIter ensureTree(const stick::String & _path, char _separator);

        Shrub & appendSibling(const stick::String & _path, char _separator);

        ChildConstIter findByName(const stick::String & _name) const;

        ChildIter findByName(const stick::String & _name);


        stick::String m_name;
        stick::String m_value;
        ValueHint m_valueHint;
        ChildArray m_children;
    };

    typedef stick::Result<Shrub> ShrubResult;

    STICK_API ShrubResult parseJSON(const stick::String & _json, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API ShrubResult loadJSON(const stick::URI & _path, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API stick::TextResult exportJSON(const Shrub & _shrub, bool _bPrettify = false);

    STICK_API ShrubResult parseXML(const stick::String & _xml, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API ShrubResult loadXML(const stick::URI & _path, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API stick::TextResult exportXML(const Shrub & _shrub, bool _bPrettify = false);
}

#endif //SCRUB_SHRUB_HPP
