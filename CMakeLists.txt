cmake_minimum_required(VERSION 3.20)
project(action_sample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Delayed template parsing makes clang accept MSVC-style leniency (missing 'template' keyword on dependent calls).
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-fdelayed-template-parsing -Wno-delayed-template-parsing-in-cxx20)
endif()

foreach(target test_nike_shoe_factory delegate_static_assertions)
  add_executable(${target} action_sample/${target}.cpp)
  target_include_directories(${target} PRIVATE action_sample)
endforeach()
