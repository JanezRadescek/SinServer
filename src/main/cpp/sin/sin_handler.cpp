//
// Created by janez on 11.11.2024.
//

#include "sin_handler.h"

#include <set>

#include "../dtos/Msg.h"
#include "../dtos/Task.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

struct MsgComparator {
    bool operator()(const Msg &lhs, const Msg &rhs) const {
        return lhs.id < rhs.id;
    }
};

std::set<Msg, MsgComparator> activeMessages;
std::mutex activeMessagesMutex;


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

bool validateNewTaskMsg(const Msg &msg, const std::function<void(const Msg &)> &broadcast_message,
                        const std::unordered_map<std::string, Msg> &activeTask) {
    if (msg.id.empty()) {
        broadcast_message(Msg("", MessageType::ERROR, Task(0, 0, 0, 0, 0), "Task id is null."));
        return false;
    }
    if (activeTask.find(msg.id) != activeTask.end()) {
        broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), "Task id already exists."));
        return false;
    }
    Task task = msg.task;
    if (task.input == 0 && task.output == 0 && task.term == 0 && task.step == 0 && task.required_steps == 0) {
        broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), "Task is null."));
        return false;
    }
    if (task.output != 0 || task.step != 0 || task.required_steps <= 0) {
        broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), "Invalid task parameters."));
        return false;
    }
    return true;
}

void process_all_steps(const Msg &msg, const std::function<void(const Msg &)> &broadcast_message) {
    // if(validateNewTaskMsg())

    auto current_task = msg.task;
    auto start = std::chrono::steady_clock::now();

    while (current_task.step < current_task.required_steps) {
        current_task = sinOneStep(current_task);

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= 1000) {
            broadcast_message(Msg(msg.id, MessageType::PARTIAL, current_task, ""));
            start = now;
        }
    }
    broadcast_message(Msg(msg.id, MessageType::RESULT, current_task, ""));
}

void process_msg(const Msg &msg, const std::function<void(const Msg &)> &broadcast_message) {
    if (msg.type == MessageType::NEW_TASK) {
        {
            std::lock_guard lock(activeMessagesMutex);
            if (activeMessages.find(msg) != activeMessages.end()) {
                broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), "Task id already exists."));
                return;
            }
            activeMessages.insert(msg);
        }
        try {
            process_all_steps(msg, broadcast_message);
        } catch (const std::exception &e) {
            broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), e.what()));
        } {
            std::lock_guard lock(activeMessagesMutex);
            activeMessages.erase(msg);
        }
    } else {
        broadcast_message(Msg(msg.id, MessageType::ERROR, Task(0, 0, 0, 0, 0), "Invalid message type."));
    }
}

void on_message_process(const std::string &message,
                        const std::function<void(const std::string &)> &broadcast_message) {
    std::string id = "";
    try {
        // Convert input string to Msg object
        Msg msg = Msg::fromString(message);
        id = msg.id;

        // Call process_one_step
        process_all_steps(msg, [&](const Msg &result_msg) { broadcast_message(result_msg.toString()); });
    } catch (const std::exception &e) {
        broadcast_message(Msg(id, MessageType::ERROR, Task(0, 0, 0, 0, 0), e.what()).toString());
    }
}

void on_open_process(const std::function<void(const std::string &)> &send_message) {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuid_str = boost::uuids::to_string(uuid);
    send_message(Msg(uuid_str, MessageType::ID, Task(0, 0, 0, 0, 0), "Server is ready!").toString());

    std::lock_guard lock(activeMessagesMutex);
    for (const Msg &msg: activeMessages) {
        send_message(msg.toString());
    }
}
