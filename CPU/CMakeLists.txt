set(HEADER_LIST inc/cpu.h)
set(SRC_LIST src/cpu.c)

add_library(CPU STATIC ${SRC_LIST} ${HEADER_LIST})

target_include_directories(CPU PUBLIC . inc src)