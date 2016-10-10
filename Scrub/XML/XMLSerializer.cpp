#include <Scrub/XML/XMLSerializer.hpp>
#include <Scrub/XML/pugixml.hpp>

namespace scrub
{
    namespace xml
    {
        static void parseXMLNode(pugi::xml_node _node, Shrub & _shrub, Allocator & _alloc)
        {
            _shrub.setName(_node.name());
            _shrub.setValueHint(ValueHint::None);

            //add the nodes attributes as children
            for (pugi::xml_attribute_iterator ait = _node.attributes_begin(); ait != _node.attributes_end(); ++ait)
            {
                _shrub.append(Shrub((*ait).name(), (*ait).value(), ValueHint::XMLAttribute, _alloc));
            }

            for (pugi::xml_node xmlchild = _node.first_child(); xmlchild; xmlchild = xmlchild.next_sibling())
            {
                if (xmlchild.type() == pugi::node_element || (xmlchild.type() == pugi::node_pcdata && _shrub.valueString().length()))
                {
                    Shrub child(_alloc);
                    parseXMLNode(xmlchild, child, _alloc);
                    _shrub.append(child);
                }
                else if (xmlchild.type() == pugi::node_pcdata)
                {
                    _shrub.setValue(xmlchild.value());
                }
            }
        }

        ShrubResult parseXML(const String & _xml, Allocator & _alloc)
        {
            //use pugi xml to parse the xml
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load(_xml.cString());

            if (result)
            {
                //recursively parse the DOM
                Shrub ret;
                parseXMLNode(doc.first_child(), ret, _alloc);
                return ret;
            }
            else
            {
                return Error(ec::ParseFailed, String::concat("Failed to parse XML: ", result.description()), STICK_FILE, STICK_LINE);
            }
        }

        static void createXMLNode(pugi::xml_node _parent, const Shrub & _shrub)
        {
            pugi::xml_node child = _parent.append_child();

            if (_shrub.name().length())
            {
                child.set_name(_shrub.name().cString());
            }
            else if (_parent)
            {
                child.set_name(String::concat(_parent.name(), "Child").cString());
            }
            else
            {
                child.set_name("");
            }

            if (_shrub.valueString().length())
            {
                pugi::xml_node txt = child.append_child(pugi::node_pcdata);
                txt.set_value(_shrub.valueString().cString());
            }
            for (const auto & c : _shrub)
            {
                if (c.valueHint() == ValueHint::XMLAttribute)
                {
                    child.append_attribute(c.name().cString()) = c.valueString().cString();
                }
                else
                {
                    createXMLNode(child, c);
                }
            }
        }

        struct XMLStringWriter: pugi::xml_writer
        {
            XMLStringWriter(Allocator & _alloc) :
                result(_alloc)
            {

            }

            void write(const void * data, size_t size) override
            {
                result.append(static_cast<const char *>(data), size);
            }

            String result;

        };

        TextResult exportXML(const Shrub & _shrub, bool _bPrettify)
        {
            pugi::xml_document doc;
            createXMLNode(doc, _shrub);
            XMLStringWriter writer(const_cast<Allocator &>(_shrub.allocator()));
            doc.save(writer, "    ", pugi::format_default, pugi::encoding_utf8);
            return writer.result;
        }
    }
}