cmake_minimum_required(VERSION 2.8)
project(lupnp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES Source/main.c)
add_executable(lupnp ${SOURCE_FILES} Header/UpnpCore.h Source/UpnpCore.c Header/HardwareHandler.h Source/HardwareHandler.c)


include_directories("/usr/local/include/gupnp-1.0")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/arm-linux-gnueabihf/glib-2.0/include")
#include_directories("/usr/lib/x86_64-linux-gnu/glib-2.0/include")
include_directories("/usr/include/libsoup-2.4")
include_directories("/usr/include/libxml2")
include_directories("/usr/include/uuid")
include_directories("/usr/include/gssdp-1.0  ")
include_directories("/usr/local/include")

link_directories("usr/local/lib")

target_link_libraries(lupnp libgmodule-2.0.so libgssdp-1.0.so libsoup-2.4.so libgobject-2.0.so
                        libglib-2.0.so libxml2.so libuuid.a libgupnp-1.0.so libwiringPi.so)
