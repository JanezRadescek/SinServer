//
// Created by janez on 11.11.2024.
//

#include "websocket_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <functional>
#include <set>

#include "../Config.h"
#include "../sin/sin_handler.h"

using server = websocketpp::server<websocketpp::config::asio>;
server ws_server;
std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl> > connections;
std::mutex connections_mutex;

void broadcast_message(const std::string &message) {
    std::lock_guard lock(connections_mutex);

    std::cout << "Broadcasting message: " << message << std::endl;
    for (auto &conn: connections) {
        ws_server.send(conn, message, websocketpp::frame::opcode::text);
    }
}

void on_message(websocketpp::connection_hdl, server::message_ptr msg) {
    std::string message = msg->get_payload();
    std::cout << "Received message: " << message << std::endl;

    std::thread([message]() {
        on_message_process(message, broadcast_message);
    }).detach();
}

void on_http(websocketpp::connection_hdl hdl) {
    server::connection_ptr con = ws_server.get_con_from_hdl(hdl);
    if (con->get_resource() == "/ws/sin") {
        con->set_body("WebSocket endpoint");
        con->set_status(websocketpp::http::status_code::ok);
    } else {
        con->set_body("Not Found");
        con->set_status(websocketpp::http::status_code::not_found);
    }
}

void on_open(websocketpp::connection_hdl hdl) {
    std::thread([hdl]() {
        on_open_process([hdl](const std::string &message) {
            ws_server.send(hdl, message, websocketpp::frame::opcode::text);
        });
    }).detach();

    std::lock_guard lock(connections_mutex);
    connections.insert(hdl);
}

void on_close(websocketpp::connection_hdl hdl) {
    std::lock_guard lock(connections_mutex);
    connections.erase(hdl);
}

void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", stopping server..." << std::endl;
    ws_server.stop_listening();
    ws_server.stop();
}


void run_server() {
    try {
        ws_server.set_message_handler(bind(&on_message, std::placeholders::_1, std::placeholders::_2));
        ws_server.set_http_handler(std::bind(&on_http, std::placeholders::_1));
        ws_server.set_open_handler(std::bind(&on_open, std::placeholders::_1));
        ws_server.set_close_handler(std::bind(&on_close, std::placeholders::_1));

        ws_server.init_asio();
        ws_server.listen(Config::PORT);
        ws_server.start_accept();

        // Register signal handler
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        ws_server.run();
    } catch (websocketpp::exception const &e) {
        std::cout << "WebSocket++ exception: " << e.what() << std::endl;
    } catch (std::exception const &e) {
        std::cout << "Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }
}
