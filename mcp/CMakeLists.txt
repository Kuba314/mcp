set(CMAKE_BUILD_TYPE Debug)

# compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -pedantic -g")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCE_FILES *.c)

add_executable(main
    ${SOURCE_FILES}
)
target_link_libraries(main ZLIB::ZLIB OpenSSL::SSL CURL::libcurl pthread m)
