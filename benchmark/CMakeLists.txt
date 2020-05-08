file(GLOB SOURCES "*.cpp")

add_executable(lox_benchmark ${SOURCES})
target_include_directories(lox_benchmark PRIVATE ${PROJECT_SOURCE_DIR}/include
                                                 ${CMAKE_BINARY_DIR})
target_link_libraries(lox_benchmark PRIVATE lox_core benchmark)
