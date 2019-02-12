//
// Created by gillis on 01/03/2017.
//
#include "Player.h"

#include "lib/easylogginpp++.h"
#include <vector>


#ifndef FOOTBALLSIMULATOR_MOVEOPTION_H
#define FOOTBALLSIMULATOR_MOVEOPTION_H

class MoveOptionInterceptor{

public:

    MoveOptionInterceptor(){

    }
    ~MoveOptionInterceptor() {

    }

    enum class InterceptionType {UNDEFINED, Frontal_pass,Pass_heading_in_direction_to,Pass_heading_partly_in_direction_to,Pass_heading_not_in_direction_to};
    enum class InterceptorPosition {In_front_player, Behind_player, Behind_target_player};
    InterceptionType type;
    InterceptorPosition positionType;
    Player * player;
    float distanceToBall;
    float distanceToPlayer;
    bool behindPlayer;
    bool behindTargetPlayer;
    double blockChance = 0;
};

struct MoveOptionInterceptorSort {
    bool operator()(const MoveOptionInterceptor *a, const MoveOptionInterceptor *b) const {
        return a->distanceToBall < b->distanceToBall;
    }
};

class MoveOption{

public:
    MoveOption(){

    }
    //cleanup interceptors
    ~MoveOption() {
        for (int i=0; i < interceptors.size(); i++){
            delete ((MoveOptionInterceptor *)interceptors[i]);
        }
        interceptors.clear();
    }

    enum class MoveStyle {Attack,Neutral,Defense};
    enum class ShotTypes {Goal_long_shot,Goal_short_shot,Long_pass,Short_pass};
    MoveStyle moveStyle;
    ShotTypes shotType;
    int attackDirection; //the direction of attack. for home = 1, away = -1
    int direction;
    Player * fromPlayer;
    Player * targetPlayer;
    Player * whenLostPassTo;
    float distance;
    double goalKeeperBlockChance = 0;
    bool result;
    bool isCorner;
    bool isCornerGoal;
    std::vector<MoveOptionInterceptor*> interceptors;

    //corner
    double cornerShooterChance = 0;
    double cornerChance = 0;
    double cornerGoalChance = 0;
    double missedShotResultedInCornerGoalChance = 0;
    Player * cornerShooter;
    Player * cornerGoalShooter;
};

struct MoveOptionSort {
    bool operator()(const MoveOption *a, const MoveOption *b) const {
        if (a->moveStyle == MoveOption::MoveStyle::Attack) return -1;
        if (a->moveStyle == MoveOption::MoveStyle::Neutral && b->moveStyle == MoveOption::MoveStyle::Defense) return 0;
        return 1;
    }
};

#endif //FOOTBALLSIMULATOR_MOVEOPTION_H
