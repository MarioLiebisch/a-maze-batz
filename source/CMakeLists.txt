find_package(SFML 2.5 COMPONENTS audio graphics main)

set(SOURCES main.cpp game.hpp game.cpp icon.cpp)
if (WIN32)
list(APPEND SOURCES resources.rc)
endif()

add_executable(a-maze-batz ${SOURCES})

set_target_properties(a-maze-batz PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
target_link_libraries(a-maze-batz sfml-graphics sfml-audio sfml-main)

if(MSVC)
set_target_properties(a-maze-batz PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

install(TARGETS a-maze-batz DESTINATION .)
