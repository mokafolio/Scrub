cmake_minimum_required (VERSION 2.8.11)
project (Scrub)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include)
link_directories(/usr/local/lib)

set (SCRUBDEPS Stick jsoncpp)

set (SCRUBINC 
Scrub/Shrub.hpp
Scrub/JSON/JSONSerializer.hpp
)

set (SCRUBSRC 
Scrub/Shrub.cpp
Scrub/JSON/JSONSerializer.cpp
)

add_library (Scrub SHARED ${SCRUBSRC})
target_link_libraries(Scrub ${SCRUBDEPS})
add_library (ScrubStatic STATIC ${SCRUBSRC})
target_link_libraries(ScrubStatic ${SCRUBDEPS})
add_subdirectory (Tests)