#1.cmake verson
cmake_minimum_required(VERSION 3.13)

#2.project name
project(database C)

set(CMAKE_C_STANDARD 99)

#3.head file path
INCLUDE_DIRECTORIES(include)

#4.source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#5.set environment variable
SET(TEST_MATH
    ${DIR_SRCS}
)
#6.add executable file
add_executable(${PROJECT_NAME} ${TEST_MATH})

#7.add link library
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m)