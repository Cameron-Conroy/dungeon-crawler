cmake_minimum_required(VERSION 3.16)
project(DungeonCrawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML 3.x
find_package(SFML 3 COMPONENTS Graphics REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/CollisionSystem.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML 3.x (uses namespaced targets)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
