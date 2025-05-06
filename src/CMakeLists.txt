add_library(club STATIC event.cpp config.cpp club.cpp)
target_include_directories(club PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(club PRIVATE $<$<CONFIG:Debug>:-Wall;-Wextra;-Wpedantic;-fno-omit-frame-pointer;-fsanitize=address>)
target_link_options(club PUBLIC $<$<CONFIG:Debug>:-fno-omit-frame-pointer;-fsanitize=address>)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
add_executable(cmd main.cpp)
set_target_properties(cmd PROPERTIES OUTPUT_NAME task)
target_link_libraries(cmd PRIVATE club)
