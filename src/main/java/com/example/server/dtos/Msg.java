package com.example.server.dtos;

public record Msg(MsgType type, String id, Task task, String error) {
}
