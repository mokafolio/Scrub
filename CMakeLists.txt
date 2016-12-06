cmake_minimum_required (VERSION 2.8.11)
project (Scrub)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")

option(BuildSubmodules "BuildSubmodules" OFF)
option(AddTests "AddTests" ON)

if(BuildSubmodules)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Submodules/Stick)
else()
    include_directories (${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include ${CMAKE_INSTALL_PREFIX}/include)
endif()

link_directories(/usr/local/lib)

set (SCRUBDEPS Stick pthread)

set (SCRUBINC 
Scrub/Shrub.hpp
Scrub/JSON/JSONSerializer.hpp
Scrub/JSON/sajson.h
Scrub/XML/XMLSerializer.hpp
Scrub/XML/pugiconfig.hpp
Scrub/XML/pugixml.hpp
)

set (SCRUBSRC 
Scrub/Shrub.cpp
Scrub/JSON/JSONSerializer.cpp
Scrub/XML/XMLSerializer.cpp
Scrub/XML/pugixml.cpp
)

if(BuildSubmodules)
    set(PrevAddTests ${AddTests})
    set(AddTests OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Submodules/Stick)
    set(AddTests ${PrevAddTests})
endif()

add_library (Scrub SHARED ${SCRUBSRC})
target_link_libraries(Scrub ${SCRUBDEPS})
add_library (ScrubStatic STATIC ${SCRUBSRC})
target_link_libraries(ScrubStatic ${SCRUBDEPS})

install (TARGETS Scrub ScrubStatic DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
foreach ( file ${SCRUBINC} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${dir} )
endforeach()

if(AddTests)
    add_subdirectory(Tests)
endif()
