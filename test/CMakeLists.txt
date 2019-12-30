cmake_minimum_required(VERSION 3.10)

if (UNIX AND NOT APPLE)
    set (LINUX TRUE)
endif ()

project(Test VERSION 0.1)

add_executable(client client_test.cpp)
target_link_libraries(client suc)

add_executable(server server_test.cpp)
target_link_libraries(
    server
    suc
)

# Add pthread when building for linux
if (LINUX)
    target_link_libraries(
        server
        pthread
    )
endif (LINUX)
