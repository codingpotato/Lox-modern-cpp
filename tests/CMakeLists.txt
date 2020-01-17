list(APPEND CMAKE_MODULE_PATH "${catch_SOURCE_DIR}/contrib")
include(CTest)
include(ParseAndAddCatchTests)

function(add_test_executable)
  cmake_parse_arguments(ADD
                        OPTIONS
                        NAME
                        SOURCES
                        ${ARGN})
  add_executable(${ADD_NAME} ${ADD_SOURCES})
  target_include_directories(${ADD_NAME}
                             PRIVATE ${catch_SOURCE_DIR}/single_include)
  target_link_libraries(${ADD_NAME} lox-interpreter)
  parseandaddcatchtests(${ADD_NAME})
endfunction(add_test_executable)

add_test_executable(NAME
                    cache_tests
                    SOURCES
                    cache_tests.cpp)

add_test_executable(NAME
                    interpreter_tests
                    SOURCES
                    interpreter_tests.cpp)

add_test_executable(NAME
                    parser_tests
                    SOURCES
                    parser_tests.cpp)

add_test_executable(NAME
                    program_tests
                    SOURCES
                    program_tests.cpp)

add_test_executable(NAME
                    scanner_tests
                    SOURCES
                    scanner_tests.cpp)

add_test_executable(NAME
                    statement_tests
                    SOURCES
                    statement_tests.cpp)

add_test_executable(NAME
                    to_string_tests
                    SOURCES
                    to_string_tests.cpp)

add_test_executable(NAME
                    value_tests
                    SOURCES
                    value_tests.cpp)
