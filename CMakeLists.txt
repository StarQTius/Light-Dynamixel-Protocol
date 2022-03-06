cmake_minimum_required(VERSION 3.21)

project(Light-Dynamixel-Protocol LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tool)
include(${PROJECT_NAME})

include(FetchContent)

FetchContent_Declare(expected
                     GIT_REPOSITORY https://github.com/TartanLlama/expected)

FetchContent_Declare(Unpadded
                     GIT_REPOSITORY https://github.com/StarQTius/Unpadded)

if(DEFINED CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
endif()

add_subdirectory(include)

if(${PROJECT_NAME}_IS_TOP_LEVEL AND BUILD_TESTING)
  include(CTest)
  FetchContent_Declare(Unity
                       GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity)
  add_subdirectory(test)
endif()
