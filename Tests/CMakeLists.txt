add_executable (ScrubTests ScrubTests.cpp)
target_link_libraries(ScrubTests Scrub ${SCRUBDEPS})
add_custom_target(check COMMAND ScrubTests)
