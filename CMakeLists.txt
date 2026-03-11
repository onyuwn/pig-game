cmake_minimum_required(VERSION 3.20)

project(PIGGAME)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(Bullet CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/**/*.cpp
    src/base/glad.c
)


add_executable(PIGGAME ${SOURCES})

target_include_directories(PIGGAME PRIVATE
    include
)

target_link_libraries(PIGGAME PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
    Freetype::Freetype
    BulletDynamics
    BulletCollision
    LinearMath
)