cmake_minimum_required(VERSION 3.23)
project(GenshinCalculator)

set(CMAKE_CXX_STANDARD 17)

add_executable(GenshinCalculator main.cpp Artifact.cpp Artifact.h Character.cpp Character.h Condition.h Weapon.cpp Weapon.h Group.cpp Group.h)
