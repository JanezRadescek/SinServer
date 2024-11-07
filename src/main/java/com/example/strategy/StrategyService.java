package com.example.strategy;

import jakarta.enterprise.context.ApplicationScoped;

import java.util.HashSet;
import java.util.List;
import java.util.Random;
import java.util.Set;

@ApplicationScoped
public class StrategyService {

    Set<Strategy> unusedStrategies = new HashSet<>(List.of(Strategy.values()));
    private final Random random = new Random();

    public synchronized Strategy getRandomStrategy() {
        Strategy[] strategies = Strategy.values();
        int randomIndex = random.nextInt(strategies.length);
        return strategies[randomIndex];
    }


    public synchronized Strategy getFairStrategy() {
        if (unusedStrategies.isEmpty()) {
            unusedStrategies = new HashSet<>(List.of(Strategy.values()));
        }
        int randomIndex = random.nextInt(unusedStrategies.size());
        Strategy selectedStrategy = unusedStrategies.stream().toList().get(randomIndex);
        unusedStrategies.remove(selectedStrategy);
        return selectedStrategy;
    }
}
