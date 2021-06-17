cmake_minimum_required(VERSION 3.14)

project(evolution_simulator_2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add evolution simulator headers and sources
add_subdirectory(include)
add_subdirectory(src)


# Pass evolution simulator headers to fug for component / singleton scanning
set(FUG_APP_HEADERS ${EVOLUTION_SIMULATOR_HEADERS})


# Fetch external dependencies
add_subdirectory(ext)


# Add evolution simulator executable target
add_executable(evolution_simulator
    ${EVOLUTION_SIMULATOR_HEADERS}
    ${EVOLUTION_SIMULATOR_SOURCES}
)

target_include_directories(evolution_simulator
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(evolution_simulator
    PUBLIC
        EVOLUTION_SIMULATOR_RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/"
)

target_link_libraries(evolution_simulator
    PUBLIC
        fug_ecs
        fug_graphics
)