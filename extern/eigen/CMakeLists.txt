project (Eigen)

# Header only library, therefore INTERFACE
add_library(eigen INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(eigen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/eigen)