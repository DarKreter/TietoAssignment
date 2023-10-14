set(DEPENDENCIES_LIST CPU)

add_executable(firmware main.c)

target_include_directories(firmware PUBLIC .)

add_dependencies(firmware ${DEPENDENCIES_LIST})
target_link_libraries(firmware PUBLIC ${DEPENDENCIES_LIST})
