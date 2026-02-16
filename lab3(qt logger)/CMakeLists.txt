cmake_minimum_required(VERSION 3.10)
project(lab3)

set(CMAKE_CXX_STANDARD 20)

if(APPLE)
    add_compile_options(-stdlib=libc++)
elseif(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(lab3 main.cpp)

target_link_libraries(lab3 Qt6::Widgets)