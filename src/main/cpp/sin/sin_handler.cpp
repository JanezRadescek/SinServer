//
// Created by janez on 11.11.2024.
//

#include "sin_handler.h"

#include <websocketpp/server.hpp>


void on_msg(server *s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::string message = msg->get_payload();
    std::cout << "Received message: " << message << std::endl;
    int number = std::stoi(message);
    message = std::to_string(sin(number));
    s->send(hdl, message, msg->get_opcode());
}