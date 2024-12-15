file(GLOB LAB_SOURCES "*.cpp")

add_library(lib STATIC ${LAB_SOURCES})
target_include_directories(lib PUBLIC "./")