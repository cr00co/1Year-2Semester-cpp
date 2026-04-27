cmake_minimum_required(VERSION 3.16)
project(Lab6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 QUIET COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    set(QT_LIBS Qt5::Core Qt5::Widgets)
else()
    set(QT_LIBS Qt6::Core Qt6::Widgets)
endif()

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    URL      https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
)

set(JSON_BuildTests   OFF CACHE INTERNAL "")
set(JSON_Install      OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(nlohmann_json)

add_executable(Lab6
    src/main.cpp
    src/MainWindow.cpp
)

target_include_directories(Lab6 PRIVATE
    src
)

target_link_libraries(Lab6 PRIVATE
    ${QT_LIBS}
    nlohmann_json::nlohmann_json
)
