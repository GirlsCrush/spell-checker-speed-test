# Copyright 2018, GlobalLogic Inc.
# All rights reserved.
#

cmake_minimum_required(VERSION 3.1)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(spell_checker)

add_executable(spell_checker_gTest
    project/spell_checker.h
    project/spell_checker.cpp
    test/spell_checker_gTest.cpp
)
target_include_directories(spell_checker_gTest PUBLIC
    project
)
set_target_properties(spell_checker_gTest PROPERTIES
    CXX_STANDARD 14
)

target_sources(spell_checker_gTest PRIVATE
    $ENV{GTEST_DIR}/src/gtest-all.cc
)
target_include_directories(spell_checker_gTest PRIVATE
    $ENV{GTEST_DIR}
    $ENV{GTEST_DIR}/include
)
target_link_libraries(spell_checker_gTest
    pthread
)
