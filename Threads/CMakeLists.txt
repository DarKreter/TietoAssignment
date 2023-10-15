set(HEADER_LIST inc/Threads.h inc/utils.h)
set(SRC_LIST src/Threads.c src/utils.c)
set(DEPENDENCIES_LIST CPU)


add_library(Threads STATIC ${SRC_LIST} ${HEADER_LIST})

target_include_directories(Threads PUBLIC . inc src)

add_dependencies(Threads ${DEPENDENCIES_LIST})
target_link_libraries(Threads PUBLIC ${DEPENDENCIES_LIST})
