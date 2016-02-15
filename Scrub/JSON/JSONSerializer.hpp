#ifndef SCRUB_JSON_JSONSERIALIZER_HPP
#define SCRUB_JSON_JSONSERIALIZER_HPP

#include <Scrub/Shrub.hpp>

namespace scrub
{
    namespace json
    {
        using namespace stick;

        STICK_LOCAL ShrubResult parseJSON(const String & _json, Allocator & _alloc);
    }
}

#endif //SCRUB_JSON_JSONSERIALIZER_HPP
