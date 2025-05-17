cmake_minimum_required(VERSION 3.16)
project(LOpenHD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(DRM REQUIRED libdrm)
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl xkbcommon wayland-cursor)

set(WAYLAND_PROTOCOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protocols)
set(GENERATED_PROTOCOLS_DIR ${CMAKE_BINARY_DIR}/generated_protocols)
file(MAKE_DIRECTORY ${GENERATED_PROTOCOLS_DIR})

add_custom_command(
    OUTPUT ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.h ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.c
    COMMAND wayland-scanner client-header ${WAYLAND_PROTOCOLS_DIR}/xdg-shell.xml ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.h
    COMMAND wayland-scanner private-code ${WAYLAND_PROTOCOLS_DIR}/xdg-shell.xml ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.c
    DEPENDS ${WAYLAND_PROTOCOLS_DIR}/xdg-shell.xml
    COMMENT "Generating wayland_xdg_shell protocol headers"
)

add_custom_target(generate-xdg-protocols
    DEPENDS ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.h ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.c
)

add_library(xdg-shell-protocols STATIC ${GENERATED_PROTOCOLS_DIR}/wayland_xdg_shell.c)
add_dependencies(xdg-shell-protocols generate-xdg-protocols)

include_directories(${GENERATED_PROTOCOLS_DIR})
include_directories(
    ${DRM_INCLUDE_DIRS}
    ${WAYLAND_INCLUDE_DIRS}
    .
    gui
    telemetry
    external
)

add_definitions(-DLV_CONF_INCLUDE_SIMPLE)

add_subdirectory(external/lvgl)

# Link protocol and system libraries to lvgl
add_dependencies(lvgl generate-xdg-protocols)
target_link_libraries(lvgl PRIVATE
    xdg-shell-protocols
    ${DRM_LIBRARIES}
    ${WAYLAND_LIBRARIES}
)

add_subdirectory(gui)
add_subdirectory(telemetry)
add_subdirectory(video)

add_executable(lopenhd src/main.cpp)
add_dependencies(lopenhd generate-xdg-protocols)
target_link_libraries(lopenhd PRIVATE
    lvgl
    gui
    video
    telemetry
    xdg-shell-protocols
    ${DRM_LIBRARIES}
    ${WAYLAND_LIBRARIES}
)
