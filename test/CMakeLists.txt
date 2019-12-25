cmake_minimum_required(VERSION 3.10)

project(Test VERSION 0.1)

add_executable(client client_test.cpp)
target_link_libraries(client suc)

add_executable(server server_test.cpp)
target_link_libraries(server suc)
