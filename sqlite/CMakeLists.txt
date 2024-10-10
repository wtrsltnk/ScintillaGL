cmake_minimum_required(VERSION 3.12)

project(sqlite VERSION "3.46.1")

add_library(sqlite)

target_sources(sqlite
    PRIVATE
      "sqlite3.c"
      "sqlite3.h"
)

target_include_directories(sqlite
    PUBLIC
        "."
)
