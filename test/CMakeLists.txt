add_executable(client client_test.cpp)
target_link_libraries(client PRIVATE suc)

add_executable(server server_test.cpp)
target_link_libraries(server PRIVATE suc)

# Add pthread when building for linux
if (LINUX)
    target_link_libraries(server PRIVATE pthread)
endif (LINUX)
