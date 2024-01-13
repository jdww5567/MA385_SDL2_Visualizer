cmake_minimum_required(VERSION 3.24.2)

project(Grapher LANGUAGES CXX C)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/plot.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/mine/gui.cpp
    ${CMAKE_SOURCE_DIR}/src/glad/glad.c
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui/imgui_widgets.cpp
)

configure_file(${CMAKE_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment.glsl ${CMAKE_BINARY_DIR}/shaders/fragment.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/compute.glsl ${CMAKE_BINARY_DIR}/shaders/compute.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/dlls/SDL2.dll ${CMAKE_BINARY_DIR}/SDL2.dll COPYONLY)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} SDL2)