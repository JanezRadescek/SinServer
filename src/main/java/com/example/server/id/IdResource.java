package com.example.server.id;

import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;

import java.util.UUID;

@Path("/id")
public class IdResource {
    

    @GET
    public UUID getClientId() {
        return UUID.randomUUID();
    }
}
