cmake_minimum_required(VERSION 3.28)
project(SinServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(WebSocketPP REQUIRED)

add_executable(SinServer src/main/cpp/main.cpp
        src/main/cpp/ws/websocket_server.cpp
        src/main/cpp/ws/websocket_server.h
        src/main/cpp/Config.h
        src/main/cpp/sin/sin_handler.cpp
        src/main/cpp/sin/sin_handler.h
        src/main/cpp/dtos/MessageType.h
        src/main/cpp/dtos/Task.h
        src/main/cpp/dtos/Msg.h
        src/main/cpp/dtos/JsonHelper.h)