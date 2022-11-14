cmake_minimum_required(VERSION 3.13)

function(set_output_directory target)
    set_target_properties(${target}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            )
endfunction()

function(setup_defines)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_definitions(DEBUG=1)
    elseif()
        add_compile_definitions(DEBUG=0)
    endif()

    if(WIN32)
        add_compile_definitions(PLATFORM_WINDOWS=1)
        add_compile_definitions(PLATFORM_LINUX=0)
    elseif(UNIX AND NOT APPLE)
        add_compile_definitions(PLATFORM_WINDOWS=0)
        add_compile_definitions(PLATFORM_LINUX=1)
    endif()
endfunction()

project(base_lib)

set(CMAKE_CXX_STANDARD 20)

setup_defines()

file(GLOB BASE_LIB_SOURCES src/*.cpp)
file(GLOB BASE_LIB_HEADERS include/base_lib/*.h)
add_library(base_lib ${BASE_LIB_SOURCES} ${BASE_LIB_HEADERS})

target_include_directories(base_lib PUBLIC third_party/include)

set_output_directory(base_lib)

file(GLOB TESTS_SOURCES tests/*.cpp)
file(GLOB TESTS_HEADERS tests/*.h)
add_executable(tests ${TESTS_SOURCES} ${TESTS_HEADERS})

add_dependencies(tests base_lib)
target_include_directories(tests PUBLIC include)
target_link_libraries(tests base_lib)

set_output_directory(tests)
