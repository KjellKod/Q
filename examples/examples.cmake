# examples/examples.cmake
set(MAIN_SRC ${Q_SOURCE_DIR}/examples/spsc_main.cpp)
add_executable(SPSMain ${MAIN_SRC})
target_link_libraries(SPSMain ${Q_LIBRARY})

