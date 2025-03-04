cmake_minimum_required(VERSION 3.10.0)

project(ConfiguratorTestApp VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB app_sources src/*.cpp)
set(SRC_FILES ${app_sources})

add_executable(ConfiguratorTestApp ${SRC_FILES})

target_include_directories(ConfiguratorTestApp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#<---Add Header dependency--->#
find_path(HIREDIS_HEADER hiredis)
target_include_directories(ConfiguratorTestApp PUBLIC ${HIREDIS_HEADER})

#<---Add Library Dependency--->#
find_library(HIREDIS_LIB hiredis)
find_library(EVENT_LIB event)
find_library(PTHREAD_LIB pthread)

target_link_libraries(ConfiguratorTestApp
    ${HIREDIS_LIB}
    ${EVENT_LIB}
    ${PTHREAD_LIB}
)

configure_file(versioning.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h @ONLY)
