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
    }
}
