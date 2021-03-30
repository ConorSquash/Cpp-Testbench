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

add_executable(solver_class_test solver_class_testbench.cpp )
target_link_libraries(solver_class_test PRIVATE   solver_class coil_dim coil_spec sensor_objective field_calc lsq eigen)
file(COPY yOriented-25cm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(daq_class_test daq_class_testbench.cpp)
target_link_libraries(daq_class_test PRIVATE daq_class eigen demodulation_class )

add_executable(server_test server_test.cpp)
target_link_libraries(server_test PRIVATE eigen OpenIGTLink)

add_executable(server_test2 server_test_2.cpp)
target_link_libraries(server_test2 PRIVATE eigen OpenIGTLink)

add_executable(server_test3 openigtlink_testbench.cpp)
target_link_libraries(server_test3 PRIVATE eigen OpenIGTLink openigtlink_class)

add_executable(system_test system_test.cpp)
target_link_libraries(system_test PRIVATE eigen OpenIGTLink openigtlink_class daq_class solver_class demodulation_class)
file(COPY yOriented-25cm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


file(COPY simulated_data.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY simulated_data_w_noise.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY yOriented-10cm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY yOriented-25cm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY zOriented-10cm.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

