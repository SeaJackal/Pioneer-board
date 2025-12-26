cmake_minimum_required(VERSION 3.10)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.deps" CACHE PATH "Base directory for CMake FetchContent downloads")

include(FetchContent)
FetchContent_Declare(
    Hydrodrivers
    GIT_REPOSITORY https://github.com/SeaJackal/Hydrodrivers.git
    GIT_TAG 2085bc4cdc769e927b65a2b39420ee56bef11b02
)
FetchContent_Populate(Hydrodrivers)

include("${hydrodrivers_SOURCE_DIR}/resources/gcc-arm-none-eabi-f4.cmake")

set(PROJECT_NAME Pioneer-board)

project(${PROJECT_NAME} VERSION 1.0)

enable_language(C CXX ASM)

# add_subdirectory(thirdparty)
add_subdirectory(${hydrodrivers_SOURCE_DIR} ${hydrodrivers_BINARY_DIR})

add_executable(${PROJECT_NAME} src/main.cpp src/irq_handlers.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE HydrodriversClockCPP PRIVATE HydrodriversUART
    PRIVATE HydrodriversShell PRIVATE HydrolibShellCommands PRIVATE HydrolibDevice)
