cmake_minimum_required(VERSION 3.21)
set (CMAKE_C_STANDARD 99)

project (c_linker)
add_definitions(-DNAPI_VERSION=4)

include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES "linker.c")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

Find_Library(LIBFFI_LIBRARY NAMES ffi)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${LIBFFI_LIBRARY})

add_compile_options(-Wall -Wextra -Wpedantic)
