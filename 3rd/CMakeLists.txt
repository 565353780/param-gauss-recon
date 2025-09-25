cmake_minimum_required(VERSION 3.10)
project(PGR)

set(ZLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/zlib/build)

file(GLOB SOURCES
    PGR/src/*.cpp
    cnpy/*.cpp
)

include_directories(
    PGR/include
    cnpy
    CLI11
    zlib
)

link_directories(${ZLIB_ROOT})

add_executable(PGRExportQuery PGR/PGRExportQuery.cpp ${SOURCES})
add_executable(PGRLoadQuery   PGR/PGRLoadQuery.cpp ${SOURCES})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(PGRExportQuery PRIVATE z)
  target_link_libraries(PGRLoadQuery PRIVATE z)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(PGRExportQuery PRIVATE zd)
  target_link_libraries(PGRLoadQuery PRIVATE zd)
endif()


set_target_properties(PGRExportQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
set_target_properties(PGRLoadQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
