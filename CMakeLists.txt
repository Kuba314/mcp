cmake_minimum_required(VERSION 3.10)
project(mcp VERSION 0.1 LANGUAGES C)

set(PROJECT_TEST_NAME run_tests)

set(CMAKE_BUILD_TYPE Debug)

# compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -pedantic")

# add include/ directory
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(ZLIB)
find_package(OpenSSL)
find_package(CURL)

file(GLOB_RECURSE PACKET_FILES src/packets/*.c)
file(GLOB_RECURSE HANDLER_FILES src/handlers/*.c)
set(SOURCE_FILES
    src/varint.c
    src/dynstring.c
    src/world.c
    src/hashmap.c
    src/tablist.c
    src/connection.c
    src/stream.c
    src/buffer.c
    src/packet_handler.c
    src/auth/mojang_auth.c
    src/config.c
    src/json.c
    ${PACKET_FILES}
    ${HANDLER_FILES}
)

add_executable(main
    src/main.c
    ${SOURCE_FILES}
)
target_link_libraries(main ZLIB::ZLIB OpenSSL::SSL CURL::libcurl pthread m)
