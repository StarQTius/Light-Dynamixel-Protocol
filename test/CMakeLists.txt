add_executable(run_ut_frame
  frame.cpp)
set_target_properties(run_ut_frame PROPERTIES CXX_STANDARD 11)
target_compile_options(run_ut_frame PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Werror>
  $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
  $<$<CXX_COMPILER_ID:GNU>: -fdiagnostics-color=always>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>: -fcolor-diagnostics>)

target_link_libraries(run_ut_frame PRIVATE unity::framework LDP)
add_test(ut_frame COMMAND run_ut_frame)
