package com.example.server.sin;

import com.example.server.dtos.Msg;
import jakarta.inject.Inject;
import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;

import java.util.List;

@Path("/sin")
public class SinResource {

    @Inject
    SinService sinService;

    @GET
    @Path("/active")
    public List<Msg> getActiveTasks() {
        return sinService.getActiveTasks();
    }
}
