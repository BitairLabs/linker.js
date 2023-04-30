cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 11)

project (py_linker)
include_directories(${CMAKE_JS_INC})
file(GLOB SOURCE_FILES "linker.c")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${PYTHON_LIBRARIES})

add_compile_options(-Wall -Wextra -Wpedantic)

add_definitions(-DNAPI_VERSION=3)