add_executable(run_ut_packet packet.cpp)
set_target_properties(run_ut_packet PROPERTIES CXX_STANDARD 11)
target_compile_options(
  run_ut_packet
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:
    /W4
    /WX>
    $<$<CXX_COMPILER_ID:GNU>:
    -fdiagnostics-color=always>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
    -fcolor-diagnostics>)

target_link_libraries(run_ut_packet PRIVATE unity::framework LDP)
add_test(NAME ut_packet COMMAND run_ut_packet)
