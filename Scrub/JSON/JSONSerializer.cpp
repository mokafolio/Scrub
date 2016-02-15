#include <Scrub/JSON/JSONSerializer.hpp>
#include <json/json.h>

namespace scrub
{
    namespace json
    {
        static String JSONValueToString(const Json::Value & _val, Allocator & _alloc)
        {
            if (_val.isString())
            {
                return String(_val.asString().c_str(), _alloc);
            }
            else if (_val.isBool())
            {
                if (_val.asBool() == true)
                    return String("true", _alloc);
                else
                    return String("false", _alloc);
            }
            else if (_val.isInt())
            {
                return toString(_val.asInt(), _alloc);
            }
            else if (_val.isUInt())
            {
                return toString(_val.asUInt(), _alloc);
            }
            else if (_val.isDouble())
            {
                return toString(_val.asDouble(), _alloc);
            }

            return String("", _alloc);
        }

        static void parseJSONObject(const Json::Value & _node, Shrub & _treeNode);

        static void parseJSONNode(const char * _name, const Json::Value & _node, Shrub & _treeNode)
        {
            Shrub child(_name, JSONValueToString(_node, _treeNode.allocator()), _treeNode.allocator());
            if (_node.isObject())
            {
                parseJSONObject(_node, child);
            }
            else
            {
                for (Size i = 0; i < _node.size(); ++i)
                {
                    parseJSONNode("", _node.get((UInt32)i, Json::Value()), child);
                }
            }
            _treeNode.append(move(child));
        }

        static void parseJSONObject(const Json::Value & _node, Shrub & _treeNode)
        {
            for (Size i = 0; i < _node.getMemberNames().size(); ++i)
            {
                parseJSONNode(_node.getMemberNames()[i].c_str(), _node.get(_node.getMemberNames()[i], Json::Value()), _treeNode);
            }
        }

        ShrubResult parseJSON(const String & _json, Allocator & _alloc)
        {
            Json::Value root;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(_json.cString(), root);
            if (!parsingSuccessful)
            {
                return Error(ec::ParseFailed, String::concat("Failed to parse JSON: ", reader.getFormatedErrorMessages().c_str()), STICK_FILE, STICK_LINE);
            }

            Shrub ret(_alloc);
            parseJSONObject(root, ret);
            return ret;
        }
    }
}
