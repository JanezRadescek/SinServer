cmake_minimum_required(VERSION 3.29)
project(SinServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(WebSocketPP REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("/usr/include/asio")
include_directories(${WebSocketPP_INCLUDE_DIRS})

add_executable(SinServer src/main/cpp/main.cpp
        src/main/cpp/ws/websocket_server.cpp
        src/main/cpp/ws/websocket_server.h
        src/main/cpp/Config.h
        src/main/cpp/sin/sin_handler.cpp
        src/main/cpp/sin/sin_handler.h)

target_link_libraries(SinServer ${Boost_LIBRARIES} ${Asio_LIBRARIES} ${WebSocketPP_LIBRARIES})