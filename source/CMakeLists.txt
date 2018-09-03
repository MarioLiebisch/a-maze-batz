if(WIN32)
find_package(SFML 2.5 COMPONENTS audio graphics main)
else()
find_package(SFML 2.5 COMPONENTS audio graphics)
endif()

set(SOURCES main.cpp game.hpp game.cpp icon.cpp)
if (WIN32)
list(APPEND SOURCES resources.rc)
endif()

add_executable(a-maze-batz ${SOURCES})

set_target_properties(a-maze-batz PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(WIN32)
target_link_libraries(a-maze-batz sfml-graphics sfml-audio sfml-main)
else()
target_link_libraries(a-maze-batz sfml-graphics sfml-audio)
endif()

if(MSVC)
set_target_properties(a-maze-batz PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

install(TARGETS a-maze-batz DESTINATION .)
