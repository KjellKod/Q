# =============================================================================
# 2018 by KjellKod This is PUBLIC DOMAIN to use at your own risk and comes
# with no warranties. This code is yours to share, use and modify with no
# strings attached and no restrictions or obligations.
#
# For more information see g3log/LICENSE or refer refer to http://unlicense.org
# ==============================================================================

cmake_minimum_required (VERSION 3.5)
enable_language(CXX)

PROJECT (Q)

set(MAJOR_VERSION 1)
if ( NOT VERSION )
   execute_process(COMMAND bash "-c" "git rev-list --branches HEAD | wc -l | tr -d ' ' | tr -d '\n'" OUTPUT_VARIABLE GIT_VERSION WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

set(MINOR_VERSION 1)
math(EXPR VERSION-BASE ${GIT_VERSION}/255)
math(EXPR VERSION-REMAINDER ${GIT_VERSION}%255)
message( STATUS "git build version: ${GIT_VERSION}" )
message( STATUS "version base: ${VERSION-BASE}" )
message( STATUS "version remainder: ${VERSION-REMAINDER}" )
set(BUILD_NUMBER ${VERSION-BASE})
set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}-${VERSION-REMAINDER})
message( STATUS "Software Version: ${VERSION}" )


SET(PROJECT_SRC ${Q_SOURCE_DIR}/src)
SET(DIR_UNIT_TEST ${Q_SOURCE_DIR}/test)
SET(DIR_3RDPARTY ${Q_SOURCE_DIR}/3rdparty)

message("source directory: ${PROJECT_SRC}")

include_directories(${SRC})
# Set with "cmake -DSYSTEM_INCLUDE=...."
if(SYSTEM_INCLUDE)
   include_directories(${SYSTEM_INCLUDE})
endif()

# change to c++20 in version 2.0
if (MSVC)
else()
   set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -Wall -Wunused -stdlib=libc++ -std=c++20  -pthread -D_GLIBCXX_USE_NANOSLEEP")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   add_compile_options("-stdlib=libc++")
endif()



file(GLOB SRC_FILES ${PROJECT_SRC}/g3log/*.h ${PROJECT_SRC}/q/*.hpp ${PROJECT_SRC}/*.cpp)
file(GLOB HEADER_FILES ${PROJECT_SRC}/q/*.hpp)

message("source files: ${SRC_FILES}")

set(Q_LIBRARY qlib)
add_library(${Q_LIBRARY} SHARED ${SRC_FILES})
set(${Q_LIBRARY}_VERSION_STRING ${VERSION})
message( STATUS "Creating ${Q_LIBRARY} VERSION: ${VERSION}" )
set_target_properties(${Q_LIBRARY} PROPERTIES LINKER_LANGUAGE CXX SOVERSION ${VERSION})



# ==========================================================================
# UNIT TEST OPTIONS:
# ============================================================================
INCLUDE (${Q_SOURCE_DIR}/test/test.cmake)
# ==========================================================================
# EXAMPLES
# ============================================================================
INCLUDE (${Q_SOURCE_DIR}/examples/examples.cmake)
