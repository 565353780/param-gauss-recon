cmake_minimum_required(VERSION 3.10)
project(PGR)

file(GLOB SOURCES
    PGR/src/*.cpp
    cnpy/*.cpp
)

include_directories(
    PGR/include
    cnpy
    CLI11
)

add_executable(PGRExportQuery PGR/PGRExportQuery.cpp ${SOURCES})
add_executable(PGRLoadQuery   PGR/PGRLoadQuery.cpp ${SOURCES})

target_link_libraries(PGRExportQuery z)
target_link_libraries(PGRLoadQuery   z)

set_target_properties(PGRExportQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
set_target_properties(PGRLoadQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/../bin/")
