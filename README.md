# Introduction
For a Football Manager game i have created this Football simulator.
the goal is to accurately simulate matches based on certain player abilities.
It generates events for goals and corners. It has support for penalties and aggregate matches in tournaments
This software does not have any graphics. The simulator is ment to serve in a backend to play matches.
It is highly optimized and can run thousands of matches simultanously.
The simulator talks to your application via a socket. Your application can receive all possible events for a football match:
Goals, fouls, halftime, penalties etc.

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

* {binary} = compile executable
* {matchspeed} = multplier of match speed. 1 = 90 minutes. 10 = 9 minutes etc
* {socket} = socket path for communication with the program. If you want to receive events during the game you need to open a socket in your application which can communicate with this process.
* {data} = This data is contains all information to play a match: The data must be in the following JSON format:

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

The localteam and visitorteam properties are containing each 11 players.
Each player has the following data properties:
* id - must be an unique numbeer
* att - this represents attack skill. The higher the number the better attack skills the player has.
* def - this represents defense skills.
* position - position of the player. The field is devided into sectors. Each player must be put in a unique sector before the match starts.
The field is 7 by 6 long. 0,0 represents lower left part of the field. 7,6 upper right.
You can setup different formations types by positioning players differently. In the JSON
above we have a 4-3-3 formation.
* type - either, def, mid or att. This says something about the wanted skill level of the player.
You can position a def player in an attack position, but that would not really make sense.

## sockets and communication
Each game opens a socket which gives your application the possibility to communicate and receive data.
Usually on unix systems this is just a file handler. In most languages you have the ability to open a socket.
The path to this socket is given to the football simulator.
Data on a socket is received in plain text. In order to know when you received a complete data part, the data has termination markers.
These termination markers are always marked with |||
see server.js for an implementation example.
Every data part is in JSON format and contains a "type" property. You can use this type property
to identify the type of event. For example goal, foul, halftime, fulltime etc.

