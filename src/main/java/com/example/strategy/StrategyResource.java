package com.example.strategy;

import jakarta.inject.Inject;
import jakarta.ws.rs.GET;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.Produces;
import jakarta.ws.rs.core.MediaType;
import org.eclipse.microprofile.config.inject.ConfigProperty;

@Path("/strategy")
public class StrategyResource {
    
    @Inject
    StrategyService strategyService;

    @ConfigProperty(name = "greeting.message", defaultValue = "Hello World")
    String message;

    @GET
    @Produces(MediaType.TEXT_PLAIN)
    @Path("/hello")
    public String hello() {
        return message;
    }

    @GET
    @Path("/random")
    public Strategy getRandomStrategy() {
        return strategyService.getRandomStrategy();
    }

    @GET
    @Path("/fair")
    public Strategy getFairStrategy() {
        return strategyService.getFairStrategy();
    }
}
