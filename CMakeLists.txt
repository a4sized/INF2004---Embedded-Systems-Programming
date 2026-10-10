cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Robotic_Car C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

# remove the hash from the lines below to include the corresponding c files in the build once done
add_executable(Robotic_Car 
    main.c
    safetyMonitor.c
    motionControl/motionControl.c
    #line_barcode/line_barcode.c        
    #obstacleScan/obstacleScan.c
    #terrain_IMU/terrain_IMU.c
    #wifi_telemetry/wifi_telemetry.c
)

target_include_directories(Robotic_Car PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/line_barcode
    ${CMAKE_CURRENT_LIST_DIR}/motionControl
    ${CMAKE_CURRENT_LIST_DIR}/obstacleScan
    ${CMAKE_CURRENT_LIST_DIR}/terrain_IMU
    ${CMAKE_CURRENT_LIST_DIR}/wifi_telemetry
)

target_link_libraries(Robotic_Car 
    pico_stdlib 
    hardware_pwm 
    hardware_i2c
)

pico_enable_stdio_usb(Robotic_Car 1)
pico_enable_stdio_uart(Robotic_Car 0)

pico_add_extra_outputs(Robotic_Car)



