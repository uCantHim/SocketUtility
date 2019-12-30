cmake_minimum_required(VERSION 3.10)

project(SUC VERSION 0.1)

set(SUC_SOURCES
	SUC.h
	SocketUtility.h
	SucInternals.h
	ClientSocket.h ClientSocket.cpp
	ServerSocket.h ServerSocket.cpp
)

add_library(
	suc STATIC
	${SUC_SOURCES}
)
