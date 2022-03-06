FetchContent_MakeAvailable(Unity)

add_library(unit_testing INTERFACE)
target_compile_features(unit_testing INTERFACE cxx_std_11)
target_compile_options(
  unit_testing
  INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Werror>
    $<$<CXX_COMPILER_ID:GNU>:
    -fdiagnostics-color=always>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
    -fcolor-diagnostics>)
target_link_libraries(unit_testing INTERFACE unity::framework LDP)

add_executable(run_packet packet.cpp)
target_link_libraries(run_packet PRIVATE unit_testing)
add_test(NAME packet COMMAND run_packet)
