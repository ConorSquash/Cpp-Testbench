project(Anserlib)

# All sources that also need to be tested in unit tests go into a static library
add_library(coil_dim STATIC coil_dimension_calc.cpp coil_dimension_calc.h)
target_include_directories(coil_dim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(coil_spec STATIC coil_spec.cpp coil_spec.h)
target_link_libraries(coil_spec PRIVATE eigen)

add_library(sensor_objective STATIC sensor_objective_function.cpp sensor_objective_function.h)
target_link_libraries(sensor_objective PRIVATE eigen)

add_library(field_calc STATIC field_coil_calc.cpp field_coil_calc.h)
target_link_libraries(field_calc PRIVATE eigen)

add_library(cube_points STATIC generate_testpoints.cpp generate_testpoints.h)
target_link_libraries(cube_points PRIVATE eigen)

add_library(solver_class STATIC solver_class.cpp solver_class.h)
target_link_libraries(solver_class PRIVATE eigen coil_dim coil_spec sensor_objective field_calc lsq)

add_library(demodulation_class STATIC demodulation_class.cpp demodulation_class.h)
target_link_libraries(demodulation_class PRIVATE eigen )

add_library(daq_class STATIC daq_class.cpp daq_class.h)
target_include_directories(daq_class PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(daq_class PUBLIC ${CMAKE_SOURCE_DIR}/extern/nidaq/include)
target_link_libraries(daq_class PRIVATE eigen daq)

add_library(openigtlink_class STATIC OpenIGTLink_class.cpp OpenIGTLink_class.h)
target_include_directories(openigtlink_class PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(openigtlink_class PUBLIC ${CMAKE_SOURCE_DIR}/extern/openigtlink/OpenIGTLink/Source)
target_link_libraries(openigtlink_class PRIVATE eigen OpenIGTLink)