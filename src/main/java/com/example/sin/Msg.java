package com.example.sin;

public record Msg(MsgType type, String id, Task task, String error) {
}
