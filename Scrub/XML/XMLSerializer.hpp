#ifndef SCRUB_XML_XMLSERIALIZER_HPP
#define SCRUB_XML_XMLSERIALIZER_HPP

#include <Scrub/Shrub.hpp>

namespace scrub
{
    namespace xml
    {
        using namespace stick;

        STICK_LOCAL ShrubResult parseXML(const String & _xml, Allocator & _alloc);
        STICK_LOCAL TextResult exportXML(const Shrub & _shrub, bool _bPrettify);
    }
}

#endif //SCRUB_XML_XMLSERIALIZER_HPP
