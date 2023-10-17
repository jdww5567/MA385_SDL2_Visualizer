cmake_minimum_required(VERSION 3.24.2)

project(Grapher)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Add your source files here (replace these placeholders)
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

# List of libraries to link against
set(LIBRARIES
    mingw32
    SDL2main
    SDL2
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})