cmake_minimum_required(VERSION 3.10)
project(mcp VERSION 0.1 LANGUAGES C)

find_package(ZLIB)
find_package(OpenSSL)
find_package(CURL)

add_subdirectory(mcp)
