# Introduction
For a Football Manager game i have created this Football simulator.
the goal is to accurately simulate matches based on certain player abilities.
It generates events for goals and corners. It has support for penalties and aggregate matches in tournaments
This software does not have any graphics. The simulator is ment to serve in a backend to play matches.
It is highly optimized and can run thousands of matches simultanously.

# Install

### on linux
1. ccmake .
2. enter configure and generate
3. cmake OR cmake --build . --target footballsimulator -- -j 8


### on macos
1. /Applications/CLion.app/Contents/bin/cmake/bin/cmake .
or /Applications/CMake.app/Contents/bin/cmake --build . -- -j 8

# How to use.
The binary can be run from command line directly or wrapped in your preferred software.
This repo contains a nodejs example implementation.
Pay attention to the required parameters when running the command:

{binary} {matchspeed} {socket} {data}

{binary} = compile executable
{matchspeed} = multplier of match speed. 1 = 90 minutes. 10 = 9 minutes etc
{socket} = socket path for communication with the program. If you want to receive events during the game you need to open a socket in your application which can communicate with this process.
{data} = This data is contains all information to play a match: The data must be in the following JSON format:

```javascript
var data =
            {
                "id" : matchId+Math.random()*10000000, //unique identifier for a match. must be unique when using paralell
                "matchSpeed" : speed, ///multplier of match speed. 1 = 90 minutes. 10 = 9 minutes etc
                "requireWinner" : 1, //if 1 a match will end with score difference or penalties
                "useAggregateHistory" : 0,
                "aggr_history_local_away_goals" :0, //Aggregation score of a previous match
                "aggr_history_visitor_away_goals" :0,
                "aggr_history_local_home_goals" :0,
                "aggr_history_visitor_home_goals" :0,
                "localteam":
                    [{"id":1,"att":50,"def":50,"position":[0,3],"type":"goal"},
                        {"id":2,"att":150,"def":50,"position":[1,0],"type":"def"},
                        {"id":3,"att":150,"def":50,"position":[1,2],"type":"def"},
                        {"id":4,"att":50,"def":50,"position":[1,4],"type":"def"},
                        {"id":5,"att":50,"def":50,"position":[1,6],"type":"def"},
                        {"id":6,"att":50,"def":50,"position":[3,1],"type":"mid"},
                        {"id":7,"att":50,"def":50,"position":[3,3],"type":"mid"},
                        {"id":8,"att":150,"def":50,"position":[3,5],"type":"mid"},
                        {"id":9,"att":150,"def":50,"position":[5,1],"type":"att"},
                        {"id":10,"att":50,"def":50,"position":[5,3],"type":"att"},
                        {"id":11,"att":50,"def":50,"position":[5,5],"type":"att"},
                    ],
                "visitorteam":
                    [{"id":12,"att":50,"def":50,"position":[7,3],"type":"goal"},
                        {"id":13,"att":50,"def":50,"position":[6,0],"type":"def"},
                        {"id":14,"att":50,"def":50,"position":[6,2],"type":"def"},
                        {"id":15,"att":50,"def":50,"position":[6,4],"type":"def"},
                        {"id":16,"att":50,"def":50,"position":[6,6],"type":"def"},
                        {"id":17,"att":50,"def":50,"position":[4,1],"type":"mid"},
                        {"id":18,"att":50,"def":50,"position":[4,3],"type":"mid"},
                        {"id":19,"att":50,"def":50,"position":[4,5],"type":"mid"},
                        {"id":20,"att":50,"def":50,"position":[2,1],"type":"att"},
                        {"id":21,"att":50,"def":50,"position":[2,3],"type":"att"},
                        {"id":22,"att":50,"def":50,"position":[2,5],"type":"att"},
                    ]
            }

```