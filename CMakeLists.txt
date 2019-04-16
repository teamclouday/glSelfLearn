cmake_minimum_required(VERSION 3.10)
project(glSelfLearn VERSION 1.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -mconsole")
add_definitions(-DGLEW_STATIC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/external/glew")
execute_process (
    COMMAND unzip ${CMAKE_SOURCE_DIR}/external/external.zip -d ${CMAKE_SOURCE_DIR}/external
)
ENDIF()

IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/SDL2.dll")
execute_process (
    COMMAND cp ${CMAKE_SOURCE_DIR}/external/SDL2/bin/SDL2.dll ${CMAKE_SOURCE_DIR}/SDL2.dll
)
ENDIF()

include_directories(
    "${CMAKE_SOURCE_DIR}/include/"
    "${CMAKE_SOURCE_DIR}/external/glew/include"
    "${CMAKE_SOURCE_DIR}/external/SDL2/include"
    "${CMAKE_SOURCE_DIR}/external/glm/include"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/external/glew/lib"
    "${CMAKE_SOURCE_DIR}/external/SDL2/lib"
)

find_package(OpenGL REQUIRED)

add_executable(run ${SRC_FILES})

target_link_libraries(run
    mingw32
    SDL2main
    SDL2
    glew32s
    ${OPENGL_LIBRARIES}
)