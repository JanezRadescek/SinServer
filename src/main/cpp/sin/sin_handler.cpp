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

void process_and_broadcast(const std::string &message, const std::function<void(const std::string&)> &broadcast_message) {
    try {
        double number = std::stod(message);
        double result = std::sin(number);
        std::string result_str = std::to_string(result);
        broadcast_message(result_str);
    } catch (const std::exception &e) {
        std::cerr << "Error processing message: " << e.what() << std::endl;
    }
}