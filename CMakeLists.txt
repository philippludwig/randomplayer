project(randomplayer)
cmake_minimum_required(VERSION 3.5)
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
include(ExternalProject)

file(GLOB_RECURSE SRC_FILES ./*.cpp)

# json dependency
ExternalProject_Add(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/json
        INSTALL_COMMAND ""
        BUILD_COMMAND ""
        )
ExternalProject_Get_Property(json source_dir)
set(json_source_dir ${source_dir}/src)

# Instruct CMake to run moc/uic/rcc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt deps
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Multimedia)
find_package(Qt5Network)
find_package(Qt5Widgets)

# Main application
QT5_WRAP_UI(UIS_HDRS gui/mainwindow.ui)
set(LINK_DIR "" CACHE FILEPATH "")
link_directories(${LINK_DIR})
add_executable(${PROJECT_NAME} ${SRC_FILES} ${UIS_HDRS})
add_dependencies(${PROJECT_NAME} json)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
include_directories(SYSTEM ${Qt5Core_INCLUDE} ${Qt5Gui_INCLUDE} ${Qt5Network_INCLUDE} ${Qt5Multimedia_INCLUDE} ${json_source_dir})

# C++14
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

# Link deps
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Network Qt5::Widgets)

# Enable C++14 and compiler-specific warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-padded")
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wshadow")
endif()
