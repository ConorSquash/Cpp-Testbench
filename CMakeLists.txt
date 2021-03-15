cmake_minimum_required(VERSION 3.14.0)

project(Anser)


# External Libraries
add_subdirectory(extern/catch)
add_subdirectory(extern/eigen)
add_subdirectory(extern/least-squares)
add_subdirectory(extern/nidaq)
add_subdirectory(extern/sensor-data)
add_subdirectory(extern/openigtlink/OpenIGTLink)
# Sources for generated libraries
add_subdirectory(src)

# Sources application executables
add_subdirectory(apps)

# Sources for unit tests
#add_subdirectory(tests)

