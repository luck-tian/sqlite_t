cmake_minimum_required(VERSION 3.25)

set(This sqlite_t)

project(${This} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(Headers 
	database_handler.h
	sqlite3.h
)

set(Sources 
	sqlite_cursor.cpp
	sqlite_database.cpp
	sqlite_prepared_statement.cpp
	sqlite3.c
)


add_library(${This} STATIC ${Sources} ${Headers})

target_include_directories(${This} PUBLIC .)



