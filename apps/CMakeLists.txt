# The main program
project(Solver_Testbench)

add_executable(solver_testbench main.cpp)
target_link_libraries(solver_testbench PRIVATE eigen coil_dim coil_spec sensor_objective field_calc lsq)

add_executable(solver_cube_testbench main_cube_test.cpp)
target_link_libraries(solver_cube_testbench PRIVATE cube_points eigen coil_dim coil_spec sensor_objective field_calc lsq)

add_executable(cpp_ni_daq_test ni_daq_test.cpp)
target_link_libraries(cpp_ni_daq_test PRIVATE eigen daq)

add_executable(cpp_dft_testbench dft_testbench.cpp)
target_link_libraries(cpp_dft_testbench PRIVATE eigen)

add_executable(cpp_daq_and_dft daq_and_demod.cpp)
target_link_libraries(cpp_daq_and_dft PRIVATE eigen daq)