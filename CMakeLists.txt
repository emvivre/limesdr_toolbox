cmake_minimum_required(VERSION 2.8.0)

# Project
project (limesdr_dump)
project (limesdr_send)
project (limesdr_stopchannel)
project (limesdr_forward)

# We have some custom .cmake scripts no in the offical distribution.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    add_definitions(-Wall)
    add_definitions(-Werror)
    add_definitions(-g)
    add_definitions(-O2)
endif()

find_package(LimeSuite)

if(NOT LIB_LIMESUITE_FOUND)
    message(FATAL_ERROR "LimeSuite required to compile")
endif()

# Finish configuring compiler/linker settings & flags
include_directories(
    ${LIB_LIMESUITE_INCLUDE_DIRS}
)

link_directories(
    ${LIB_LIMESUITE_LIBRARIES}
)

message(" include dir = ${LIB_LIMESUITE_INCLUDE_DIRS}")
message(" link dir = ${LIB_LIMESUITE_LIBRARIES}")
# build program
add_executable(limesdr_dump limesdr_dump.c limesdr_util.c)
add_executable(limesdr_send limesdr_send.c limesdr_util.c)
add_executable(limesdr_stopchannel limesdr_stopchannel.c)
add_executable(limesdr_forward limesdr_forward.c limesdr_util.c)

target_link_libraries( limesdr_dump
    LINK_PUBLIC
    ${LIB_LIMESUITE_LIBRARIES}
)
target_link_libraries( limesdr_send
    LINK_PUBLIC
    ${LIB_LIMESUITE_LIBRARIES}
)
target_link_libraries( limesdr_stopchannel
    LINK_PUBLIC
    ${LIB_LIMESUITE_LIBRARIES}
)
target_link_libraries( limesdr_forward
    LINK_PUBLIC
    ${LIB_LIMESUITE_LIBRARIES}
)
