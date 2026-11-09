add_executable(consumer_cpp14 consumer.cpp)
target_link_libraries(consumer_cpp14 PRIVATE autoeth::runtime)
target_compile_options(consumer_cpp14 PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>)