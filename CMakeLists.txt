cmake_minimum_required(VERSION 3.10)
project(glSelfLearn VERSION 1.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall")
IF(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mconsole")
ENDIF(WIN32)

add_definitions(-DGLEW_STATIC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

IF(WIN32)
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
    "${CMAKE_SOURCE_DIR}/external/SOIL/include"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/external/glew/lib"
    "${CMAKE_SOURCE_DIR}/external/SDL2/lib"
    "${CMAKE_SOURCE_DIR}/external/SOIL/lib"
)
ENDIF(WIN32)

find_package(OpenGL REQUIRED)

IF(UNIX)
include_directories("${CMAKE_SOURCE_DIR}/include/")
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
else()
    message("glew is not found on this machine")
endif()
FIND_PATH(SDL2_INCLUDE_DIRS SDL.h PATH_SUFFIXES SDL2)
FIND_LIBRARY(SDL2_LIBRARIES SDL2)
if(SDL2_INCLUDE_DIR_NOTFOUND)
    message("SDL2 is not found on this machine")
else()
    include_directories(${SDL2_INCLUDE_DIRS})
endif()
FIND_PATH(SOIL_INCLUDE_DIRS SOIL.h PATH_SUFFIXES SOIL)
FIND_LIBRARY(SOIL_LIBRARIES SOIL)
if(SOIL_INCLUDE_DIRS_NOTFOUND)
    message("SOIL is not found on this machine")
else()
    include_directories(${SOIL_INCLUDE_DIRS})
endif()
ENDIF(UNIX)

add_executable(run ${SRC_FILES})

IF(WIN32)
target_link_libraries(run
    mingw32
    SDL2main
    SDL2
    glew32s
    SOIL
    ${OPENGL_LIBRARIES}
)
ENDIF(WIN32)

IF(UNIX)
target_link_libraries(run
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${SOIL_LIBRARIES}
    ${OPENGL_LIBRARIES}
)
ENDIF(UNIX)