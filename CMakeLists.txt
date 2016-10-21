project( ml )

cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(
  -std=c++11
  -w
  -lpthread
  -Ofast
  -lncurses
  # -g
  )

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

file(

  GLOB_RECURSE

  source_files

  src/*

  )

add_executable(

  ml

  ${source_files}

  )
target_link_libraries(ml -lpthread -lncurses -lsfml-graphics -lsfml-window -lsfml-system)
