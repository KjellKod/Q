# examples/examples.cmake
set(MAIN_SRC ${Q_SOURCE_DIR}/examples/spsc_main.cpp)
add_executable(example_spsc ${MAIN_SRC})
target_link_libraries(example_spsc ${Q_LIBRARY})

