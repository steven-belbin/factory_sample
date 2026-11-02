cmake_minimum_required(VERSION 3.20)
project(action_sample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

foreach(target test_nike_shoe_factory delegate_static_assertions)
  add_executable(${target} action_sample/${target}.cpp)
  target_include_directories(${target} PRIVATE action_sample)
endforeach()
