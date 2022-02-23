cmake_minimum_required(VERSION 3.0)
project(c_multi_thread)

set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -O3")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

set(CURL_LIBRARY "-lcurl -lpthread")
find_package(CURL REQUIRED)
add_executable (main src/main.c)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(main ${CURL_LIBRARIES})