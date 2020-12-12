project (NI-DAQ)

# Header only library, therefore INTERFACE
add_library(daq INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(daq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(daq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib64/msvc/NIDAQmx.lib)
target_link_libraries(daq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib64/msvc/nisyscfg.lib)
