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

double sinOneTerm(double term, double x, int n) {
    return term * -x * x / ((2 * n) * (2 * n + 1));
}

Task sinOneStep(const Task &task) {
    double x = task.input;
    // x = fmod(x, 2 * M_PI); // Technically, it converges on the whole ℝ, so ¯\_(ツ)_/¯. Since we are technical, it's not even in line with the task.
    double term = task.term;
    int step = task.step;
    double newTerm = step == 0 ? x : sinOneTerm(term, x, step);
    double newOutput = task.output + newTerm;
    std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Simulate long processing time
    std::cout << "Input: " << x << ", Step: " << step << ", New value: " << newOutput << std::endl;
    return Task(task.input, newOutput, newTerm, task.step + 1, task.required_steps);
}



void process_all_steps(const Msg &msg, const std::function<void(const std::string &)> &broadcast_message) {
    auto current_task = msg.task;
    auto start = std::chrono::steady_clock::now();

    while (current_task.step < current_task.required_steps) {
        current_task = sinOneStep(current_task);

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= 1000) {
            broadcast_message(Msg(msg.id, MessageType::PARTIAL, current_task, "").toString());
            start = now;
        }
    }
    broadcast_message(Msg(msg.id, MessageType::RESULT, current_task, "").toString());
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
