FetchContent_MakeAvailable(expected Unpadded)

set(LDP_HEADERS packet.hpp detail/def.hpp detail/undef.hpp)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
                           INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE expected Unpadded)

if(DEFINED CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
  foreach(HEADER IN LISTS LDP_HEADERS)
    add_iwyu_target(ldp/${HEADER} ${PROJECT_NAME})
  endforeach()
endif()
