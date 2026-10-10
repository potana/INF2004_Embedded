cmake_minimum_required(VERSION 3.13)

# Pico WH A: motion controller (FreeRTOS, C/C++).
# Needs PICO_SDK_PATH and FREERTOS_KERNEL_PATH (see ../README.md).
set(PICO_BOARD pico_w)
include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(pico_a_motion C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pico_a_motion
    src/app_main.c
    src/subsystem1/protocol.c
    src/subsystem1/task_link.c
    src/subsystem2/as5600.c
    src/subsystem2/task_motor.c
    src/subsystem2/task_planner.c
    src/subsystem4/task_safety.c
)

target_include_directories(pico_a_motion PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(pico_a_motion
    pico_stdlib
    hardware_pwm
    hardware_i2c
    hardware_uart
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# USB serial for logs; UART0 (GP0/GP1) is reserved for the link to Pico B.
pico_enable_stdio_usb(pico_a_motion 1)
pico_enable_stdio_uart(pico_a_motion 0)
pico_add_extra_outputs(pico_a_motion)

# Host-side unit tests for code with no hardware dependency (CRC, framing).
# Build with:  gcc -Isrc -o /tmp/test_protocol tests/test_protocol.c src/subsystem1/protocol.c
