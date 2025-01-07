cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

set(CMAKE_C_STANDARD 99)

add_executable(snake main.c server.c client.c socket.c)
target_compile_options(snake PRIVATE "-g" "-O0" "-Wall" "-Wextra" "-Wpedantic")
