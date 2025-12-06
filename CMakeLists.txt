cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME Pioneer-board)

project(${PROJECT_NAME} VERSION 1.0)

enable_language(C CXX ASM)

add_subdirectory(thirdparty/Hydrodrivers)

add_executable(${PROJECT_NAME} src/main.cpp src/irq_handlers.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE HydrodriversClockCPP PRIVATE HydrodriversUART
    PRIVATE HydrolibShell PRIVATE HydrolibShellCommands PRIVATE HydrolibDevice)
