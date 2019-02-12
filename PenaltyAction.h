//
// Created by gillis on 01/03/2017.
//
#include "Player.h"

#include "lib/easylogginpp++.h"
#include <vector>

#ifndef FOOTBALLSIMULATOR_PenaltyAction_H
#define FOOTBALLSIMULATOR_PenaltyAction_H

class PenaltyAction{

public:

    PenaltyAction(){

    }
    ~PenaltyAction() {

    }

    Player::Side currentSide;
    Player * shooter;
    int take;
    int round;
    float hitChance;
};

#endif //FOOTBALLSIMULATOR_PenaltyAction_H
