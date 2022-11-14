cmake_minimum_required(VERSION 3.13)

function(set_output_directory target)
    set_target_properties(${target}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME}_${CMAKE_BUILD_TYPE}_${target}"
            )
endfunction()

function(setup_exe name sources headers)
    file(GLOB_RECURSE ${name}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/${sources})
    file(GLOB_RECURSE ${name}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/${headers})
    add_executable(${name} ${${name}_SOURCES} ${${name}_HEADERS})
    set_output_directory(${name})
endfunction()

function(setup_lib name sources headers)
    file(GLOB_RECURSE ${name}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/${sources})
    file(GLOB_RECURSE ${name}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/${headers})
    add_library(${name} ${${name}_SOURCES} ${${name}_HEADERS})
    set_output_directory(${name})
endfunction()

function(setup_dll name sources headers)
    file(GLOB_RECURSE ${name}_SOURCES ${CMAKE_CURRENT_LIST_DIR}/${sources})
    file(GLOB_RECURSE ${name}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/${headers})
    add_library(${name} SHARED ${${name}_SOURCES} ${${name}_HEADERS})
    set_output_directory(${name})
endfunction()



project(base_lib)

set(CMAKE_CXX_STANDARD 23)

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



#base_lib
setup_lib(base_lib "src/*.cpp" "include/base_lib/*.h")

target_include_directories(base_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(base_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/third_party/include)



#tests
setup_exe(tests "tests/*.cpp" "tests/*.h")

add_dependencies(tests base_lib)
target_link_libraries(tests base_lib)
