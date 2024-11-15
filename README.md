# Sin Server

## C++

The application can be built using:

```shell script
sudo apt-get update && \
sudo apt-get install -y cmake libboost-all-dev libasio-dev libwebsocketpp-dev && \
mkdir -p build && \
cmake -S . -B build && \
cmake --build build
```

The application is now runnable using:

```shell script
./build/SinServer
```

### Docker

The image can be built using:

```shell script
docker build -t sinserver .
```

To run the image:

```shell script
docker run -p 8080:8080 sinserver
```


## Java

The application can be packaged using:

```shell script
./mvnw package
```

The application is now runnable using:

```shell script
java -jar ./target/quarkus-app/quarkus-run.jar
```
