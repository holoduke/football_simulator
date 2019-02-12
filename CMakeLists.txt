cmake_minimum_required(VERSION 3.6)
project(footballsimulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2 -DNDEBUG")



set(SOURCE_FILES main.cpp game.cpp game.h Lineup.cpp Lineup.h Player.cpp Player.h Position.cpp Position.h Team.cpp Team.h Util.cpp Util.h Communication.cpp Communication.h MoveOption.cpp MoveOption.h MatchReport.cpp MatchReport.h)
add_executable(footballsimulator ${SOURCE_FILES})
