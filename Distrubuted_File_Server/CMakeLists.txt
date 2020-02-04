cmake_minimum_required(VERSION 3.15)
project(Client C)
project(Server C)

set(CMAKE_C_STANDARD 11)

add_executable(Client dfc.c dfHeader.h)
add_executable(Server dfs.c dfHeader.h)
