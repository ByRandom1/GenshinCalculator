cmake_minimum_required(VERSION 3.23)
project(GenshinCalculator)

set(CMAKE_CXX_STANDARD 17)

add_executable(GenshinCalculator main.cpp Artifact.cpp Artifact.h Character.cpp Character.h Condition.h Weapon.cpp Weapon.h Group.cpp Group.h Reinforced_Artifact.cpp Reinforced_Artifact.h)

INCLUDE_DIRECTORIES(/usr/local/Cellar/jsoncpp/1.9.5/include)
LINK_DIRECTORIES(/usr/local/Cellar/jsoncpp/1.9.5/lib)

file(GLOB LIBRARIES "/usr/local/Cellar/jsoncpp/1.9.5/lib/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

TARGET_LINK_LIBRARIES(GenshinCalculator ${LIBRARIES})