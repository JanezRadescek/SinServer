//
// Created by janez on 11.11.2024.
//

#include "sin_handler.h"
#include "../dtos/Msg.h"
#include "../dtos/Task.h"

#include <websocketpp/server.hpp>


void on_msg(server *s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::string message = msg->get_payload();
    std::cout << "Received message: " << message << std::endl;
    int number = std::stoi(message);
    message = std::to_string(sin(number));
    s->send(hdl, message, msg->get_opcode());
}

Task process_one_step(const Task &task) {
    double number = task.input;
    double result = std::sin(number);
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate long processing time
    return Task(task.input, result, 0, task.step + 1, task.required_steps);
}

void process_all_steps(const Msg &msg, const std::function<void(const std::string &)> &broadcast_message) {
    auto current_task = msg.task;
    while (current_task.step < current_task.required_steps) {
        current_task = process_one_step(current_task);
        broadcast_message(Msg(msg.id, MessageType::PARTIAL, current_task, "").toString());
    }
}

void process_and_broadcast(const std::string &message,
                           const std::function<void(const std::string &)> &broadcast_message) {
    std::string id = "";
    try {
        // Convert input string to Msg object
        Msg msg = Msg::fromString(message);
        id = msg.id;

        // Call process_one_step
        process_all_steps(msg, broadcast_message);
    } catch (const std::exception &e) {
        broadcast_message(Msg(id, MessageType::ERROR, Task(0, 0, 0, 0, 0), e.what()).toString());
    }
}
