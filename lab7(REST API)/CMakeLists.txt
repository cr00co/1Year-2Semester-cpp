cmake_minimum_required(VERSION 3.16)
project(QuoteApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

add_executable(QuoteApp
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/QuoteFetcher.cpp
    src/QuoteFetcher.h
)

target_link_libraries(QuoteApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
