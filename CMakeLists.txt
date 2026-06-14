cmake_minimum_required(VERSION 4.1)
project(bellit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    tiny-process-library
    GIT_REPOSITORY https://gitlab.com/eidheim/tiny-process-library.git
    GIT_TAG 8bbb5a211c5c9df8ee69301da9d22fb977b27dc1
    GIT_SHALLOW ON
)
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG b6135315a54deb2b556219906eda8346f69bf703
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(
    tiny-process-library
    cxxopts
)

# Ensure tiny-process-library is compiled with C++ 20, because on Windows MSVC
# standard library requires at least C++ 14.
get_target_property(tiny_process_library_compile_options tiny-process-library COMPILE_OPTIONS)
if(tiny_process_library_compile_options)
    list(REMOVE_ITEM tiny_process_library_compile_options -std=c++11)
    set_target_properties(tiny-process-library PROPERTIES
        COMPILE_OPTIONS "${tiny_process_library_compile_options}"
    )
endif()
target_compile_features(tiny-process-library PRIVATE cxx_std_20)

add_executable(${PROJECT_NAME} src/main.cpp)
target_sources(${PROJECT_NAME}
    PRIVATE FILE_SET CXX_MODULES
    FILES src/notification.cppm
    src/process.cppm
)
target_compile_options(${PROJECT_NAME}
    PRIVATE -Wall -Werror
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE tiny-process-library
    PRIVATE cxxopts
)
