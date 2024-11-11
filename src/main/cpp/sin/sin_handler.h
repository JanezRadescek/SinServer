//
// Created by janez on 11.11.2024.
//

#ifndef SIN_HANDLER_H
#define SIN_HANDLER_H


#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

void on_msg(server *s, websocketpp::connection_hdl hdl, server::message_ptr msg);

#endif //SIN_HANDLER_H
