add_executable(autoeth_tests test_main.cpp)
target_link_libraries(autoeth_tests PRIVATE autoeth::runtime)
target_include_directories(autoeth_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(autoeth_tests PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>)

add_test(NAME autoeth_unit COMMAND autoeth_tests)