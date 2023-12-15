
set(BENCHMARK_MAIN ${Q_SOURCE_DIR}/benchmark/benchmark_main.cpp)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (default: Release)" FORCE)
endif()

# Set compiler flags for high performance
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()


file(GLOB BENCHMARK_SRC_FILES "benchmark/*.cpp")
include_directories(${PROJECT_SRC})

add_executable(benchmark ${BENCHMARK_MAIN} ${BENCHMARK_SRC_FILES})
target_link_libraries(benchmark ${Q_LIBRARY})

