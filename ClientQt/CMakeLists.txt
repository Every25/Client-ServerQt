cmake_minimum_required(VERSION 3.20)

project(LibrariesWidget CXX)

set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB SOURCES "*.cpp" "*.h")
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

set(RESOURCES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/icons)
list(APPEND SOURCES ${RESOURCES_FILE})

set(ICONS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icons)

# Цель для копирования папки icons в директорию сборки
add_custom_target(copy_icons ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ICONS_SOURCE_DIR}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/icons
    COMMENT "Copying icons directory to output directory"
)

#add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  PRIVATE nlohmann_json::nlohmann_json
)