cmake_minimum_required(VERSION 3.16)
project(lab5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/openssl")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenSSL REQUIRED)

add_executable(lab5
    src/main.cpp
    src/AuthorizationForm.cpp
    src/MainWindow.cpp
)

target_include_directories(lab5 PRIVATE src)
target_link_libraries(lab5 Qt6::Widgets OpenSSL::Crypto)
