#
# CMake configuration for HAPPEX timing board module parsers
#

# Library containing HAPPEX timing board module parser implementation
add_library(haptb_parser STATIC
    ModuleParser_HAPTB.cc
)

target_include_directories(haptb_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/data_objects
)

# Link against core parser library
target_link_libraries(haptb_parser
    PUBLIC
        core
)

set(HAPTB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data_objects PARENT_SCOPE)

file(GLOB HAPTB_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/data_objects/*.h
)

set(HAPTB_PUBLIC_HEADERS ${HAPTB_PUBLIC_HEADERS} PARENT_SCOPE)


