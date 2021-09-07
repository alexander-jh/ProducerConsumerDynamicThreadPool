cmake_minimum_required(VERSION 3.20)
project(Project1 C)

set(CMAKE_C_STANDARD 11)

set(EXEC executor.c)
set(SOURCES threads.c atomic_queue.c transform.c)
set(HEADERS threads.h atomic_queue.h transform.h transformMat.h)
set(OBJECTS transformMat.o)

add_executable(Project1 ${EXEC} ${SOURCES} ${OBJECTS} ${HEADERS})
