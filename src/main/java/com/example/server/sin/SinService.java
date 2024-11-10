package com.example.server.sin;

import com.example.server.dtos.Msg;
import com.example.server.dtos.MsgType;
import com.example.server.dtos.Task;
import io.quarkus.logging.Log;
import jakarta.enterprise.context.ApplicationScoped;

import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

@ApplicationScoped
public class SinService {

    private ExecutorService executorService = Executors.newCachedThreadPool();
    private final ConcurrentHashMap<String, Boolean> cancellationMap = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, Msg> activeTask = new ConcurrentHashMap<>();

    public void handleMsg(Msg msg, Consumer<Msg> broadCastTask) {
        executorService.submit(() -> {
            switch (msg.type()) {
                case NEW_TASK -> handleNewTask(msg, broadCastTask);
                case CANCEL -> cancelTask(msg);
                default -> broadCastTask.accept(new Msg(msg.type(), msg.id(), null, "Unsupported msg type."));
            }
        });
    }

    public List<Msg> getActiveTasks() {
        return List.copyOf(activeTask.values());
    }

    private void handleNewTask(Msg msg, Consumer<Msg> broadCastTask) {
        Task currentTask = msg.task();

        var validMsg = validateNewTaskMsg(msg, broadCastTask);
        if (!validMsg) {
            return;
        }
        activeTask.put(msg.id(), msg);
        var start = System.currentTimeMillis();
        while (currentTask.step() < currentTask.required_steps()) {
            if (Boolean.TRUE.equals(cancellationMap.getOrDefault(msg.id(), false))) {
                var cancelMsg = new Msg(MsgType.CANCELED, msg.id(), null, null);
                broadCastTask.accept(cancelMsg);
                return;
            }
            currentTask = sinOneStep(currentTask);
            activeTask.put(msg.id(), new Msg(MsgType.PARTIAL, msg.id(), currentTask, null));
            if (System.currentTimeMillis() - start >= 1000) {
                var newMsg = new Msg(MsgType.PARTIAL, msg.id(), currentTask, null);
                broadCastTask.accept(newMsg);
                start += 1000;
            }
        }
        var newMsg = new Msg(MsgType.RESULT, msg.id(), currentTask, null);
        broadCastTask.accept(newMsg);
        activeTask.remove(msg.id());
    }

    private boolean validateNewTaskMsg(Msg msg, Consumer<Msg> broadCastTask) {
        if(msg.id() == null) {
            var newMsg = new Msg(MsgType.CANCELED, null, null, "Task id is null.");
            broadCastTask.accept(newMsg);
            return false;
        }
        if (activeTask.contains(msg.id())) {
            var newMsg = new Msg(MsgType.CANCELED, msg.id(), null, "Task id already exists.");
            broadCastTask.accept(newMsg);
            return false;
        }
        Task task = msg.task();
        if (task == null) {
            var newMsg = new Msg(MsgType.CANCELED, msg.id(), null, "Task is null.");
            broadCastTask.accept(newMsg);
            return false;
        }
        if (task.output() != 0 || task.step() != 0 || task.required_steps() <= 0) {
            var newMsg = new Msg(MsgType.CANCELED, msg.id(), null, "Invalid task parameters.");
            broadCastTask.accept(newMsg);
            return false;
        }
        return true;
    }

    private void cancelTask(Msg msg) {
        cancellationMap.put(msg.id(), true);
    }


    private static Task sinOneStep(Task task) {
        double x = task.input();
//        x = x % (2 * Math.PI); // Technically, it converges on the whole ℝ, so ¯\_(ツ)_/¯. Since we are technical, it's not even in line with the task.
        double term = task.term();
        int step = task.step();
        double newTerm = step == 0 ? x : sinOneTerm(term, x, step);
        double newOutput = task.output() + newTerm;
        try {
            Thread.sleep(300); // Simulate slowness, indivisible by 1000
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        Log.infof("Input: %.5f, Step: %d, New value: %.5f", x, step, newOutput);
        return new Task(task.input(), newOutput, newTerm, task.step() + 1, task.required_steps());
    }

    private static double sinOneTerm(double term, double x, int n) {
        return term * -x * x / ((2 * n) * (2 * n + 1));
    }
}


