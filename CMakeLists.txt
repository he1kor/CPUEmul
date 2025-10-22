cmake_minimum_required(VERSION 3.10)
project(CPUemul VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

file(DOWNLOAD 
    https://github.com/CLIUtils/CLI11/releases/download/v2.6.0/CLI11.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/CLI11.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC include
    ${CMAKE_CURRENT_BINARY_DIR}
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)