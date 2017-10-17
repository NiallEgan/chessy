cmake_minimum_required(VERSION 3.8)
project(chessy)


include(CTest)

set(CMAKE_C_STANDARD 99)
SET(GCC_COVERAGE_COMPILE_FLAGS "-O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})


set(SOURCE_FILES src/main.c src/board.h src/board.c src/move.h src/move_gen.c src/print.c src/main.h src/perft.h src/move.c src/eval.h src/init.c src/move_parsing.c src/fen.c src/perft_tests.c src/main_search.c src/search.h src/transposition_table.c src/search_tests.c src/end.c src/quescience.c src/eval.c src/x_board.c)
add_executable(chessy ${SOURCE_FILES})