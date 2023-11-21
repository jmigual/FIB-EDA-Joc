cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(apocalypse CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE "src/engine")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts
    GIT_TAG v3.1.1
)

FetchContent_MakeAvailable(cxxopts)

target_link_libraries(${PROJECT_NAME} PRIVATE cxxopts::cxxopts)