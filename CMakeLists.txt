cmake_minimum_required(VERSION 3.19)
project(McClient C)

set(CMAKE_C_STANDARD 99)

include_directories(includes/)
add_executable(McClient src/packet.c src/utils.c src/client.c src/network.c src/login_packet.c src/crypt.c)
target_link_libraries(McClient m)