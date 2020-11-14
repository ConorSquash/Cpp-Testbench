project (Least-Squares)

# Header only library, therefore INTERFACE
add_library(lsq INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(lsq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)