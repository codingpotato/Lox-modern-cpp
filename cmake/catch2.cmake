include(FetchContent)

message("-- Download catch2")

FetchContent_Declare(catch
                     GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                     GIT_TAG v2.11.1)

FetchContent_GetProperties(catch)
if(NOT catch_POPULATED)
  FetchContent_Populate(catch)
  add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif()
