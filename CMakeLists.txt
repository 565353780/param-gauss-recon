project("ParametricGaussRecon")
cmake_minimum_required(VERSION 3.0)

find_package(ZLIB REQUIRED)

set(CPP_FOLDER ../param-gauss-recon/param_gauss_recon/Cpp)
set(LIB_FOLDER ../param-gauss-recon/param_gauss_recon/Lib)
set(SRC_FOLDER ${CPP_FOLDER}/src)

set(SOURCES
  ${SRC_FOLDER}/Cube.cpp
  ${SRC_FOLDER}/Geometry.cpp
  ${SRC_FOLDER}/MarchingCubes.cpp
  ${SRC_FOLDER}/Mesh.cpp
  ${SRC_FOLDER}/Octnode.cpp
  ${SRC_FOLDER}/Octree.cpp
  ${SRC_FOLDER}/ply.cpp
  ${SRC_FOLDER}/plyfile.cpp
  ${LIB_FOLDER}/cnpy/cnpy.cpp)

include_directories(${CPP_FOLDER}/include)

add_executable(PGRExportQuery ${CPP_FOLDER}/PGRExportQuery.cpp ${SOURCES})
target_compile_features(PGRExportQuery PRIVATE cxx_std_17)
target_include_directories(PGRExportQuery PRIVATE "${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/cnpy/" "${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/CLI11/")
target_link_libraries(PGRExportQuery PRIVATE ZLIB::ZLIB)
set_target_properties(PGRExportQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/apps/")

add_executable(PGRLoadQuery ${CPP_FOLDER}/PGRLoadQuery.cpp ${SOURCES})
target_compile_features(PGRLoadQuery PRIVATE cxx_std_17)
target_include_directories(PGRLoadQuery PRIVATE "${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/cnpy/" "${PROJECT_SOURCE_DIR}/${LIB_FOLDER}/CLI11/")
target_link_libraries(PGRLoadQuery PRIVATE ZLIB::ZLIB)
set_target_properties(PGRLoadQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/apps/")
