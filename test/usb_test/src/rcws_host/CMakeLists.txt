message("CMAKE_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(rcws_host rcws_host.cc)

# 要出現 g++ -lxxx， 在 cmake 之中要去掉 'l'
target_link_libraries(rcws_host PRIVATE 
host_usb_lib
lra_fft_lib)

target_include_directories(rcws_host PRIVATE 
host_usb_lib
lra_fft_lib
)

# set to bin dir
set_target_properties(rcws_host
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_compile_definitions(rcws_host PRIVATE RCWS_LRA_DATA_PATH=\"${RCWS_LRA_DATA_PATH}\")
target_compile_definitions(rcws_host PRIVATE RCWS_LRA_ROOT_PATH=\"${RCWS_LRA_ROOT_PATH}\")