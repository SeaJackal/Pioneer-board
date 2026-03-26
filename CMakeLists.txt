cmake_minimum_required(VERSION 3.10)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.deps" CACHE PATH "Base directory for CMake FetchContent downloads")

include(FetchContent)
FetchContent_Declare(
    Hydrodrivers
    GIT_REPOSITORY https://github.com/SeaJackal/Hydrodrivers.git
    GIT_TAG 631813004b1742879e637e0468465f7a7261c4bd
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
    PRIVATE HydrodriversShell PRIVATE HydrodriversThruster PRIVATE HydrolibShellCommands
    PRIVATE HydrolibDevice PRIVATE HydrolibBusDatalink PRIVATE HydrolibBusApplication)
