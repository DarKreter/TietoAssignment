set(DEPENDENCIES_LIST CPU Threads::Threads Threads)

add_executable(firmware src/main.c src/test.c)

target_include_directories(firmware PUBLIC . src inc)

add_dependencies(firmware ${DEPENDENCIES_LIST})
target_link_libraries(firmware PUBLIC ${DEPENDENCIES_LIST})
