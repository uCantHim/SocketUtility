cmake_minimum_required(VERSION 3.10)

project(SUC VERSION 0.1)

set(SUC_SOURCES
	main.cpp
	SUC.h
	SocketUtility.h
	SucInternals.h
	ClientSocket.h ClientSocket.cpp
	ServerSocket.h ServerSocket.cpp
)

add_executable(
	main
	${SUC_SOURCES}
)

add_library(
	suc
	${SUC_SOURCES}
)

target_include_directories(
	main PUBLIC
	"${PROJECT_BINARY_DIR}"
)
