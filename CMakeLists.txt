cmake_minimum_required(VERSION 3.0)

message("Finding ccache...")
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message(STATUS "ccache found")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
else()
  message(WARNING "ccache not found")
endif()

include(FetchContent)

project(Light-Dynamixel-Protocol
  VERSION 0.0
  LANGUAGES CXX)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

message("Making Unpadded available...")
FetchContent_Declare(Unpadded
  GIT_REPOSITORY https://github.com/StarQTius/Unpadded)
FetchContent_MakeAvailable(Unpadded)

add_subdirectory(include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  enable_testing()
  if(BUILD_TESTING)
    message("Making Unity available...")
    FetchContent_Declare(Unity
      GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity)
    FetchContent_MakeAvailable(Unity)

    message("Building test")
    add_subdirectory(test)
  endif()
endif()
