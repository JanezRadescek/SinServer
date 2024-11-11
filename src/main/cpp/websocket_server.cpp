//
// Created by janez on 11.11.2024.
//

#include "websocket_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <functional>

#include "Config.h"

typedef websocketpp::server<websocketpp::config::asio> server;

void on_message(server *s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << "Received message: " << msg->get_payload() << std::endl;
    s->send(hdl, msg->get_payload(), msg->get_opcode());
}

void on_http(server *s, websocketpp::connection_hdl hdl) {
    server::connection_ptr con = s->get_con_from_hdl(hdl);
    if (con->get_resource() == "/ws/sin") {
        con->set_body("WebSocket endpoint");
        con->set_status(websocketpp::http::status_code::ok);
    } else {
        con->set_body("Not Found");
        con->set_status(websocketpp::http::status_code::not_found);
    }
}


void run_server() {
    server echo_server;

    try {
        echo_server.set_message_handler(bind(&on_message, &echo_server, std::placeholders::_1, std::placeholders::_2));
        echo_server.set_http_handler(std::bind(&on_http, &echo_server, std::placeholders::_1));

        echo_server.init_asio();
        echo_server.listen(Config::PORT);
        echo_server.start_accept();

        echo_server.run();
    } catch (websocketpp::exception const &e) {
        std::cout << "WebSocket++ exception: " << e.what() << std::endl;
    } catch (std::exception const &e) {
        std::cout << "Standard exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception" << std::endl;
    }
}
