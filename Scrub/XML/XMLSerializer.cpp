#include <Scrub/XML/XMLSerializer.hpp>
#include <Scrub/XML/pugixml.hpp>

namespace scrub
{
    namespace xml
    {
        static void parseXMLNode(pugi::xml_node _node, Shrub & _shrub)
        {
            //get the nodes text node (if any)
            pugi::xml_text text = _node.text();
            _shrub.setName(_node.name());
            _shrub.setValue(text.get());

            //add the nodes attributes as children
            for (pugi::xml_attribute_iterator ait = _node.attributes_begin(); ait != _node.attributes_end(); ++ait)
            {
                _shrub.append(Shrub((*ait).name(), (*ait).value()));
            }

            for (pugi::xml_node xmlchild = _node.first_child(); xmlchild; xmlchild = xmlchild.next_sibling())
            {
                Shrub child;
                parseXMLNode(xmlchild, child);
                _shrub.append(child);
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
                parseXMLNode(doc.first_child(), ret);
                return ret;
            }
            else
            {
                return Error(ec::ParseFailed, String::concat("Failed to parse XML: ", result.description()), STICK_FILE, STICK_LINE);
            }
        }

        TextResult exportXML(const Shrub & _shrub, bool _bPrettify)
        {

        }
    }
}