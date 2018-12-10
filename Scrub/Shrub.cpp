#include <Stick/FileUtilities.hpp>
#include <Scrub/JSON/JSONSerializer.hpp>
#include <Scrub/XML/XMLSerializer.hpp>
#include <algorithm> //for std::sort

namespace scrub
{
    using namespace stick;

    Shrub::Shrub(Allocator & _allocator) :
        m_name(_allocator),
        m_value(_allocator),
        m_valueHint(ValueHint::None),
        m_children(_allocator)
    {

    }

    Shrub::Shrub(const String & _name, ValueHint _hint, Allocator & _allocator) :
        m_name(_name),
        m_value(_allocator),
        m_valueHint(_hint),
        m_children(_allocator)
    {

    }

    Shrub::Shrub(const String & _name, const String & _value, ValueHint _hint, Allocator & _allocator) :
        m_name(_name),
        m_value(_value),
        m_valueHint(_hint),
        m_children(_allocator)
    {

    }

    Maybe<Shrub &> Shrub::child(const String & _path, char _separator)
    {
        Shrub * desc = const_cast<Shrub *>(resolvePath(_path, _separator));
        if (desc)
            return *desc;
        return Maybe<Shrub &>();
    }

    Maybe<const Shrub &> Shrub::child(const String & _path, char _separator) const
    {
        const Shrub * desc = resolvePath(_path, _separator);
        if (desc)
            return *desc;
        return Maybe<const Shrub &>();
    }

    Shrub & Shrub::setName(const String & _name)
    {
        m_name = _name;
        return *this;
    }

    Shrub & Shrub::setValue(const String & _value)
    {
        m_value = _value;
        return *this;
    }

    Shrub & Shrub::setValueHint(ValueHint _hint)
    {
        m_valueHint = _hint;
        return *this;
    }

    /*
    Shrub & Shrub::set(const String & _path, const char * _val, char _separator)
    {
        auto it = ensureTree(_path, _separator);
        it->m_value = _val;
        it->m_valueHint = ValueHint::JSONString;
        return *it;
    }

    Shrub & Shrub::set(const stick::String & _path, const char * _val, ValueHint _hint, char _separator)
    {
        auto it = ensureTree(_path, _separator);
        it->m_value = _val;
        it->m_valueHint = _hint;
        return *it;
    }

    Shrub & Shrub::append(const String & _path, const char * _val, char _separator)
    {
        auto it = ensureTree(_path, _separator);
        it->m_children.append(Shrub(String("", m_children.allocator()), String(_val, m_children.allocator()), ValueHint::None, m_children.allocator()));
        return it->m_children.last();
    }*/

    Shrub & Shrub::append(const stick::String & _path, char _separator)
    {
        return appendSibling(_path, _separator);
    }

    Shrub & Shrub::append(const stick::String & _path, const Shrub & _node, char _separator)
    {
        Shrub & sibling = appendSibling(_path, _separator);
        return sibling.append(_node);
    }

    Shrub & Shrub::append(const Shrub & _child)
    {
        m_children.append(_child);
        return m_children.last();
    }

    Shrub & Shrub::append(Shrub && _child)
    {
        m_children.append(std::move(_child));
        return m_children.last();
    }

    const Shrub * Shrub::resolvePath(const String & _path, char _separator) const
    {
        auto segments = path::segments(_path, const_cast<Allocator &>(m_children.allocator()), _separator);
        Size segment = 0;
        const Shrub * ret = this;
        while (true)
        {
            auto it = ret->findByName(segments[segment]);
            if (it != ret->m_children.end())
            {
                ret = &(*it);
                if (segment == segments.count() - 1)
                {
                    return ret;
                }
                segment++;
            }
            else
            {
                break;
            }
        }
        return nullptr;
    }

    Shrub::ChildIter Shrub::ensureTree(const String & _path, char _separator)
    {
        auto segments = path::segments(_path, const_cast<Allocator &>(m_children.allocator()), _separator);
        Size segment = 0;
        Shrub * ret = this;
        while (true)
        {
            auto it = ret->findByName(segments[segment]);
            if (it == ret->m_children.end())
            {
                ret->m_children.append(Shrub(segments[segment], ValueHint::None, const_cast<Allocator &>(m_children.allocator())));
                it = ret->m_children.begin() + ret->m_children.count() - 1;
                ret = &(*it);
            }
            if (segment == segments.count() - 1)
            {
                return it;
            }
            ret = &(*it);
            segment++;
        }
        STICK_ASSERT(false);
    }

    Shrub & Shrub::appendSibling(const String & _path, char _separator)
    {
        auto idx = _path.rfindIndex(_separator);
        if(idx != String::InvalidIndex)
        {
            auto it = ensureTree(_path.sub(0, idx), _separator);
            return (*it).append(Shrub(_path.sub(idx + 1)));
        }
        else
        {
            return append(Shrub(_path));
        }
    }

    Shrub::ChildConstIter Shrub::findByName(const String & _name) const
    {
        return findIf(m_children.begin(), m_children.end(), [this, _name](const Shrub & _child) { return _child.m_name == _name; });
    }

    Shrub::ChildIter Shrub::findByName(const String & _name)
    {
        return findIf(m_children.begin(), m_children.end(), [this, _name](const Shrub & _child) { return _child.m_name == _name; });
    }

    const String & Shrub::valueString() const
    {
        return m_value;
    }

    const String & Shrub::name() const
    {
        return m_name;
    }

    ValueHint Shrub::valueHint() const
    {
        return m_valueHint;
    }

    Shrub & Shrub::sort()
    {
        std::sort(m_children.begin(), m_children.end(), [this](const Shrub & _a, const Shrub & _b) { return _a.m_name < _b.m_name; });
        return *this;
    }

    Shrub::ChildIter Shrub::begin()
    {
        return m_children.begin();
    }

    Shrub::ChildConstIter Shrub::begin() const
    {
        return m_children.begin();
    }

    Shrub::ChildIter Shrub::end()
    {
        return m_children.end();
    }

    Shrub::ChildConstIter Shrub::end() const
    {
        return m_children.end();
    }

    Shrub::ReverseChildIter Shrub::rbegin()
    {
        return m_children.rbegin();
    }

    Shrub::ReverseChildConstIter Shrub::rbegin() const
    {
        return m_children.rbegin();
    }

    Shrub::ReverseChildIter Shrub::rend()
    {
        return m_children.rend();
    }

    Shrub::ReverseChildConstIter Shrub::rend() const
    {
        return m_children.rend();
    }

    Size Shrub::count() const
    {
        return m_children.count();
    }

    Allocator & Shrub::allocator()
    {
        return m_children.allocator();
    }

    const Allocator & Shrub::allocator() const
    {
        return m_children.allocator();
    }

    ShrubResult parseJSON(const String & _json, Allocator & _alloc)
    {
        return json::parseJSON(_json, _alloc);
    }

    ShrubResult loadJSON(const String & _path, Allocator & _alloc)
    {
        auto result = loadTextFile(_path, _alloc);
        if (result)
        {
            return parseJSON(result.get(), _alloc);
        }
        return result.error();
    }

    TextResult exportJSON(const Shrub & _shrub, bool _bPrettify)
    {
        return json::exportJSON(_shrub, _bPrettify);
    }

    ShrubResult parseXML(const String & _xml, Allocator & _alloc)
    {
        return xml::parseXML(_xml, _alloc);
    }

    ShrubResult loadXML(const String & _path, Allocator & _alloc)
    {
        auto result = loadTextFile(_path, _alloc);
        if (result)
        {
            return parseXML(result.get(), _alloc);
        }
        return result.error();
    }

    TextResult exportXML(const Shrub & _shrub, bool _bPrettify)
    {
        return xml::exportXML(_shrub, _bPrettify);
    }
}
