cmake_minimum_required(VERSION 3.14)
project(SQL_Interpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  tabulate
  GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
  GIT_TAG v1.5
)
FetchContent_MakeAvailable(tabulate)


include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(SQL_Interpreter ${SOURCES})

target_link_libraries(SQL_Interpreter PRIVATE nlohmann_json tabulate)
add_custom_command(TARGET SQL_Interpreter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/grammar.json
    $<TARGET_FILE_DIR:SQL_Interpreter>)
