package com.example.server;

import com.example.server.dtos.Strategy;
import io.quarkus.test.junit.QuarkusTest;
import org.junit.jupiter.api.Test;

import static io.restassured.RestAssured.given;
import static org.hamcrest.CoreMatchers.is;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

@QuarkusTest
class StrategyResourceTest {
    @Test
    void testHelloEndpoint() {
        given()
                .when().get("/strategy/hello")
                .then()
                .statusCode(200)
                .body(is("Hello, --World-- Properties!"));
    }

    @Test
    void testRandomEndpoint() {
        given()
                .when().get("/strategy/random")
                .then()
                .statusCode(200)
                .extract().as(Strategy.class);
    }

    @Test
    void testFairEndpoint() {
        Strategy strategy1 = given()
                .when().get("/strategy/fair")
                .then()
                .statusCode(200)
                .extract().as(Strategy.class);

        Strategy strategy2 = given()
                .when().get("/strategy/fair")
                .then()
                .statusCode(200)
                .extract().as(Strategy.class);

        Strategy strategy3 = given()
                .when().get("/strategy/fair")
                .then()
                .statusCode(200)
                .extract().as(Strategy.class);

        assertNotEquals(strategy1, strategy2);
        assertNotEquals(strategy2, strategy3);
        assertNotEquals(strategy1, strategy3);
    }

}
