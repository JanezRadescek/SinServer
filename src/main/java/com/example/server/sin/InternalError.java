package com.example.server.sin;

public class InternalError extends RuntimeException {
    public InternalError(Throwable cause) {
        super(cause);
    }
}
