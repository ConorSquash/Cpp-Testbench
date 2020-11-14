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