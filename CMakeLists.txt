cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "-std=c++17 -O0")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/std:c++17 /O0")
endif()

project("ParametricGaussRecon")

set(Open3D_ROOT ../param-gauss-recon/3rd/open3d-devel-linux)
set(ROOT_FOLDER ../param-gauss-recon)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(D_GLIBCXX_USE_CXX11_ABI 0)

list(APPEND CMAKE_PREFIX_PATH ${Open3D_ROOT})

find_package(ZLIB REQUIRED)

file(GLOB_RECURSE SRC ${ROOT_FOLDER}/src/*.cpp)
list(FILTER SRC EXCLUDE REGEX "main.cpp")

set(PGR_FOLDER ${ROOT_FOLDER}/3rd/PGR)
file(GLOB_RECURSE PGR_SRC ${PGR_FOLDER}/src/*.cpp)

set(CNPY_SRC ${ROOT_FOLDER}/3rd/cnpy/cnpy.cpp)

include_directories(${ROOT_FOLDER}/include)
include_directories(${PGR_FOLDER}/include)
include_directories(${ROOT_FOLDER}/3rd/nanoflann)

add_executable(PGRExportQuery ${PGR_FOLDER}/PGRExportQuery.cpp ${PGR_SRC} ${CNPY_SRC})
target_compile_features(PGRExportQuery PRIVATE cxx_std_17)
target_include_directories(PGRExportQuery PRIVATE "${ROOT_FOLDER}/3rd/cnpy/" "${ROOT_FOLDER}/3rd/CLI11/")
target_link_libraries(PGRExportQuery PRIVATE ZLIB::ZLIB)
set_target_properties(PGRExportQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

add_executable(PGRLoadQuery ${PGR_FOLDER}/PGRLoadQuery.cpp ${PGR_SRC} ${CNPY_SRC})
target_compile_features(PGRLoadQuery PRIVATE cxx_std_17)
target_include_directories(PGRLoadQuery PRIVATE "${ROOT_FOLDER}/3rd/cnpy/" "${ROOT_FOLDER}/3rd/CLI11/")
target_link_libraries(PGRLoadQuery PRIVATE ZLIB::ZLIB)
set_target_properties(PGRLoadQuery PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

find_package(Eigen3 REQUIRED NO_MODULE)
if (EIGEN3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIR})
endif()

find_package(Open3D REQUIRED)
if (Open3D_FOUND)
  message(STATUS "Found Open3D ${Open3D_VERSION}")
  link_directories(${Open3D_LIBRARY_DIRS})
  include_directories(${Open3D_INCLUDE_DIRS})
endif()

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

add_executable(${PROJECT_NAME} ./src/main.cpp ${SRC} ${CNPY_SRC})

find_library(TORCH_PYTHON_LIBRARY torch_python PATH "${TORCH_INSTALL_PREFIX}/lib")

target_link_libraries(${PROJECT_NAME} PRIVATE Open3D::Open3D)

target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES} ${TORCH_PYTHON_LIBRARY})
if(TORCH_CUDA)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_CUDA_LIBRARIES})
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${TORCH_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)

target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_FOLDER}/3rd/cnpy/")
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
