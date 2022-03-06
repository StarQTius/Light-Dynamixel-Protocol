FetchContent_MakeAvailable(expected Unpadded)

set(LIGHT_DYNAMIXEL_PROTOCOL_HEADERS packet.hpp detail/def.hpp detail/undef.hpp)

add_library(LDP INTERFACE)
target_include_directories(LDP INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(LDP INTERFACE expected Unpadded)

if(DEFINED CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
  foreach(HEADER IN LISTS LIGHT_DYNAMIXEL_PROTOCOL_HEADERS)
    add_iwyu_target(dxl/${HEADER} LDP)
  endforeach()
endif()
