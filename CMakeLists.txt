PROJECT(emgl)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

SET(PROJECT_BINARY_DIR build)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(GLM REQUIRED)

INCLUDE_DIRECTORIES(
    ${GLUT_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

ADD_EXECUTABLE(emgl src/main.cpp)

TARGET_LINK_LIBRARIES(
    emgl
    ${OPENGL_LIBRARY}
    ${GLUT_LIBRARY}
)

ADD_CUSTOM_TARGET(
    emscripten
    em++
    -std=c++11
    -s FULL_ES2=1
    -s GL_TESTING=1
    -O2
    -I ${GLM_INCLUDE_DIR}
    -o emgl.html
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
