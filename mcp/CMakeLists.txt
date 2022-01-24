set(CMAKE_BUILD_TYPE Debug)

# compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -pedantic -g")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# define SOURCE_PATH_SIZE for relative __FILENAME__ macro
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")

file(GLOB_RECURSE SOURCE_FILES *.c)

add_executable(main
    ${SOURCE_FILES}
)
target_link_libraries(main ZLIB::ZLIB OpenSSL::SSL CURL::libcurl pthread m ncurses form)
