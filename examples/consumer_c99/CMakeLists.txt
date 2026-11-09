add_executable(consumer_c99 consumer.c)
target_link_libraries(consumer_c99 PRIVATE autoeth::runtime)
target_compile_options(consumer_c99 PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang>:-std=c99 -Wall -Wextra -Wpedantic>)