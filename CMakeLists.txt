#1.cmake verson
cmake_minimum_required(VERSION 3.13)

#2.project name
project(ECE361_TextConference C)
SET(CMAKE_C_STANDARD 99)

#3.head file path
INCLUDE_DIRECTORIES(include)

#4.source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

SET(EXCUTABLE bin)

add_executable(server ${EXCUTABLE}/server.c)
target_link_libraries(server m)

add_executable(client ${EXCUTABLE}/client.c)
target_link_libraries(client m)
