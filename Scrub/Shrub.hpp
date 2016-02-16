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

namespace scrub
{
    //hints for serializing
    enum class STICK_API ValueHint
    {
        Auto = 0,
        JSONInt,
        JSONUInt,
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

        Shrub(const stick::String & _name, stick::Allocator & _allocator = stick::defaultAllocator());

        Shrub(const stick::String & _name, const stick::String & _value, stick::Allocator & _allocator = stick::defaultAllocator());

        /*Shrub(const Shrub & _other);

        Shrub(Shrub && _other);*/

        stick::Maybe<Shrub &> child(const stick::String & _path, char _separator = '.');

        stick::Maybe<const Shrub &> child(const stick::String & _path, char _separator = '.') const;

        template<class T>
        stick::Maybe<T> get(const stick::String & _path, char _separator = '.') const
        {
            const Shrub * desc = resolvePath(_path, _separator);
            if (desc)
                return detail::convert<T>(desc->m_value);
            return stick::Maybe<T>();
        }

        template<class T>
        T get(const stick::String & _path, T _orValue) const
        {
            auto maybe = get<T>(_path);
            if (maybe)
                return *maybe;
            return _orValue;
        }

        Shrub & setName(const stick::String & _name);

        Shrub & setValue(const stick::String & _value);

        template<class T>
        Shrub & set(const stick::String & _path, T _val, char _separator = '.')
        {
            auto it = ensureTree(_path, _separator);
            it->m_value = stick::toString(_val, m_children.allocator());
            return *this;
        }

        Shrub & set(const stick::String & _path, const char * _val, char _separator = '.');

        template<class T>
        Shrub & append(const stick::String & _path, T _val, char _separator = '.')
        {
            auto it = ensureTree(_path, _separator);
            it->m_children.append(Shrub(stick::String("", m_children.allocator()), stick::toString(_val, m_children.allocator()), m_children.allocator()));
            return *this;
        }

        Shrub & append(const stick::String & _path, const char * _val, char _separator = '.');

        Shrub & append(const Shrub & _child);

        Shrub & append(Shrub && _child);

        const stick::String & value() const;

        const stick::String & name() const;

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

        ChildConstIter findByName(const stick::String & _name) const;

        ChildIter findByName(const stick::String & _name);


        stick::String m_name;
        stick::String m_value;
        ChildArray m_children;
    };

    STICK_RESULT_HOLDER(ShrubResultHolder, shrub);
    typedef stick::Result<Shrub, ShrubResultHolder> ShrubResult;
    STICK_RESULT_HOLDER(TextResultHolder, text);
    typedef stick::Result<stick::String, TextResultHolder> TextResult;

    STICK_API ShrubResult parseJSON(const stick::String & _json, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API ShrubResult loadJSON(const stick::URI & _path, stick::Allocator & _alloc = stick::defaultAllocator());
    STICK_API TextResult exportJSON(const Shrub & _shrub, bool _bPrettify = false);
}

#endif //SCRUB_SHRUB_HPP
