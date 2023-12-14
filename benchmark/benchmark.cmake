
set(BENCHMARK_MAIN ${Q_SOURCE_DIR}/benchmark/benchmark_main.cpp)

file(GLOB BENCHMARK_SRC_FILES "benchmark/*.cpp")
include_directories(${PROJECT_SRC})

add_executable(benchmark ${BENCHMARK_MAIN} ${BENCHMARK_SRC_FILES})
target_link_libraries(benchmark ${Q_LIBRARY})

