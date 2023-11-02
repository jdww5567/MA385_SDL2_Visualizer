cmake_minimum_required(VERSION 3.24.2)

project(Grapher LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/vertexHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/cameraHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/glad/glad.c
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_widgets.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

# DLLs
set(LIBRARIES
    SDL2
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Set Compiler Flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -Wall)
endif()