cmake_minimum_required(VERSION 3.10)
project(nodemcu-rgblight-desktop-client C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(CMAKE_C_STANDARD 11)

add_executable(nodemcu-rgblight-desktop-client main.c)

target_include_directories(nodemcu-rgblight-desktop-client PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(nodemcu-rgblight-desktop-client PRIVATE ${GTK3_LIBRARIES})

install(TARGETS nodemcu-rgblight-desktop-client DESTINATION bin)