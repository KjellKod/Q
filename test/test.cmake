# =====================================================
#  Public Domain: Inspired by github.com/KjellKod/g3log/CMakeLists.cmake
#  2015, April 30, @author Kjell.Hedstrom
# =====================================================

# ref: https://google.github.io/googletest/quickstart-cmake.html
include(FetchContent)
FetchContent_Declare(
   googletest
   URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SRC_FILES "test/*.cpp")
message("test files: ${TEST_SRC_FILES}")
include_directories(${PROJECT_SRC})
set(TestRunner UnitTestRunner)

enable_testing()
add_executable(UnitTestRunner 
   ${TEST_SRC_FILES}
)

target_link_libraries(
   UnitTestRunner 
   ${Q_LIBRARY}
   GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(UnitTestRunner)

# set(GTEST_DIR ${DIR_3RDPARTY}/gtest-1.7.0)
# set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
# message( "Attempt to build gtest. gtest directory: " ${GTEST_DIR})
# include_directories(${GTEST_INCLUDE_DIRECTORIES})
# add_library(gtest_170_lib ${GTEST_DIR}/src/gtest-all.cc)
# set_target_properties(gtest_170_lib PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
# enable_testing(true)

# include_directories(${DIR_UNIT_TEST})
# include_directories(${PROJECT_SRC})
# if(SYSTEM_INCLUDE)
#    include_directories(${SYSTEM_INCLUDE})
# endif()

# file(GLOB TEST_SRC_FILES "test/*.cpp")
# message("test files: ${TEST_SRC_FILES}")
# set(TestRunner UnitTestRunner)


# # build the unit tests   
# set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wunused -std=c++14 -pthread -D_GLIBCXX_USE_NANOSLEEP")
# add_executable(${TestRunner} ${DIR_3RDPARTY}/test_main.cpp ${TEST_SRC_FILES} )
# set_target_properties(${TestRunner} PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
# set_target_properties(${TestRunner} PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
# set_target_properties(${TestRunner} PROPERTIES COMPILE_FLAGS "-isystem -pthread ")
# target_link_libraries(${TestRunner} ${Q_LIBRARY} gtest_170_lib -lstdc++)