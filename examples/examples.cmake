set(SPSC_EXAMPLE_SRC ${Q_SOURCE_DIR}/examples/spsc_main.cpp)
add_executable(example_spsc ${SPSC_EXAMPLE_SRC})
target_link_libraries(example_spsc ${Q_LIBRARY})

set(NOTIFY_EXAMPLE_SRC ${Q_SOURCE_DIR}/examples/thread_notify_main.cpp)
add_executable(example_thread_notify ${NOTIFY_EXAMPLE_SRC})
target_link_libraries(example_thread_notify ${Q_LIBRARY})