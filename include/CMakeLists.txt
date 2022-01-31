add_library(LDP INTERFACE)
target_include_directories(LDP INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(LDP INTERFACE Unpadded)
