package com.example.server.sin;

import com.example.server.dtos.Msg;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import io.quarkus.logging.Log;
import jakarta.enterprise.context.ApplicationScoped;
import jakarta.inject.Inject;
import jakarta.websocket.OnClose;
import jakarta.websocket.OnError;
import jakarta.websocket.OnMessage;
import jakarta.websocket.OnOpen;
import jakarta.websocket.Session;
import jakarta.websocket.server.ServerEndpoint;
import org.eclipse.microprofile.config.inject.ConfigProperty;

import java.io.IOException;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@ServerEndpoint("/ws/sin")
@ApplicationScoped
public class SinSocket {

    private static final Set<Session> sessions = Collections.synchronizedSet(new HashSet<>());
    private final ExecutorService executorService = Executors.newCachedThreadPool();
    private final ObjectMapper objectMapper = new ObjectMapper();

    @ConfigProperty(name = "error.internal", defaultValue = "Internal error")
    String internalError;

    @Inject
    SinService sinService;

    @OnOpen
    public void onOpen(Session session) {
        sessions.add(session);
        Log.info("onOpen> ");
    }

    @OnClose
    public void onClose(Session session) {
        sessions.remove(session);
        Log.info("onClose> ");
    }

    @OnError
    public void onError(Session session, Throwable throwable) {
        Log.info("onError> : " + throwable);
    }

    @OnMessage
    public void onMessage(String message) {
        Log.info("onMessage> : " + message);
        try {
            var msg = toMessage(message);
            if (msg == null) {
                broadcastMessage("Invalid msg");
                return;
            }
            sinService.handleMsg(msg, this::broadCastMsg);
        } catch (InternalError e) {
            broadcastMessage(internalError);
        }
    }

    private Msg toMessage(String message) {
        try {
            return objectMapper.readValue(message, Msg.class);
        } catch (JsonProcessingException e) {
            Log.error("Cant parse message.", e);
            return null;
        }
    }

    private void broadCastMsg(Msg msg) {
        String jsonString;
        try {
            jsonString = objectMapper.writeValueAsString(msg);
        } catch (JsonProcessingException e) {
            jsonString = internalError;
        }
        broadcastMessage(jsonString);
    }

    private void broadcastMessage(String message) {
        Log.info("onMessage< : " + message);
        synchronized (sessions) {
            for (Session session : sessions) {
                if (session.isOpen()) {
                    executorService.submit(() -> {
                        try {
                            session.getBasicRemote().sendText(message);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    });
                }
            }
        }
    }
}
