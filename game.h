//
// Created by gillis on 25/12/2016.
//

#ifndef FOOTBALLSIMULATOR_GAME_H
#define FOOTBALLSIMULATOR_GAME_H

#include <iostream>
#include <ctime>
#include <thread>
#include "lib/easylogginpp++.h"
#include "Lineup.h"
#include "Communication.h"
#include "Team.h"

namespace game {

    class GameOutputWriter {

    public:
        void onStart(){
            LOG(INFO) << "output writer onStart";
        }
        void onFinish(){
            LOG(INFO) << "output writer onFinish";
        }
        void onHalfTimeStart(){
            LOG(INFO) << "output writer onHalfTimeStarts";
        }
        void onHalfTimeFinish(){
            LOG(INFO) << "output writer onHalfTimeFinish";
        }
    };

    class GameOptions {

    public:

        GameOptions(Lineup &lineup, Team &localteam, Team &visitorteam, Communication &communicator) : lineup(lineup),localteam(localteam),visitorteam(visitorteam),communicator(communicator){

        }

        Lineup &lineup;
        Communication &communicator;
        Team &localteam;
        Team &visitorteam;
        int duration = 45;
        int halftimeDuration = 15;
        int firstBreakDuration = 5;
        int extraTimeDuration = 15;
        int secondBreakDuration = 5;
        int thirdBreakDuration = 5;
        int breakBetweenPenalty = 1;

        int stepsPerMinute = 18;
        long multiplier = 100000;
        bool winnerRequired = false;
        bool useAggregate = false;
        int aggr_local_home_goals = 0;
        int aggr_visitor_home_goals = 0;
        int aggr_local_away_goals = 0;
        int aggr_visitor_away_goals = 0;
    };

    class Game {

    private:
        void runLoop();
        void initStartValues();
        void resetBallPosition(Player::Side side);
        bool homeCanStart;
        int totalSteps;
        int halfTimeSteps;
        int extraTimeSteps;
        int currentHalfTimeStep;
        int currentFullTimeStep;
        int currentFirstBreakStep;
        int currentExtraTimeStep;
        int currentSecondBreakStep;
        int currentThirdBreakStep;
        int currentPenaltyBetweenBreakStep;
        int firstBreakSteps;
        int secondBreakSteps;
        int thirdBreakSteps;
        int timeBetweenPenalty;
        int stepTimeout;

    public:
        Game(GameOptions const&options) : options(options) {
            status = READY_TO_START;
            initStartValues();
        }

        enum MatchStatus {
            READY_TO_START, FT, HT, PL, FIRST_HALF_PLAYING, SECOND_HALF_PLAYING, BREAK1, BREAK2, BREAK3, ET_FIRST_HALF, ET_SECOND_HALF, AET, PEN, PEN_BREAK, P

            //break1 = pause after FT with draw game but winner required
            //ET extra time
            //break2 . break between extra time
            //break2 . break after extra time and penalties
            //AET . match finished after extra time
            //PEN.  penalties are taken.
            //PENBREAK break between penalties
            //P.  match finished after penalties
        };

        void start() {
            LOG(INFO) << "Start game";
            runLoop();
        };

        GameOptions options;
        MatchStatus status;
        Position ballPosition = Position(0,0);
    };
}


#endif //FOOTBALLSIMULATOR_GAME_H
