cmake_minimum_required(VERSION 3.10)
project(ParallelHeatPropagation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Add source files
add_executable(parallel_heat_propagation src/main.cpp src/grid.cpp)

# Find and link OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(parallel_heat_propagation PUBLIC OpenMP::OpenMP_CXX)
endif()
