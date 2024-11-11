//
// Created by janez on 11.11.2024.
//

#ifndef SIN_HANDLER_H
#define SIN_HANDLER_H


#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using server = websocketpp::server<websocketpp::config::asio>;

void process_and_broadcast(const std::string &message, const std::function<void(const std::string&)> &broadcast_message);

#endif //SIN_HANDLER_H
