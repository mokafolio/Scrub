#include <Scrub/JSON/JSONSerializer.hpp>
#include <Scrub/JSON/sajson.h>

namespace scrub
{
    namespace json
    {
        static String JSONValueToString(const sajson::value & _val, Allocator & _alloc)
        {
            if (_val.get_type() == sajson::TYPE_STRING)
            {
                return String(_val.as_string().c_str(), _alloc);
            }
            else if (_val.get_type() == sajson::TYPE_TRUE)
            {
                return String("true", _alloc);
            }
            else if (_val.get_type() == sajson::TYPE_FALSE)
            {
                return String("false", _alloc);
            }
            else if (_val.get_type() == sajson::TYPE_INTEGER)
            {
                return toString(_val.get_integer_value(), _alloc);
            }
            else if (_val.get_type() == sajson::TYPE_DOUBLE)
            {
                return toString(_val.get_double_value(), _alloc);
            }

            return String("", _alloc);
        }

        static void parseJSONObject(const sajson::value & _node, Shrub & _treeNode);

        static void parseJSONNode(const String & _name, const sajson::value & _node, Shrub & _treeNode)
        {
            Shrub child(_name, JSONValueToString(_node, _treeNode.allocator()), _treeNode.allocator());
            if (_node.get_type() == sajson::TYPE_OBJECT)
            {
                parseJSONObject(_node, child);
            }
            else if (_node.get_type() == sajson::TYPE_ARRAY)
            {

                for (Size i = 0; i < _node.get_length(); ++i)
                {
                    parseJSONNode("", _node.get_array_element(i), child);
                }
            }
            _treeNode.append(move(child));
        }

        static void parseJSONObject(const sajson::value & _node, Shrub & _treeNode)
        {
            STICK_ASSERT(_node.get_type() == sajson::TYPE_OBJECT);
            for (Size i = 0; i < _node.get_length(); ++i)
            {
                const sajson::string & str = _node.get_object_key(i);
                parseJSONNode(String(str.data(), str.data() + str.length()), _node.get_object_value(i), _treeNode);
            }
        }

        ShrubResult parseJSON(const String & _json, Allocator & _alloc)
        {
            const sajson::document & document = sajson::parse(sajson::literal(_json.cString()));
            if (!document.is_valid())
            {
                return Error(ec::ParseFailed, String::concat("Failed to parse JSON: ", document.get_error_message().c_str()), STICK_FILE, STICK_LINE);
            }
            const sajson::value & root = document.get_root();
            Shrub ret(_alloc);
            parseJSONObject(root, ret);
            return ret;
        }

        static bool isObject(const Shrub & _child)
        {
            for (const auto & child : _child)
            {
                if (child.name().length())
                    return true;
            }
            return false;
        }

        static void indent(String & _out, UInt32 _count)
        {
            for (UInt32 i = 0; i < _count; ++i)
                _out.append("    ");
        }

        static Error exportChild(const Shrub & _child, bool _bIsPartOfArray, String & _out, bool _bIsLastChild, bool _bPrettify, UInt32 _indentation)
        {
            if (_bPrettify)
                indent(_out, _indentation);

            if (_child.count())
            {
                //this is an array
                if (!isObject(_child))
                {
                    if (!_bIsPartOfArray)
                    {
                        _out.append("\"", _child.name(), "\" : [");
                        if (_bPrettify) _out.append("\n");
                    }
                    else
                    {
                        _out.append("[");
                        if (_bPrettify) _out.append("\n");
                    }
                    Size i = 0;
                    for (const Shrub & child : _child)
                    {
                        exportChild(child, true, _out, i == _child.count() - 1, _bPrettify, _indentation + 1);
                        ++i;
                    }
                    if (_bPrettify)
                        indent(_out, _indentation);

                    if (!_bIsLastChild)
                        _out.append("],");
                    else
                        _out.append("]");
                    if (_bPrettify) _out.append("\n");
                }
                else
                {
                    if (!_bIsPartOfArray)
                        _out.append("\"", _child.name(), "\" : {");
                    else
                        _out.append("{");
                    if (_bPrettify) _out.append("\n");
                    Size i = 0;
                    for (const Shrub & child : _child)
                    {
                        exportChild(child, false, _out, i == _child.count() - 1, _bPrettify, _indentation + 1);
                        ++i;
                    }

                    if (_bPrettify)
                        indent(_out, _indentation);
                    
                    if (!_bIsLastChild)
                        _out.append("},");
                    else
                        _out.append("}");
                    if (_bPrettify) _out.append("\n");
                }
            }
            else
            {
                if (!_bIsPartOfArray)
                    _out.append("\"", _child.name(), "\" : \"", _child.value(), "\"");
                else
                    _out.append("\"", _child.value(), "\"");
                if (!_bIsLastChild)
                    _out.append(",");
                if (_bPrettify) _out.append("\n");
            }

            return Error();
        }

        TextResult exportJSON(const Shrub & _shrub, bool _bPrettify)
        {
            String ret(const_cast<Allocator &>(_shrub.allocator()));
            ret.reserve(2048); //just a random guess for now
            exportChild(_shrub, true, ret, true, _bPrettify, 0);
            return ret;
        }
    }
}
