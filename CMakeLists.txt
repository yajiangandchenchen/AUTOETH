cmake_minimum_required(VERSION 3.20)

project(autoeth
  VERSION 0.1.0
  DESCRIPTION "Automotive Ethernet communication and end-to-end protection SDK"
  LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(AUTOETH_BUILD_SHARED "Build shared libraries" ON)
option(AUTOETH_BUILD_STATIC "Build static libraries" ON)
option(AUTOETH_BUILD_TESTS "Build the unit tests" ON)
option(AUTOETH_BUILD_CONSUMERS "Build the independent C99 and C++14 consumers" ON)

# Off by default. Only the static-analysis scanner turns this on, to adapt its
# own host include layout; product builds never see it.
option(AUTOETH_ANALYSIS_HOST "Apply the static-analysis host header adaptation" OFF)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/autoethAnalysis.cmake.in)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(autoeth_warnings INTERFACE)
target_compile_options(autoeth_warnings INTERFACE
  $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>)

set(AUTOETH_PUBLIC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(autoeth_add_library_base base_name)
  set(kind ${ARGV1})
  if(kind STREQUAL "SHARED")
    add_library(${base_name} SHARED ${ARGN})
    target_compile_definitions(${base_name} PUBLIC AUTOETH_SHARED)
  else()
    add_library(${base_name} STATIC ${ARGN})
    target_compile_definitions(${base_name} PUBLIC AUTOETH_STATIC)
  endif()
  target_include_directories(${base_name}
    PUBLIC
      $<BUILD_INTERFACE:${AUTOETH_PUBLIC_INCLUDE_DIRS}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  target_link_libraries(${base_name} PUBLIC Threads::Threads)
  target_link_libraries(${base_name} PRIVATE autoeth_warnings)
  set_target_properties(${base_name} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})
endfunction()

set(AUTOETH_CORE_SOURCES
  src/core/check.cpp
  src/core/build_info.cpp
  src/core/crc.cpp
  src/core/version_info.cpp
  src/core/wire.cpp
  src/core/tp.cpp
  src/core/codec.cpp
  src/core/discovery.cpp)

set(AUTOETH_E2E_SOURCES
  src/e2e/p01.cpp
  src/e2e/p02.cpp
  src/e2e/p04.cpp
  src/e2e/p04m.cpp
  src/e2e/p05.cpp
  src/e2e/p06.cpp
  src/e2e/p07.cpp
  src/e2e/p07m.cpp
  src/e2e/p08.cpp
  src/e2e/p08m.cpp
  src/e2e/p11.cpp
  src/e2e/p22.cpp
  src/e2e/p44.cpp
  src/e2e/p44m.cpp
  src/e2e/e2e_wide.cpp
  src/e2e/sm.cpp)

set(AUTOETH_RUNTIME_SOURCES
  src/runtime/config.cpp
  src/runtime/runtime.cpp
  src/classic_host/classic_modules.cpp
  src/classic_host/classic_transformers.cpp
  src/classic_host/classic_binding.cpp)

set(autoeth_library_targets)
set(autoeth_all_targets)

foreach(kind IN ITEMS SHARED STATIC)
  if(kind STREQUAL "SHARED" AND NOT AUTOETH_BUILD_SHARED)
    continue()
  endif()
  if(kind STREQUAL "STATIC" AND NOT AUTOETH_BUILD_STATIC)
    continue()
  endif()
  string(TOLOWER ${kind} suffix)
  autoeth_add_library_base(autoeth_core_${suffix} ${kind} ${AUTOETH_CORE_SOURCES})
  autoeth_add_library_base(autoeth_e2e_${suffix} ${kind} ${AUTOETH_E2E_SOURCES})
  target_link_libraries(autoeth_e2e_${suffix} PUBLIC autoeth_core_${suffix})
  autoeth_add_library_base(autoeth_runtime_${suffix} ${kind} ${AUTOETH_RUNTIME_SOURCES})
  target_link_libraries(autoeth_runtime_${suffix} PUBLIC autoeth_e2e_${suffix} autoeth_core_${suffix})

  add_library(autoeth::${kind}::core ALIAS autoeth_core_${suffix})
  add_library(autoeth::${kind}::e2e ALIAS autoeth_e2e_${suffix})
  add_library(autoeth::${kind}::runtime ALIAS autoeth_runtime_${suffix})
  list(APPEND autoeth_library_targets
    autoeth_core_${suffix} autoeth_e2e_${suffix} autoeth_runtime_${suffix})
endforeach()

if(NOT autoeth_library_targets)
  message(FATAL_ERROR "At least one of AUTOETH_BUILD_SHARED/AUTOETH_BUILD_STATIC must be ON")
endif()

# The documented imported names must resolve in either library kind, so a
# static-only or shared-only build stays consumable (R0.4.2 / R0.4.3).
if(TARGET autoeth_core_shared)
  set(AUTOETH_PREFERRED_KIND shared)
else()
  set(AUTOETH_PREFERRED_KIND static)
endif()
add_library(autoeth::core ALIAS autoeth_core_${AUTOETH_PREFERRED_KIND})
add_library(autoeth::e2e ALIAS autoeth_e2e_${AUTOETH_PREFERRED_KIND})
add_library(autoeth::runtime ALIAS autoeth_runtime_${AUTOETH_PREFERRED_KIND})

add_executable(autoethd src/apps/autoethd/main.cpp)
target_link_libraries(autoethd PRIVATE autoeth::runtime autoeth_warnings)
list(APPEND autoeth_all_targets autoethd)

add_executable(autoeth-client src/apps/autoeth_client/main.cpp)
target_link_libraries(autoeth-client PRIVATE autoeth::runtime autoeth_warnings)
list(APPEND autoeth_all_targets autoeth-client)

add_executable(autoethctl src/apps/autoethctl/main.cpp)
target_link_libraries(autoethctl PRIVATE autoeth::runtime autoeth_warnings)
list(APPEND autoeth_all_targets autoethctl)

install(TARGETS ${autoeth_library_targets} ${autoeth_all_targets} autoeth_warnings
  EXPORT autoethTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY include/autoeth DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY share/autoeth DESTINATION ${CMAKE_INSTALL_DATADIR})

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/autoethConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/autoethConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/autoethConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/autoeth)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/autoethConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/autoethConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/autoeth)

install(EXPORT autoethTargets
  NAMESPACE autoeth::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/autoeth
  FILE autoethTargets.cmake)

# Native clang tooling (clang-tidy) resolves a compilation database by walking
# up from each source file, so a database that only lives in build/ is not found
# for sources under src/. Publish an identical copy at the source root when the
# analysis host adapter is enabled; product builds leave the tree untouched.
if(AUTOETH_ANALYSIS_HOST AND CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_target(autoeth_publish_compile_db ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    COMMENT "Publishing compile_commands.json for native analysis tooling")
endif()

if(AUTOETH_BUILD_CONSUMERS)
  add_subdirectory(examples/consumer_c99)
  add_subdirectory(examples/consumer_cpp14)
endif()

if(AUTOETH_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/unit)
endif()