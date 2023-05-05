cmake_minimum_required(VERSION 3.21)
set (CMAKE_C_STANDARD 99)

project (py_linker)
add_definitions(-DNAPI_VERSION=4)

include_directories(${CMAKE_JS_INC})

file(GLOB SOURCE_FILES "linker.c")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

find_package (Python REQUIRED COMPONENTS Interpreter Development.Embed)
target_include_directories(${PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${Python_LIBRARY_DIRS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${CMAKE_DL_LIBS}  ${Python_LIBRARIES})

execute_process (
  COMMAND python -c "import sysconfig; print(sysconfig.get_config_var('INSTSONAME'), end ='')"
  OUTPUT_VARIABLE LIBPYTHON_PATH
)
add_compile_definitions(LIBPYTHON_PATH="${LIBPYTHON_PATH}")

add_compile_options(-Wall -Wextra -Wpedantic)
