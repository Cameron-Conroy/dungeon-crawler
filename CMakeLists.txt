cmake_minimum_required(VERSION 3.16)
project(DungeonCrawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML 3.x
find_package(SFML 3 COMPONENTS Graphics Audio REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/states/PlayingState.cpp
    src/states/MainMenuState.cpp
    src/states/GameOverState.cpp
    src/states/VictoryState.cpp
)

# Header files (for IDE integration)
set(HEADERS
    src/Application.hpp
    src/core/AssetManager.hpp
    src/core/EventBus.hpp
    src/core/GameState.hpp
    src/core/StateManager.hpp
    src/ecs/Component.hpp
    src/ecs/Entity.hpp
    src/ecs/EntityManager.hpp
    src/ecs/EntityFactory.hpp
    src/ecs/Systems.hpp
    src/game/Room.hpp
    src/game/Floor.hpp
    src/game/RunState.hpp
    src/states/PlayingState.hpp
    src/states/MainMenuState.hpp
    src/states/PausedState.hpp
    src/states/GameOverState.hpp
    src/states/VictoryState.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SFML 3.x (uses namespaced targets)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# ============================================================================
# Testing with Catch2
# ============================================================================

option(BUILD_TESTS "Build the test suite" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.0
    )
    FetchContent_MakeAvailable(Catch2)

    set(TEST_SOURCES
        tests/test_components.cpp
        tests/test_entity.cpp
        tests/test_entity_factory.cpp
        tests/test_systems.cpp
        tests/test_event_bus.cpp
        tests/test_room.cpp
        tests/test_floor.cpp
        tests/test_run_state.cpp
    )

    add_executable(DungeonCrawlerTests ${TEST_SOURCES})
    target_link_libraries(DungeonCrawlerTests PRIVATE Catch2::Catch2WithMain SFML::Graphics SFML::Audio)
    target_include_directories(DungeonCrawlerTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

    include(CTest)
    include(Catch)
    catch_discover_tests(DungeonCrawlerTests)
endif()
