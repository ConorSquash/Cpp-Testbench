# The main program
project(Solver_Testbench)

add_executable(solver_testbench main.cpp)
target_link_libraries(solver_testbench PRIVATE eigen coil_dim coil_spec sensor_objective field_calc lsq)