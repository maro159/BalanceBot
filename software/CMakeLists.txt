# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "C:\proj2\Pico SDK v1.5.0\pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(BalanceBot C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add source files
add_executable(BalanceBot
        main.c
        oled.c
        encoder_rot.c
        menu.c
        imu.c
        motor.c 
        pid.c 
        controler.c 
        ssd1306/ssd1306.c
        remote_control.c
        adc.c
        )

# Set program name and version
pico_set_program_name(BalanceBot "BalanceBot")
pico_set_program_version(BalanceBot "0.1")

# Enable/disable stdio over UART/USB
pico_enable_stdio_uart(BalanceBot 1)
pico_enable_stdio_usb(BalanceBot 0)

# Add include files
target_include_directories(BalanceBot PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/ssd1306
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add the standard library to the build
target_link_libraries(BalanceBot
pico_stdlib)

# Add other libraries
target_link_libraries(BalanceBot
        hardware_i2c
        hardware_pio
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_pwm
        hardware_adc
        pico_sync
        pico_multicore
        )

# Add pio files
pico_generate_pio_header(BalanceBot ${CMAKE_CURRENT_LIST_DIR}/quadrature_encoder.pio)

# Create additional build files
pico_add_extra_outputs(BalanceBot)