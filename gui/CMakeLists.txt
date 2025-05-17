# GUI module (LVGL display drivers, screens, init)

file(GLOB GUI_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/osd/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/osd_loader/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/display_backends/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/indev_backends/*.c"
)

add_library(gui ${GUI_SRC})

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gui PRIVATE lvgl)
