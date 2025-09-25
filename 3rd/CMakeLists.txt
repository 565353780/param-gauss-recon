cmake_minimum_required(VERSION 3.10)
project(PGR)

set(ZLIB_ROOT ${ROOT_FOLDER}/3rd/zlib/build)

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

target_link_libraries(PGRExportQuery PRIVATE z)
target_link_libraries(PGRLoadQuery PRIVATE z)

set_target_properties(PGRExportQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
set_target_properties(PGRLoadQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
